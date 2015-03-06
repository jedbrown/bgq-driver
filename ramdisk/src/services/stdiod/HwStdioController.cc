/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

//! \file  HwStdioController.cc
//! \brief Methods for bgcios::stdio::HwStdioController class.

// Includes
#include "HwStdioController.h"
#include <ramdisk/include/services/common/RdmaError.h>
#include <ramdisk/include/services/common/RdmaDevice.h>
#include <ramdisk/include/services/common/RdmaCompletionQueue.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <poll.h>
#include <errno.h>
#include <iomanip>
#include <sstream>
#include <queue>
#include <ramdisk/include/services/common/Cioslog.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include <sys/socket.h>

using namespace bgcios::stdio;

LOG_DECLARE_FILE("cios.stdiod");


HwStdioController::HwStdioController(StdioConfigPtr config) : StdioController(config)
{
   // Set work directory.
   _workDirectory = bgcios::WorkDirectory;

   // Build the path to the iosd command channel.
   std::ostringstream iosdPath;
   iosdPath << _workDirectory << bgcios::IosctlCommandChannelName;
   _iosdCmdChannelPath = iosdPath.str();
}

HwStdioController::~HwStdioController()
{
   _dataListener.reset();
   _dataChannel.reset();
   free(_inboundMessage);
   free(_outboundMessage);
}

int
HwStdioController::startup(in_port_t dataChannelPort)
{
   // Build the path to the command channel.
   std::ostringstream cmdChannelPath;
   cmdChannelPath << _workDirectory << bgcios::StdioCommandChannelName;

   // Create the command channel socket.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(cmdChannelPath.str()));
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error creating command channel '" << cmdChannelPath.str() << "': " << e.what());
      return e.errcode();
   }

   // Create listening socket for data channel.
   try {
      _dataListener = bgcios::InetStreamSocketPtr(new bgcios::InetStreamSocket(dataChannelPort, true));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating listening socket for data channel: " << e.what());
      _dataListener.reset();
      return e.errcode();
   }

   // Prepare socket to listen for connections.
   try {
      _dataListener->listen(1);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error listening for new data channel connections: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("listening for new data channel connections on fd " << _dataListener->getSd() << " using address " << _dataListener->getName());

   // Find the address of the I/O link device.
   RdmaDevicePtr linkDevice;
   try {
      linkDevice = RdmaDevicePtr(new RdmaDevice(bgcios::RdmaDeviceName, bgcios::RdmaInterfaceName));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error opening InfiniBand device: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created InfiniBand device for " << linkDevice->getDeviceName() << " using interface " << linkDevice->getInterfaceName());

   // Create listener for RDMA connections.
   try {
      _rdmaListener = bgcios::RdmaServerPtr(new bgcios::RdmaServer(linkDevice->getAddress(), BaseRdmaPort));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error creating listening RDMA connection: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created listening RDMA connection on port " << BaseRdmaPort);

   // Create a protection domain object.
   try {
      _protectionDomain = RdmaProtectionDomainPtr(new RdmaProtectionDomain(_rdmaListener->getContext()));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error allocating protection domain: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created protection domain " << _protectionDomain->getHandle());

   // Create a completion channel object.
   try {
      _completionChannel = RdmaCompletionChannelPtr(new RdmaCompletionChannel(_rdmaListener->getContext(), false)); 
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error constructing completion channel: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created completion channel using fd " << _completionChannel->getChannelFd());

   // Listen for connections.
   int err = _rdmaListener->listen(256);
   if (err != 0) {
      LOG_ERROR_MSG("error listening for new RDMA connections: " << bgcios::errorString(err));
      return err;
   }
   LOG_CIOS_DEBUG_MSG("listening for new RDMA connections on fd " << _rdmaListener->getEventChannelFd());

   return 0;
}

int
HwStdioController::cleanup(void)
{
   return 0;
}

void
HwStdioController::eventMonitor(void)
{
   const int cmdChannel   = 0;
   const int dataChannel  = 1;
   const int compChannel  = 2;
   const int eventChannel = 3;
   const int dataListener = 4;
   const int dataChanAuthWaiter= 5;
   const int numFds       = 6;

   int note_error = 0;
   
   FlightLogDumpWaiter flightLogDumpWait("stdiod");
   flightLogDumpWait.start();  //call the thread wrapper which invokes run

   pollfd pollInfo[numFds];
   int timeout = -1; // 10000 == 10 sec

   // Initialize the pollfd structure.
   pollInfo[cmdChannel].fd = _cmdChannel->getSd();
   pollInfo[cmdChannel].events = POLLIN;
   pollInfo[cmdChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

   pollInfo[dataChannel].fd = _dataChannel == NULL ? -1 : _dataChannel->getSd();
   pollInfo[dataChannel].events = POLLIN;
   pollInfo[dataChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added data channel using fd " << pollInfo[dataChannel].fd << " to descriptor list");

   pollInfo[compChannel].fd = _completionChannel->getChannelFd();
   pollInfo[compChannel].events = POLLIN;
   pollInfo[compChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added completion channel using fd " << pollInfo[compChannel].fd << " to descriptor list");

   pollInfo[eventChannel].fd = _rdmaListener->getEventChannelFd();
   pollInfo[eventChannel].events = POLLIN;
   pollInfo[eventChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added event channel using fd " << pollInfo[eventChannel].fd << " to descriptor list");

   pollInfo[dataListener].fd = _dataListener->getSd();
   pollInfo[dataListener].events = POLLIN;
   pollInfo[dataListener].revents = 0;
   LOG_CIOS_TRACE_MSG("added data channel listener using fd " << pollInfo[dataListener].fd << " to descriptor list");

   pollInfo[dataChanAuthWaiter].fd = _dataChanAuthWaiter == NULL ? -1 : _dataChanAuthWaiter->getSd();
   pollInfo[dataChanAuthWaiter].events = POLLIN;
   pollInfo[dataChanAuthWaiter].revents = 0;
   LOG_CIOS_TRACE_MSG("added _dataChanAuthWaiter using fd " << pollInfo[dataListener].fd << " to descriptor list");

   // Process events until told to stop.
   while (!_done) {

      // Wait for an event on one of the descriptors.
      int rc = poll(pollInfo, numFds, timeout);

      // There was no data so try again.
      if (rc == 0) {
         continue;
      } 

      // There was an error so log the failure and try again.
      if (rc == -1) {
         int err = errno;
         if (err == EINTR) {
            LOG_CIOS_WARN_MSG("poll returned errno=EINTR, continuing ...");
            continue;
         }

         LOG_ERROR_MSG("error polling socket descriptors: " << bgcios::errorString(err));
         return;
      } 

      // Check for an event on the command channel.
      if (pollInfo[cmdChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on command channel");
         commandChannelHandler();
         pollInfo[cmdChannel].revents = 0;
      }

      // Check for an event on the data channel listener.
      if (pollInfo[dataListener].revents & POLLIN) {
         LOG_INFO_MSG_FORCED("input event available on data channel listener");
         pollInfo[dataListener].revents = 0;
        
         _dataChanAuthWaiter.reset();//drop any previous waiting connection
         _dataChanAuthWaiter = makeDataChannel();
         if (!_dataChanAuthWaiter) {
            LOG_ERROR_MSG("error making new data channel");
         }
         else {
            // Set the send buffer size for the data channel.
            try {
                  _dataChanAuthWaiter->setSendBufferSize(_config->getSendBufferSize());
                 }
             catch (SocketError& e) {
                 LOG_ERROR_MSG("error setting send buffer size for data channel: " << e.what());
                 _dataChanAuthWaiter.reset();
             }
         }
         if (!_dataChanAuthWaiter) {
            pollInfo[dataChanAuthWaiter].fd = -1;
         }
         else {
           pollInfo[dataChanAuthWaiter].fd = _dataChanAuthWaiter->getSd();
           LOG_INFO_MSG_FORCED("waiting data channel is connected to " << _dataChanAuthWaiter->getPeerName() << " using fd " << _dataChanAuthWaiter->getSd());
         }
      }

      // Check for an event on the completion channel.
      if (pollInfo[compChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on completion channel");
         completionChannelHandler();
         pollInfo[compChannel].revents = 0;
         if (!_dequeStdioMsgInClient.empty() ){
            pollInfo[dataChannel].events =  POLLOUT|POLLIN;
            //f (pollInfo[dataChannel].fd != -1)pollInfo[dataChannel].revents |= POLLOUT;//poll on datachannel section forced to run for sending data on datachannel
         }
      }

      // Check for an event on the data channel.
      if (pollInfo[dataChannel].revents)
      {
        
        if (pollInfo[dataChannel].revents & (POLLERR|POLLHUP|POLLNVAL) ) {//error!
          LOG_CIOS_WARN_MSG("data channel dropped, pollInfo[dataChannel].revents & (POLLERR|POLLHUP|POLLNVAL)");
          _dataChannel.reset();
          pollInfo[dataChannel].revents = 0; //no more received event processing
          pollInfo[dataChannel].fd = -1;      
        }
        
        if (pollInfo[dataChannel].revents & POLLIN) {
          LOG_CIOS_TRACE_MSG("input event available on data channel");
          if ( (note_error = dataChannelHandler()) ) {
             _dataChannel.reset();
             pollInfo[dataChannel].fd = -1;
             pollInfo[dataChannel].revents = 0; //no more received event processing
             LOG_CIOS_WARN_MSG("pollInfo[dataChannel].revents & POLLIN encountered recv error"<<strerror(note_error)<<"("<<note_error<<")" );
          }
        }
        if (pollInfo[dataChannel].revents & POLLOUT){
          if (!_dequeStdioMsgInClient.empty() ) {
            RdmaClientPtr client = _dequeStdioMsgInClient.front();
            //need to check if writing of stdio worked OK?
            if ( (note_error = writeStdio(client)) ) {
              _dataChannel.reset();
              pollInfo[dataChannel].fd = -1;
              pollInfo[dataChannel].revents = 0; //no more received event processing
              LOG_CIOS_WARN_MSG("pollInfo[dataChannel].revents & POLLOUT encountered send error "<<strerror(note_error)<<"("<<note_error<<")" );
            }
          }
          if (_dequeStdioMsgInClient.empty() ) {
            pollInfo[dataChannel].events = POLLIN;  
          }
        }

        pollInfo[dataChannel].revents = 0;
      } //endif dataChannel received events

      // Check for an event on the event channel.
      if (pollInfo[eventChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on event channel");
         eventChannelHandler();
         pollInfo[eventChannel].revents = 0;
      }

      // Check for an event on _dataChanAuthWaiter
      if (pollInfo[dataChanAuthWaiter].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on _dataChanAuthWaiter");
         pollInfo[dataChanAuthWaiter].revents = 0;
        
         // Handle the Authenticate message which must be sent first.
         if (!dataChannelHandler(_dataChanAuthWaiter)) {
             _dataChannel = _dataChanAuthWaiter;
             pollInfo[dataChannel].fd = _dataChannel->getSd();
             pollInfo[dataChannel].events =  POLLIN|POLLOUT;
             _dataChanAuthWaiter.reset();
             pollInfo[dataChanAuthWaiter].fd = -1;
             LOG_INFO_MSG_FORCED("data channel is authenticated with " << _dataChannel->getPeerName() << " using fd " << _dataChannel->getSd());
         }
         else {
             LOG_INFO_MSG_FORCED("data channel FAILED authenticating " << _dataChannel->getPeerName() << " using fd " << _dataChannel->getSd());
             _dataChanAuthWaiter.reset();
             pollInfo[dataChanAuthWaiter].fd = -1;
             
         }
      }
   }

   // Reset for next time.
   _done = 0;

   return;
}

int
HwStdioController::commandChannelHandler(void)
{
   // Receive a message from the command channel.
   std::string source;
   int err = recvFromCommandChannel(source, _inboundMessage);

   // An error occurred receiving a message.
   if (err != 0) {
      LOG_ERROR_MSG("error receiving message from command channel: " << bgcios::errorString(err));
      if (err == EPIPE) { // When command channel closes, stop handling events.
         _done = true;
      }
      return err;
   }


   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
   CIOSLOGMSG(CMD_RECV_MSG,msghdr);
   // Make sure the service field is correct.
   if ((msghdr->service != bgcios::StdioService) && (msghdr->service != bgcios::IosctlService) && (msghdr->service != bgcios::ToolctlService)) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << (int)msghdr->service << " is wrong, header: " << bgcios::printHeader(*msghdr));
      sendErrorAckToCommandChannel(source, bgcios::WrongService, 0);
      return 0;
   }

   // Handle the message.
   LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message from '" << source << "' is available on command channel");
   switch (msghdr->type) {

      case bgcios::iosctl::Terminate:
         err = terminate(source);
         break;

      case bgcios::iosctl::Interrupt:
         err = interrupt(source);
         break;

      case bgcios::iosctl::ErrorAck:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": last message sent to '" << source << "' was not handled successfully, " << bgcios::returnCodeToString(msghdr->returnCode));
         break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         err = sendErrorAckToCommandChannel(source, bgcios::UnsupportedType, 0);
         break;
   }

   return 0;
}

int
HwStdioController::dataChannelHandler(InetSocketPtr authOnly)
{
   InetSocketPtr& dataChannel( authOnly ? authOnly : _dataChannel );
   if (!dataChannel){  
      return ENOLINK;
   }

   // Receive a message from the data channel.
   int err = recvFromDataChannel(_inboundMessage, dataChannel);

   // An error occurred receiving a message.
   if (err != 0) {
      LOG_ERROR_MSG("error receiving message from data channel: " << bgcios::errorString(err));
      return err;
   }

   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
   CIOSLOGMSG(DTA_RECV_MSG,msghdr);

   // Make sure the service field is correct.

   if (msghdr->service != bgcios::StdioService) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << msghdr->service << " is wrong");
      sendErrorAckToDataChannel(bgcios::WrongService, bgcios::StdioService);
      return 1;
   }

   // Make the sure protocol version is a match.
   if (msghdr->version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": " << toString(msghdr->type) << " message protocol version " << (int)msghdr->version <<
                    " does not match stdiod version " << (int)ProtocolVersion << " for message received from data channel (" <<
                    getVersionString("stdiod", (int)ProtocolVersion) << ")");
      sendErrorAckToDataChannel(bgcios::VersionMismatch, ProtocolVersion);
      return 1;
   }

   // Make sure the Authenticate message is arriving when expected.
   if (authOnly && msghdr->type != Authenticate) {
      LOG_CIOS_TRACE_MSG("Job " << msghdr->jobId << ": expected Authenticate message");
      return 1;
   } else if (!authOnly && msghdr->type == Authenticate) {
      LOG_CIOS_TRACE_MSG("Job " << msghdr->jobId << ": unexpected Authenticate message");
      dataChannel.reset();
      return 1;
   }

   // Handle the message.
   LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << toString(msghdr->type) << " message is available on data channel");
   switch (msghdr->type) {
      case Authenticate:
         err = authenticate(authOnly);
         break;

      case StartJob: 
         err = startJob();
         break;

      case ReadStdinAck:
         readStdinAck();
         break;

      case ChangeConfig:
         err = changeConfig();
         break;

      case Reconnect:
         err = reconnect();
         break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         err = sendErrorAckToDataChannel(bgcios::UnsupportedType, 0);
         break;
   }

   // Check for errors handling the message.
   if (err != 0) {
      LOG_CIOS_INFO_MSG("Job " << msghdr->jobId << ": error handling " << toString(msghdr->type) << " message: " << bgcios::errorString(err));
   }

   return err;
}

void
HwStdioController::eventChannelHandler(void)
{
   int err;

   // Wait for the event (it should be here now).
   err = _rdmaListener->waitForEvent();
   if (err != 0) {
      return;
   }

   // Handle the event.
   rdma_cm_event_type type = _rdmaListener->getEventType();
   switch (type) {

      case RDMA_CM_EVENT_CONNECT_REQUEST:
      {
         // Construct a RdmaCompletionQueue object for the new client.
         RdmaCompletionQueuePtr completionQ;
         try {
             completionQ = RdmaCompletionQueuePtr(new RdmaCompletionQueue(_rdmaListener->getEventContext(), RdmaCompletionQueue::MaxQueueSize, _completionChannel->getChannel()));
         }
         catch (bgcios::RdmaError& e) {
            LOG_ERROR_MSG("error creating completion queue: " << e.what());
            return;
         }

         // Construct a new RdmaClient object for the new client.
         RdmaClientPtr client;
         try {
             client = RdmaClientPtr(new RdmaClient(_rdmaListener->getEventId(), _protectionDomain, completionQ));
         }
         catch (bgcios::RdmaError& e) {
            LOG_ERROR_MSG("error creating rdma client: " << e.what());
            completionQ.reset();
            return;
         }

         // Add new client to map of active clients.
         _clients.add(client->getQpNum(), client);

         // Add completion queue to completion channel.
         _completionChannel->addCompletionQ(completionQ);

         // Post a receive to get the first message.
         client->postRecvMessage();

         // Accept the connection from the new client.
         err = client->accept();
         if (err != 0) {
            LOG_ERROR_MSG("error accepting client connection: " << bgcios::errorString(err));
            _clients.remove(client->getQpNum());
            _completionChannel->removeCompletionQ(completionQ);
            client->reject(); // Tell client the bad news
            client.reset();
            completionQ.reset();
            break;
         }

         LOG_CIOS_DEBUG_MSG(client->getTag() << "connection accepted from " << client->getRemoteAddressString() << " is using completion queue " <<
                       completionQ->getHandle());
         break;
      }

      case RDMA_CM_EVENT_ESTABLISHED:
      {
         // Find connection associated with this event.
         RdmaClientPtr client = _clients.get(_rdmaListener->getEventQpNum());
         LOG_CIOS_INFO_MSG(client->getTag() << "connection established with " << client->getRemoteAddressString());

         break;
      }

      case RDMA_CM_EVENT_DISCONNECTED:
      {
         // Find connection associated with this event.
         uint32_t qp = _rdmaListener->getEventQpNum();
         RdmaClientPtr client = _clients.get(qp);
         RdmaCompletionQueuePtr completionQ = client->getCompletionQ();

         // Complete disconnect initiated by peer.
         err = client->disconnect(false);
         if (err == 0) {
            LOG_CIOS_INFO_MSG(client->getTag() << "disconnected from " << client->getRemoteAddressString());
         }
         else {
            LOG_ERROR_MSG(client->getTag() << "error disconnecting from peer: " << bgcios::errorString(err));
         }

         // Acknowledge the event (must be done before removing the rdma cm id).
         _rdmaListener->ackEvent();

         // Remove connection from map of active connections.
         _clients.remove(qp);

         // Destroy connection object.
         LOG_CIOS_DEBUG_MSG("destroying RDMA connection to client " << client->getRemoteAddressString());
         client.reset();

         // Remove completion queue from the completion channel.
         _completionChannel->removeCompletionQ(completionQ);

         // Destroy the completion queue.
         LOG_CIOS_DEBUG_MSG("destroying completion queue " << completionQ->getHandle());
         completionQ.reset();

         //clear any job info for the connection--find Job ID(s) associated with client, and handle like CloseStdio message
         //_jobs.clear();

         break;
      }

      default:
      {
         LOG_ERROR_MSG("event " << rdma_event_str(type) << " is not supported");
         break;
      }
   }

   // Acknowledge the event.  Should this always be done?
   if (type != RDMA_CM_EVENT_DISCONNECTED) {
      _rdmaListener->ackEvent();
   }

   return;
}

void
HwStdioController::completionChannelHandler(void)
{
   try {
      // Get the notification event from the completion channel.
      RdmaCompletionQueuePtr completionQ = _completionChannel->getEvent();

      // Remove work completions from the completion queue until it is empty.
      while (completionQ->removeCompletions() != 0) {

         // Get the next work completion.
         struct ibv_wc *completion = completionQ->popCompletion();

         // Check the status in the completion queue entry.
         if (completion->status != IBV_WC_SUCCESS) {
            LOG_ERROR_MSG("failed work completion, status '" << ibv_wc_status_str(completion->status) << "' for operation " <<
                          completionQ->wc_opcode_str(completion->opcode) <<  " (" << completion->opcode << ")");
            continue;
         }

         // Check the opcode in the completion queue entry.
         switch (completion->opcode) {
            case IBV_WC_SEND:
            {
               LOG_CIOS_TRACE_MSG("send operation completed successfully for queue pair " << completion->qp_num);
               break;
            }

            case IBV_WC_RECV:
            {
               LOG_CIOS_TRACE_MSG("receive operation completed successfully for queue pair " << completion->qp_num << " (received " << completion->byte_len << " bytes)");

               // Find the connection that received a message.
               RdmaClientPtr client = _clients.get(completion->qp_num);

               // Handle the message.
               bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)client->getInboundMessagePtr();
               if (msghdr->type == WriteStdout)
               {}
               else if (msghdr->type == WriteStderr) 
               {}
               else {
                 CIOSLOGMSG_RECV_WC(BGV_RECV_MSG, msghdr,completion);
               }

               LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << "." << msghdr->rank << ": " << toString(msghdr->type) <<
                             " message is available on completion channel from queue pair " << completion->qp_num);
               

               switch (msghdr->type) {
                     case ReadStdin:
                        readStdin(client);
                        break;

                     case WriteStdout:
                     case WriteStderr:
                        // put all the stdio messages found onto a processing queue
                        if ( addValidStdioMsg(client) ) continue;
                        break;

                     case CloseStdio:
                        closeStdio(client);
                        break;

                     

                     case ErrorAck:
                        break; // Nothing to do here


                     default:
                        LOG_ERROR_MSG("unsupported message type " << msghdr->type << " received from client " << bgcios::printHeader(*msghdr));
                        ErrorAckMessage *outMsg = (ErrorAckMessage *)client->getOutboundMessagePtr();
                        memcpy(&(outMsg->header), msghdr, sizeof(MessageHeader));
                        outMsg->header.type = ErrorAck;
                        outMsg->header.returnCode = bgcios::UnsupportedType;
                        outMsg->header.errorCode = 0;
                        outMsg->header.length = sizeof(ErrorAckMessage);
                        client->setOutboundMessageLength(outMsg->header.length);
                        break;
              }
               

               // Post a receive to get next message.
               client->postRecvMessage();

               // Send reply message in outbound message buffer to client.
               if (client->isOutboundMessageReady()) {
                   bgcios::MessageHeader * msgout = (bgcios::MessageHeader *)client->getOutboundMessagePtr();
                   if ( (msgout->type != WriteStdout) && (msgout->type != WriteStdout) ){
                     CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());
                   }
                   try {//$NEW try-catch
                       client->postSendMessage();
                   }
                   catch (const RdmaError& e) {
                       msgout->errorCode = (uint32_t)e.errcode();
                       CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());
                       LOG_ERROR_MSG("Job " << msgout->jobId << "." << msgout->rank << ": error posting WriteStderrAck or WriteStdoutAck" <<
                       client->getQpNum() << ": " << bgcios::errorString(e.errcode()));
                       //invalidate connection to compute?  Drain all messages for job by marking job invalid?
                   }
               }

               break;
            }

            case IBV_WC_RDMA_READ:
            {
               LOG_CIOS_DEBUG_MSG("rdma read operation completed successfully for queue pair " << completion->qp_num);
               break;
            }

            default:
            {
               LOG_ERROR_MSG("unsupported operation " << completion->opcode << " in work completion");
               break;
            }
         }
      }
   }

   catch (const RdmaError& e) {
      LOG_ERROR_MSG("error removing work completions from completion queue: " << bgcios::errorString(e.errcode()));
   }

   return;
}

int
HwStdioController::authenticate(InetSocketPtr dataChannel)
{
   // Get pointer to inbound Authenticate message.
   AuthenticateMessage *inMsg = (AuthenticateMessage *)_inboundMessage;

   // Build AuthenticateAck message in outbound buffer.
   AuthenticateAckMessage *outMsg = (AuthenticateAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = AuthenticateAck;
   outMsg->header.length = sizeof(AuthenticateAckMessage);
   outMsg->header.returnCode = Success;

   // Get the shared key from the personality.
   BF_KEY bfkey;
   const int err = getEncryptionKey(&bfkey);
   if (err != 0) {
      LOG_ERROR_MSG("could not get encryption key:" << err);
      dataChannel.reset();
      _done = 1;
      return err;
   }

   // Decrypt the data from the message.
   unsigned char ivec[8] = { 0 };
   unsigned char decryptedData[PlainDataSize];
   BF_cbc_encrypt(inMsg->encryptedData, decryptedData, EncryptedDataSize, &bfkey, ivec, BF_DECRYPT);

   // Compare the data.
   if (memcmp(inMsg->plainData, decryptedData, sizeof(inMsg->plainData)) != 0) {
      LOG_ERROR_MSG("authentication failed");
      
      std::ostringstream os;
      for ( unsigned i = 0; i < PlainDataSize; ++i ) {
         os << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(decryptedData[i]);
      }
      LOG_ERROR_MSG( "decrypted: " << os.str() );

      os.str("");
      for ( unsigned i = 0; i < PlainDataSize; ++i ) {
         os << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(inMsg->encryptedData[i]);
      }
      LOG_ERROR_MSG( "encrypted: " << os.str() );

      dataChannel.reset();
      _done = 1;
      return EPERM;
   }

   // Send AuthenticateAck message.
   LOG_CIOS_DEBUG_MSG("AuthenticateAck message sent on data channel");
   return sendToDataChannel(&outMsg->header, dataChannel); 
}

int
HwStdioController::startJob(void)
{
   // Get pointer to inbound StartJob message.
   StartJobMessage *inMsg = (StartJobMessage *)_inboundMessage;

   // Build StartJobAck message in outbound buffer.
   StartJobAckMessage *outMsg = (StartJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = StartJobAck;
   outMsg->header.length = sizeof(StartJobAckMessage);
   outMsg->header.returnCode = Success;

   // Construct Job object to track the job and add it to the list.
   const JobPtr job(new Job(inMsg->header.jobId, inMsg->numRanksForIONode));
   _jobs.add(inMsg->header.jobId, job);

   job->closeStdioAccumulator.setLimit((int)inMsg->numRanksForIONode);
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": " << inMsg->numRanksForIONode << " ranks participating in job");

   // Send StartJobAck message.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": StartJobAck message sent on data channel");
   return sendToDataChannel(&outMsg->header); 
}

void
HwStdioController::readStdin(const RdmaClientPtr& client)
{
   // Get pointer to inbound ReadStdin message.
   ReadStdinMessage *inMsg = (ReadStdinMessage *)client->getInboundMessagePtr();

   // Validate the job id.
   const JobPtr& job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " not active when handling ReadStdin message from rank " << inMsg->header.rank);

      // Build ReadStdinAck message in outbound message region.
      ReadStdinAckMessage *outMsg = (ReadStdinAckMessage *)client->getOutboundMessagePtr();
      memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
      outMsg->header.type = ReadStdinAck;
      outMsg->header.length = sizeof(MessageHeader);
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      client->setOutboundMessageLength(outMsg->header.length);
      return;
   }
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": ReadStdin message is requesting " << inMsg->length << " bytes");

   if (job->isKilled())
   {
       LOG_CIOS_INFO_MSG("Read stdin attempted after a job has begun termination. Job " << inMsg->header.jobId << "Rank " << inMsg->header.rank);

       // Build ReadStdinAck message in outbound message region.
       ReadStdinAckMessage *outMsg = (ReadStdinAckMessage *)client->getOutboundMessagePtr();
       memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
       outMsg->header.type = ReadStdinAck;
       outMsg->header.length = sizeof(MessageHeader);
       outMsg->header.returnCode = bgcios::RequestFailed;
       outMsg->header.errorCode = EINTR;
       client->setOutboundMessageLength(outMsg->header.length);
       return;
   }

   // Forward message to the data channel.
   int err = sendToDataChannel(inMsg);
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": ReadStdin message sent on data channel");

   if (err != 0) {
      LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ":" << inMsg->header.rank <<
                   ": error sending ReadStdin message on data channel: " << bgcios::errorString(err));

      // Build ReadStdinAck message in outbound message region.
      ReadStdinAckMessage *outMsg = (ReadStdinAckMessage *)client->getOutboundMessagePtr();
      memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
      outMsg->header.type = ReadStdinAck;
      outMsg->header.length = sizeof(MessageHeader);
      outMsg->header.returnCode = bgcios::SendError;
      outMsg->header.errorCode = (uint32_t)err;
      client->setOutboundMessageLength(outMsg->header.length);
      return;
   }

   // Save the client connection that is reading from standard input.
   job->setStdinClient(client);

   return;
}

void
HwStdioController::readStdinAck(void)
{
   // Get pointer to inbound ReadStdinAck message.
   ReadStdinAckMessage *inMsg = (ReadStdinAckMessage *)_inboundMessage;

   // Validate the job id.
   const JobPtr& job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " not active when handling ReadStdinAck message for rank " << inMsg->header.rank);

      // Build ReadStdinAck message in outbound message region.
      ReadStdinAckMessage *outMsg = (ReadStdinAckMessage *)_outboundMessage;
      memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
      outMsg->header.type = ReadStdinAck;
      outMsg->header.length = sizeof(MessageHeader);
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      sendToDataChannel(outMsg);
      return;
   }

   // Forward message to the client that is reading from standard input.
   RdmaClientPtr client = job->getStdinClient();
   if (client != NULL) {
      void *outMsg = client->getOutboundMessagePtr();
      memcpy(outMsg, inMsg, inMsg->header.length);
      try {
         CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());
         client->postSendMessage(inMsg->header.length);
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": ReadStdinAck message is ready for queue pair " <<
                       client->getQpNum() << " (" << bgcios::dataLength(&(inMsg->header)) << " bytes)");
      }
      catch (const RdmaError& e) {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": error posting ReadStdinAck message for queue pair " <<
                       client->getQpNum() << ": " << bgcios::errorString(e.errcode()));
      }

      // Reset client connection now that read has been satisfied.
      RdmaClientPtr noClient;
      job->setStdinClient(noClient);
   }
   else {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": ReadStdinAck message was ignored, waiting compute node not found");
   }

   return;
}

int
HwStdioController::interrupt(const std::string source)
{
   // Get pointer to inbound Interrupt message.
   bgcios::iosctl::InterruptMessage *inMsg = (bgcios::iosctl::InterruptMessage *)_inboundMessage;

   // Is this message coming from the tool control daemon
   bool fromToolCtld = (inMsg->header.service == ToolctlService) ? true : false;

   // Validate the job id.
   const JobPtr& job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Note that this is not a big deal.  The job could still be running but all of the ranks being serviced
      // by this I/O node could have already ended.
      LOG_INFO_MSG("Job " << inMsg->header.jobId << " not active when handling Interrupt message source="<<source);

      return 0;
   }
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << " Interrupt source=" << source);

   // If there is a client that is reading from standard input, forward a ReadStdinAck message with an error.
   RdmaClientPtr client = job->getStdinClient();
   if (client != NULL) {
      // Build a ReadStdinAck message in the client's outbound message region.
      ReadStdinAckMessage *ackMsg = (ReadStdinAckMessage *)client->getOutboundMessagePtr();
      ackMsg->header.service = bgcios::StdioService;
      ackMsg->header.version = ProtocolVersion;
      ackMsg->header.type = ReadStdinAck;
      ackMsg->header.rank = 0; // We could save the rank but it really isn't needed.
      ackMsg->header.sequenceId = inMsg->header.sequenceId;
      ackMsg->header.returnCode = bgcios::RequestFailed;
      ackMsg->header.errorCode = EINTR;
      ackMsg->header.length = sizeof(bgcios::MessageHeader);
      ackMsg->header.jobId = inMsg->header.jobId;

      try {
         CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());
         client->postSendMessage(ackMsg->header.length);
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": ReadStdinAck message is ready for queue pair " <<
                       client->getQpNum() << " to interrupt in progress operation");
      }
      catch (const RdmaError& e) {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": error posting ReadStdinAck message for queue pair " <<
                       client->getQpNum() << ": " << bgcios::errorString(e.errcode()));
      }      

      // Reset client connection now that read has been satisfied.
      RdmaClientPtr noClient;
      job->setStdinClient(noClient);
   }

   // Mark the job as killed to stop subsequent I/O operations.
   if (!fromToolCtld && (inMsg->signo == SIGKILL))
   {
       job->markKilled();
       LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": marked job as killed");
   }

   return 0;;
}

uint64_t
HwStdioController::writeStdio(const RdmaClientPtr& client)
{
   // Get pointer to inbound WriteStdio message.
   WriteStdioMessage *inMsg = (WriteStdioMessage *)client->getInboundMessagePtr();
   WriteStdioAckMessage *outMsg = (WriteStdioAckMessage *)client->getOutboundMessagePtr();
   uint64_t ret_errorCode = 0;

   // Validate the job id.
   const JobPtr& job = _jobs.get(inMsg->header.jobId);
   
   if ( (job != NULL) && (!job->isKilled() ) ){

     //if (job->logJobStatistics()) job->writeTimer.start();

     // use inMsg to track results of send.  Errno tracked in inMsg header.errorCode.  Bytes sent in inMsg header.returnCode 
     unsigned int length = inMsg->header.length - outMsg->header.returnCode;
     char * buff_start = (char *)inMsg;
     buff_start += outMsg->header.returnCode;

     const SocketPtr& socket = std::tr1::static_pointer_cast<Socket>(_dataChannel);
     if ( !socket ) {
        // when data channel is not connected (could be a failover event) drop
        // stdout and stderr instead of keeping it in a queue
        //outMsg->header.returnCode = bgcios::SendError;
        //outMsg->header.errorCode = ENOTCONN; 
        //job->dropStdioMessage(bgcios::dataLength(&(inMsg->header))); 
        //Keep message, send when data channel comes back
        return ENOTCONN;       
     }
     else {
         outMsg->header.returnCode += socket->sendOnConnectedSocket( (char *)inMsg,length, outMsg->header.errorCode);
         ret_errorCode = outMsg->header.errorCode;

         //if (job->logJobStatistics())job->writeTimer.stop();

         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": " << toString(inMsg->header.type) <<
                 " message sent on data channel (" << bgcios::dataLength(&(inMsg->header)) << " bytes)");
         if (outMsg->header.errorCode) {
             //LOG_CIOS_INFO_MSG("outMsg->header.errorCode="<<outMsg->header.errorCode<<" outMsg->header.jobId="<<outMsg->header.jobId);
             //outMsg->header.returnCode = bgcios::SendError;
             //job->dropStdioMessage(bgcios::dataLength(&(inMsg->header))); 
             //Keep message, send when data channel comes back
             return ret_errorCode;       
         }
         else {
             outMsg->header.returnCode = bgcios::Success;
         }
     }
  }
  else {
    if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " not active when handling WriteStdio message from rank " << inMsg->header.rank);
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
    }

    // Drop output if job has been killed.
    else if (job->isKilled()) {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = EINTR;
      job->dropStdioMessage(bgcios::dataLength(&(inMsg->header))); 
    }
  }      
   // Post a receive to get next message.
   client->postRecvMessage();

   // Send reply message in outbound message buffer to client.
   if (client->isOutboundMessageReady()) {
      if ( (outMsg->header.type != WriteStdoutAck) && (outMsg->header.type != WriteStderrAck) ){
        CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());
      }
      try { //$NEW try-catch block
           client->postSendMessage();
      }
      catch (const RdmaError& e) {
           CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());
           LOG_ERROR_MSG("Job " << inMsg->header.jobId << "." << inMsg->header.rank << ": error posting WriteStderrAck or WriteStdoutAck" <<
                       client->getQpNum() << ": " << bgcios::errorString(e.errcode()));
          //invalidate connection to compute?  Drain all messages for job by marking job invalid?
      }
      _dequeStdioMsgInClient.pop_front();  //remove from deque
   }

   return ret_errorCode;
}

bool 
HwStdioController::addValidStdioMsg(const RdmaClientPtr & client){
    // Get pointer to inbound WriteStdio message.
   WriteStdioMessage *inMsg = (WriteStdioMessage *)client->getInboundMessagePtr();
   // Validate the job id.
   const JobPtr& job = _jobs.get(inMsg->header.jobId);

   // Build WriteStdioAck message in outbound message region.
   WriteStdioAckMessage *outMsg = (WriteStdioAckMessage *)client->getOutboundMessagePtr();
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = inMsg->header.type == WriteStdout ? WriteStdoutAck : WriteStderrAck;
   outMsg->header.length = sizeof(WriteStdioAckMessage); 
   client->setOutboundMessageLength(outMsg->header.length);  

   
   if ( (job != NULL) && (!job->isKilled() ) ){
      _dequeStdioMsgInClient.push_back(client);
      return true;    
   }
   else{
    if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " not active when handling WriteStdio message from rank " << inMsg->header.rank);
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
    }
    // Drop output if job has been killed.
    else if (job->isKilled()) {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = EINTR;     
    }
   }
   return false;
}

void
HwStdioController::closeStdio(const RdmaClientPtr& client)
{
   // Get pointer to inbound CloseStdio message.
   CloseStdioMessage *inMsg = (CloseStdioMessage *)client->getInboundMessagePtr();

   // Build CloseStdioAck message in outbound message region.
   CloseStdioAckMessage *outMsg = (CloseStdioAckMessage *)client->getOutboundMessagePtr();
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = CloseStdioAck;
   outMsg->header.length = sizeof(CloseStdioAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Validate the job id.
   const JobPtr& job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " not active when handling CloseStdio message from rank " << inMsg->header.rank);
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      client->setOutboundMessageLength(outMsg->header.length);
      return;
   }

   // Accumulate messages and forward to the data channel when all messages have been received.
   int err = 0;
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": " << inMsg->header.rank << " ranks closed stdio");
   const bool ready = job->closeStdioAccumulator.add(inMsg);
   if (ready) {
      err = sendToDataChannel((void *)job->closeStdioAccumulator.get());
      LOG_CIOS_DEBUG_MSG(
              "Job " << inMsg->header.jobId << ": CloseStdio message sent on data channel (" <<
              job->closeStdioAccumulator.getLimit() << " compute nodes)"
              );

      // Remove the job from the map and destroy the Job object.
      if (err == 0) {
         if (job->getNumDroppedStdioMsgs() > 0) {
            LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": " << job->getNumDroppedStdioMsgs() << " messages with " << job->getNumDroppedStdioBytes() <<
                         " bytes of data were dropped when job was terminated/killed by signal or data channel was disconected");
         }
         job->closeStdioAccumulator.resetCount();
         if (job->writeTimer.getNumOperations() > 0) {
              LOG_INFO_MSG("Job " << inMsg->header.jobId << ": stats for write: " << job->writeTimer);
         }
         _jobs.remove(job->getJobId());
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": removed job from list");
      }

      // Keep the Job object so CloseStdio message can be resent when the data channel is reconnected.
      else {
         LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": error sending CloseStdio mesage on data channel: " << bgcios::errorString(err));
      }
   }

   if (err != 0) {
      outMsg->header.returnCode = bgcios::SendError;
      outMsg->header.errorCode = (uint32_t)err;
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": CloseStdioAck message is ready for queue pair " << client->getQpNum()); 
   client->setOutboundMessageLength(outMsg->header.length);
   return;
}

int
HwStdioController::changeConfig(void)
{
   // Get pointer to inbound ChangeConfig message.
   ChangeConfigMessage *inMsg = (ChangeConfigMessage *)_inboundMessage;

   // Build ChangeConfigAck message in outbound buffer.
   ChangeConfigAckMessage *outMsg = (ChangeConfigAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ChangeConfigAck;
   outMsg->header.length = sizeof(ChangeConfigAckMessage);
   outMsg->header.returnCode = Success;

   // Update logging levels for the specified trace types.
   setLoggingLevel("ibm.cios.common", inMsg->commonTraceLevel);
   if (inMsg->commonTraceLevel != 0) {
      LOG_INFO_MSG("Changed ibm.cios.common log level to '" << inMsg->commonTraceLevel << "'"); 
   }
   setLoggingLevel("ibm.cios.stdiod", inMsg->stdiodTraceLevel);
   if (inMsg->stdiodTraceLevel != 0) {
      LOG_INFO_MSG("Changed ibm.cios.stdiod log level to '" << inMsg->stdiodTraceLevel << "'");  
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": ChangeConfigAck message sent on data channel");
   return sendToDataChannel(outMsg);
}

int
HwStdioController::reconnect(void)
{
   // Get pointer to inbound Reconnect message.
   ReconnectMessage *inMsg = (ReconnectMessage *)_inboundMessage;

   // Build ReconnectAck message in outbound buffer.
   ReconnectAckMessage *outMsg = (ReconnectAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ReconnectAck;
   outMsg->header.length = sizeof(ReconnectAckMessage);
   outMsg->header.returnCode = Success;

   // remember jobs to remove after iterating through our container
   std::queue<uint64_t> jobsToRemove;

   // Run the list of jobs and resend any CloseStdio messages.
   for (job_list_iterator iter = _jobs.begin(); iter != _jobs.end(); ++iter) {
      JobPtr job = iter->second;

      // Forward the CloseStdio message to the data channel if it has been received from all of the compute nodes.
      if (job->closeStdioAccumulator.atLimit()) {
         int err = sendToDataChannel((void *)job->closeStdioAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": CloseStdio message sent on data channel when handling Reconnect message (" <<
                       job->closeStdioAccumulator.getLimit() << " compute nodes)");

         // Remove the job from the map and destroy the Job object.
         if (err == 0) {
            if (job->getNumDroppedStdioMsgs() > 0) {
               LOG_CIOS_WARN_MSG("Job " << inMsg->header.jobId << ": " << job->getNumDroppedStdioMsgs() << " messages with " << job->getNumDroppedStdioMsgs() <<
                            " bytes of data were dropped while data channel was disconnected");
            }
            job->closeStdioAccumulator.resetCount();
            jobsToRemove.push( job->getJobId() );
         }

         // Keep the Job object so CloseStdio message can be resent when the data channel is reconnected.
         else {
            LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": error sending CloseStdio mesage on data channel when handling Reconnect message: " << bgcios::errorString(err));
         }
      }
   }

   while ( !jobsToRemove.empty() ) {
       LOG_INFO_MSG("Job " << jobsToRemove.front() << ": removed job from list when handling Reconnect message");
       _jobs.remove( jobsToRemove.front() );
       jobsToRemove.pop();
   }

   // Send ReconnectAck message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": ReconnectAck message sent on data channel");
   return sendToDataChannel(outMsg);
}

int
HwStdioController::terminate(const std::string source)
{
   // Get pointer to Terminate message available from inbound buffer.
   bgcios::iosctl::TerminateMessage *inMsg = (bgcios::iosctl::TerminateMessage *)_inboundMessage;

   // Cleanup resources.
   int err = cleanup();

   // Set flags to stop processing messages.
   _done = true;
   _terminated = true;

   // Build TerminateAck message in outbound buffer.
   bgcios::iosctl::TerminateAckMessage *outMsg = (bgcios::iosctl::TerminateAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = bgcios::iosctl::TerminateAck;
   outMsg->header.length = sizeof(bgcios::iosctl::TerminateAckMessage);
   if (err == 0) {
      outMsg->header.returnCode = bgcios::Success;
   }
   else {
      outMsg->header.returnCode = bgcios::RequestFailed;
      outMsg->header.errorCode = (uint32_t)err;
   }

   // Send TerminateAck message to source daemon.
   return sendToCommandChannel(source, outMsg);
}

