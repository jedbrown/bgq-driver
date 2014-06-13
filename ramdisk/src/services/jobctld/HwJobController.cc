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

//! \file  HwJobController.cc
//! \brief Methods for bgcios::jobctl:HwJobController class.

// Includes
#include "HwJobController.h"
#include "ToolProcess.h"
#include <ramdisk/include/services/common/RdmaError.h>
#include <ramdisk/include/services/common/RdmaDevice.h>
#include <ramdisk/include/services/common/RdmaCompletionQueue.h>
#include <ramdisk/include/services/common/MessageResult.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/IosctlMessages.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <iomanip>
#include <queue>
#include <ramdisk/include/services/common/Cioslog.h>
#include <ramdisk/include/services/common/SignalHandler.h>



using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");

//! Interval between checks for ended tools.
const int ToolInterval = 3000;

HwJobController::HwJobController(JobctlConfigPtr config) : JobController(config)
{
   // Initialize private data.
   _nextServiceId = 0;
   _activeTools = 0;

   // Set work directory.
   _workDirectory = bgcios::WorkDirectory; // or "/tmp/cios25/" for siw testing

   // Build the path to the iosd command channel.
   std::ostringstream iosdPath;
   iosdPath << _workDirectory << bgcios::IosctlCommandChannelName;
   _iosdCmdChannelPath = iosdPath.str();
}

HwJobController::~HwJobController()
{
   //! \todo Should call cleanup() method here?
   free(_inboundMessage);
   free(_outboundMessage);
}

int
HwJobController::startup(in_port_t dataChannelPort)
{
   // Build the path to the command channel.
   std::ostringstream cmdChannelPath;
   cmdChannelPath << _workDirectory << bgcios::JobctlCommandChannelName;

   // Create the command channel socket.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(cmdChannelPath.str()));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating command channel '" << cmdChannelPath.str() << "': " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("[" << _cmdChannel->getSd() << "] created command channel at '" << _cmdChannel->getName() << "'");

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
      LOG_ERROR_MSG("error listening for new connections: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("listening for new data channel connections on fd " << _dataListener->getSd() << " using address " << _dataListener->getName());

   // Find the address of the I/O link device.
   RdmaDevicePtr linkDevice;
   try {
      linkDevice = RdmaDevicePtr(new RdmaDevice(bgcios::RdmaDeviceName, bgcios::RdmaInterfaceName)); // "mlx4_0", "ib0"
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error opening InfiniBand device: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created InfiniBand device for " << linkDevice->getDeviceName() << " using interface " << linkDevice->getInterfaceName());

   // Create a server bound to the I/O link device so protection domain and completion queue objects can be created before connections
   // start arriving on the listener.  This object is never used for connections.
   try {
      _boundServer = bgcios::RdmaServerPtr(new bgcios::RdmaServer(linkDevice->getAddress(), BaseRdmaPort-1));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error creating bound RDMA server: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created bound RDMA server on port " << BaseRdmaPort-1);

   // Create a protection domain object.
   try {
      _protectionDomain = RdmaProtectionDomainPtr(new RdmaProtectionDomain(_boundServer->getContext()));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error allocating protection domain: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created protection domain " << _protectionDomain->getHandle());

   // Create a completion channel object.
   try {
      _completionChannel = RdmaCompletionChannelPtr(new RdmaCompletionChannel(_boundServer->getContext(), false)); 
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error constructing completion channel: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created completion channel using fd " << _completionChannel->getChannelFd());

   // Create listener for RDMA connections.
   try {
      _rdmaListener = bgcios::RdmaServerPtr(new bgcios::RdmaServer(INADDR_ANY, BaseRdmaPort));
   }
   catch (bgcios::RdmaError& e) {
      LOG_ERROR_MSG("error creating listening RDMA connection: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("created listening RDMA connection on port " << BaseRdmaPort);

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
HwJobController::cleanup(void)
{
   _dataListener.reset();
   _dataChannel.reset();

   return 0;
}

void
HwJobController::eventMonitor(void)
{
   const int cmdChannel   = 0;
   const int dataChannel  = 1;
   const int compChannel  = 2;
   const int eventChannel = 3;
   const int dataListener = 4;
   const int pipeForSig   = 5;
   const int numFds       = 6;

   pollfd pollInfo[numFds];
   int timeout = -1;

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
   pollfdCompletionChannel = &pollInfo[compChannel];

   pollInfo[eventChannel].fd = _rdmaListener->getEventChannelFd();
   pollInfo[eventChannel].events = POLLIN;
   pollInfo[eventChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added event channel using fd " << pollInfo[eventChannel].fd << " to descriptor list");

   pollInfo[dataListener].fd = _dataListener->getSd();
   pollInfo[dataListener].events = POLLIN;
   pollInfo[dataListener].revents = 0;
   LOG_CIOS_TRACE_MSG("added data channel listener using fd " << pollInfo[dataListener].fd << " to descriptor list");

    
   bgcios::SigWritePipe SigWritePipe(SIGUSR1);

   pollInfo[pipeForSig].fd = SigWritePipe._pipe_descriptor[0];
   pollInfo[pipeForSig].events = POLLIN;
   pollInfo[pipeForSig].revents = 0;
   LOG_CIOS_TRACE_MSG("added signal pipe listener using fd " << pollInfo[pipeForSig].fd << " to descriptor list");


   // Process events until told to stop.
   while (!_done) {

      // Wait for an event on one of the descriptors.
      timeout = _activeTools ? ToolInterval : -1;
      int rc = poll(pollInfo, numFds, timeout);

      // There was an error so log the failure and try again.
      if (rc == -1) {
         int err = errno;
         if (err == EINTR) {
            LOG_CIOS_TRACE_MSG("poll returned EINTR, continuing ...");
            continue;
         }

         LOG_ERROR_MSG("poll failed, error " << err);
         return;
      } 

      // Check on the status of tool processes if there are any active.
      if (_activeTools) {
         LOG_CIOS_TRACE_MSG("there are " << _activeTools << " active tools on this node");
         // Check if any SIGCHLD signals have been delivered for tool processes that ended.
         struct timespec timeout = { 0, 0 };
         sigset_t waitSet;
         sigemptyset(&waitSet);
         sigaddset(&waitSet, SIGCHLD);
         int signo = sigtimedwait(&waitSet, NULL, &timeout);

         // If so, find the tool process that ended and report the status to runjob.
         if (signo == SIGCHLD) {
            LOG_CIOS_TRACE_MSG("at least one child process has ended");
            for (job_list_iterator iter = _jobs.begin(); iter != _jobs.end(); ++iter) {
               JobPtr job = iter->second;
               uint32_t toolId = job->findEndedTool();
               if (toolId != 0) {
                  int err = exitTool(job, toolId);
                  if (err != 0) {
                     LOG_ERROR_MSG("Job " << job->getJobId() << ": error sending ExitTool message on data channel: " << bgcios::errorString(err));
                  }
               }
            }
         }
      }

      // There was no data so try again.
      if (rc == 0) {
         continue;
      } 

      // Check for an event on the command channel.
      if (pollInfo[cmdChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on command channel");
         pollInfo[cmdChannel].revents = 0;
         commandChannelHandler();
      }

      // Check for an event on the data channel.
      if (pollInfo[dataChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on data channel");
         pollInfo[dataChannel].revents = 0;
         if (dataChannelHandler() == EPIPE) {
             pollInfo[dataChannel].fd = -1;
         }
      }

      // Check for an event on the completion channel.
      if (pollInfo[compChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on completion channel");
         pollInfo[compChannel].revents = 0;
         completionChannelHandler();
      }

      // Check for an event on the event channel.
      if (pollInfo[eventChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on event channel");
         pollInfo[eventChannel].revents = 0;
         eventChannelHandler();
      }

      // Check for an event on the pipe for signal.
      if (pollInfo[pipeForSig].revents & POLLIN) {
         LOG_INFO_MSG_FORCED("input event available pipe from signal handler");
         pollInfo[pipeForSig].revents = 0;
         siginfo_t siginfo;
         read(pollInfo[pipeForSig].fd,&siginfo,sizeof(siginfo_t));
         const size_t BUFSIZE = 1024;
         char buffer[BUFSIZE];
         const size_t HOSTSIZE = 256;
         char hostname[HOSTSIZE];
         hostname[0]=0;
         gethostname(hostname,HOSTSIZE);
         snprintf(buffer,BUFSIZE,"/var/spool/abrt/fl_jobctld.%d.%s.log",getpid(),hostname);
         LOG_INFO_MSG_FORCED("Attempting to write flight log "<<buffer);
         printLogMsg(buffer); //print the log to stdout
      }

      // Check for an event on the data channel listener.
      if (pollInfo[dataListener].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on data channel listener");
         pollInfo[dataListener].revents = 0;

         // Make a new data channel connected to runjob.
         const InetSocketPtr incoming = makeDataChannel();
         if (!incoming) {
            LOG_ERROR_MSG("error making new data channel");
            continue;
         }

         LOG_INFO_MSG_FORCED("data channel is connected to " << incoming->getPeerName() << " using fd " << incoming->getSd());

         // Handle the Authenticate message which must be sent first.
         if (!dataChannelHandler(incoming)) {
             _dataChannel = incoming;
             pollInfo[dataChannel].fd = _dataChannel->getSd();
             LOG_INFO_MSG_FORCED("data channel is authenticated with " << _dataChannel->getPeerName() << " using fd " << _dataChannel->getSd());;
         }
      }
   }

   // Reset for next time.
   _done = 0;

   return;
}

int
HwJobController::dataChannelHandler(InetSocketPtr authOnly)
{
   InetSocketPtr& dataChannel( authOnly ? authOnly : _dataChannel );

   // Receive a message from the data channel.
   int err = recvFromDataChannel(_inboundMessage, dataChannel);
   
   // When data channel closes, stop handling events.
   if (err == EPIPE) {
      LOG_ERROR_MSG("data channel connected to " << dataChannel->getPeerName() << " is closed");
      dataChannel.reset();
      return err;
   }

   // An error occurred receiving a message.
   if (err != 0) {
      LOG_ERROR_MSG("error receiving message from data channel: " << bgcios::errorString(err));
      return err;
   }

   // Make sure the service field is correct.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
   if (msghdr->service != bgcios::JobctlService) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << (int)msghdr->service << " is wrong, header: " << bgcios::printHeader(*msghdr));
      sendErrorAckToDataChannel(bgcios::WrongService, bgcios::JobctlService);
      return 1;
   }

   if ( msghdr->type != Heartbeat ) {
       CIOSLOGMSG(DTA_RECV_MSG,msghdr);
   }

   // Make the sure protocol version is a match.
   if (msghdr->version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": " << toString(msghdr->type) << " message protocol version " << (int)msghdr->version <<
                    " does not match jobctld version " << (int)ProtocolVersion << " for message received from data channel (" <<
                    getVersionString("jobctld", (int)ProtocolVersion) << ")");
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
      case Heartbeat:
          err = heartbeat();
          break;

      case Authenticate:
         err = authenticate(authOnly);
         break;

      case SetupJob:
         err = setupJob();
         break;

      case LoadJob: 
         err = loadJob();
         break;

      case StartJob:
         err = startJob();
         break;

      case StartTool:
         err = startTool();
         break;

      case EndTool:
         err = endTool();
         break;

      case CheckToolStatus:
         err = checkToolStatus();
         break;

      case ExitToolAck:
         exitToolAck();
         break;

      case SignalJob:
         err = signalJob();
         break;

      case CleanupJob:
         err = cleanupJob();
         break;

      case ChangeConfig:
         err = changeConfig();
         break;

      case Reconnect:
         err = reconnect();
         break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported");
         sendErrorAckToDataChannel(bgcios::UnsupportedType, 0);
         break;
   }

   if (err != 0) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": error handling " << toString(msghdr->type) << " message: " << bgcios::errorString(err));
   }
   return err;
}

int
HwJobController::commandChannelHandler(void)
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

   // Handle the message.
   LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message from '" << source << "' is available on command channel");
   switch (msghdr->type) {

      case bgcios::iosctl::Ready:
         err = discoverNodeStep2();
         break;

      case bgcios::iosctl::Terminate:
         err = terminate();
         break;

      case LoadJobAck:
         err = loadJobAck();
         break;

      case CleanupJobAck:
         err = cleanupJobAck();
         break;

      case bgcios::iosctl::ErrorAck:
         LOG_CIOS_WARN_MSG("Job " << msghdr->jobId << ": ErrorAck message received from '" << source << "': " << bgcios::returnCodeToString(msghdr->returnCode));
         break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         err = sendErrorAckToCommandChannel(source, bgcios::UnsupportedType, 0);
         break;
   }

   if (err != 0) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": sending " << bgcios::toString(msghdr) << " ack: " << bgcios::errorString(err));
   }

   return 0;
}

void
HwJobController::eventChannelHandler(void)
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
             client->createRegionAuxOutbound(_protectionDomain);
         }
         catch (bgcios::RdmaError& e) {
            LOG_ERROR_MSG("error creating rdma client: " << e.what());
            completionQ.reset();
            return;
         }

         // A compute node always has an IP address on the 10.x.x.x network.  Only accept connections from a client that behaves like CNK.
         if ((client->getRemoteIPv4Address() & 0xff000000) != 0x0a000000) {
            LOG_ERROR_MSG("connection from " << client->getRemoteAddressString() << " rejected because it is not a compute node");
            client->reject();
            client.reset();
            completionQ.reset();
            break;
         }

         // Add new client to map of active connections.
         _clients.add(client->getQpNum(), client);

         // Add completion queue to completion channel.
         _completionChannel->addCompletionQ(completionQ);

         // Post receives
         if (client->postRecvMsgMult(4) ){ //post 4 receives using the blocking size against the inbound Message Region;
           LOG_ERROR_MSG("error posting receives using postRecvMsgMult");
         }

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
         const uint32_t qp = _rdmaListener->getEventQpNum();
         RdmaClientPtr client = _clients.get(qp);
         if ( !client ) {
             LOG_ERROR_MSG( "could not find client for qp " << qp );
             break;
         }

         ComputeNodePtr cnode = _cnodes.get(client->getUniqueId());
         if ( !cnode ) {
             LOG_ERROR_MSG( "could not find compute node client " << client->getTag() );
             break;
         }

         RdmaCompletionQueuePtr completionQ = client->getCompletionQ();

         // Complete disconnect initiated by peer.
         err = client->disconnect(false);
         if (err == 0) {
            LOG_CIOS_INFO_MSG(client->getTag() << "disconnected from " << client->getRemoteAddressString() << " (" << *cnode << ")");
         }
         else {
            LOG_ERROR_MSG(client->getTag() << "error disconnecting from " << client->getRemoteAddressString() << ": " << bgcios::errorString(err));
         }

         // Acknowledge the event (must be done before removing the rdma cm id).
         _rdmaListener->ackEvent();

         // Cleanup compute node if it connected successfully.
         if (cnode != NULL) {
            // Remove compute node from map of connected compute nodes.
            _cnodes.remove(cnode->getServiceId());

            // Destroy compute node object.
            cnode.reset();
         }

         // Remove client from map of active connections.
         _clients.remove(qp);

         // Destroy connection object.
         LOG_CIOS_DEBUG_MSG("destroying RDMA connection to client " << client->getRemoteAddressString());
         client.reset();

         // Remove completion queue from the completion channel.
         _completionChannel->removeCompletionQ(completionQ);

         // Destroy the completion queue.
         LOG_CIOS_DEBUG_MSG("destroying completion queue " << completionQ->getHandle());
         completionQ.reset();

         break;
      }

      case RDMA_CM_EVENT_TIMEWAIT_EXIT:
      {
         LOG_CIOS_DEBUG_MSG("timewait state completed for queue pair " << _rdmaListener->getEventQpNum());
         break;
      }

      default:
      {
         LOG_ERROR_MSG("event " << rdma_event_str(type) << " is not supported");
         break;
      }
   }

   // Acknowledge the event.
   if (type != RDMA_CM_EVENT_DISCONNECTED) {
      _rdmaListener->ackEvent();
   }

   return;
}

void
HwJobController::completionChannelHandler(void)
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
            RdmaClientPtr client = _clients.get(completion->qp_num);
            if (client != NULL) {
               LOG_ERROR_MSG(client->getTag() << "failed work completion, status '" << ibv_wc_status_str(completion->status) << "' for operation " <<
                             completionQ->wc_opcode_str(completion->opcode) <<  " (" << completion->opcode << ") using source queue pair " << completion->src_qp);
            }
            else {
               LOG_ERROR_MSG("failed work completion, status '" << ibv_wc_status_str(completion->status) << "' for operation " <<
                             completionQ->wc_opcode_str(completion->opcode) <<  " (" << completion->opcode << ") using source queue pair " << completion->src_qp);
            }
            continue;
         }

         // Check the opcode in the completion queue entry.
         switch (completion->opcode) {
            case IBV_WC_SEND:
            {
               LOG_CIOS_TRACE_MSG("send operation completed successfully for queue pair " << completion->qp_num);
               //printf("IBV_WC_SEND completion->qp_num=%llu\n",(long long unsigned int)completion->qp_num);
               RdmaClientPtr client = _clients.get(completion->qp_num);
               client->setOutboundMessageLength(0);
               break;
            }

            case IBV_WC_RECV:
            {
               LOG_CIOS_TRACE_MSG("receive operation completed successfully for queue pair " << completion->qp_num << " (received " << completion->byte_len << " bytes)");

               // Find the connection that received a message.
               const RdmaClientPtr client = _clients.get(completion->qp_num);
               client->setOutboundMessageLength(0);  
               // Handle the message.
               bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)completion->wr_id;
               
               //CIOSLOGMSG(BGV_RECV_MSG, msghdr);
               CIOSLOGMSG_RECV_WC(BGV_RECV_MSG, msghdr,completion);

               // check message version in discoverNodeStep1 as the first and only check since it is the first received
               switch (msghdr->type) {
                     case DiscoverNode: discoverNodeStep1(client, msghdr); break;
                     case SetupJobAck: setupJobAck(msghdr); break;
                     case LoadJobAck: loadJobAck(msghdr); break;
                     case StartJobAck: startJobAck(msghdr); break;
                     case ExitProcess: exitProcess(client,msghdr); break;
                     case ExitJob: exitJob(client,msghdr); break;
                     case CleanupJobAck: cleanupJobAck(msghdr); break;
                     case SignalJobAck: signalJobAck(msghdr); break;

                     case ErrorAck: break; // Nothing to do here

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

               // re-post receive buffer
               client->postRecvMsg((uint64_t)completion->wr_id);
               

               // Send reply message in outbound message buffer to client.
               if (client->isOutboundMessageReady()) {
                  CIOSLOGMSG_QP(BGV_SEND_MSG, client->getOutboundMessagePtr(),client->getQpNum());                
                  client->postSendMessage();//signal post send completed
               }

//             LOG_CIOS_WARN_MSG("Job " << msghdr->jobId << ": " << client->opCountersToString() << " " << *completionQ);
               break;
            }

            case IBV_WC_RDMA_READ:
            {
               LOG_CIOS_TRACE_MSG("rdma read operation completed successfully for queue pair " << completion->qp_num);
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
      LOG_ERROR_MSG("error handling work completions from completion queue: " << bgcios::errorString(e.errcode()));
   }

   return;
}


int
HwJobController::authenticate(InetSocketPtr channel)
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
      LOG_ERROR_MSG("could not get encryption key: " << err);
      channel.reset();
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

      channel.reset();
      _done = 1;
      return EPERM;
   }

   // Send AuthenticateAck message.
   LOG_CIOS_DEBUG_MSG("AuthenticateAck message sent on data channel");
   return sendToDataChannel(outMsg, channel); 
}

void
HwJobController::discoverNodeStep1(const RdmaClientPtr& client, bgcios::MessageHeader * mh)
{
   // Get pointer to inbound DiscoverNode message.
   DiscoverNodeMessage *inMsg = (DiscoverNodeMessage *)mh;

   // Make the sure protocol version is a match.  Check here as the first and only time
   if (inMsg->header.version != ProtocolVersion) {
      
      LOG_ERROR_MSG("Expected version="<<ProtocolVersion<<" but received DiscoverNodeMessage version="<<inMsg->header.version);
      ErrorAckMessage *outMsg = (ErrorAckMessage *)client->getOutboundMessagePtr();
      memcpy(&(outMsg->header), &inMsg->header, sizeof(MessageHeader));
      outMsg->header.type = ErrorAck;
      outMsg->header.returnCode = bgcios::VersionMismatch;
      outMsg->header.errorCode = ProtocolVersion;
      outMsg->header.length = sizeof(ErrorAckMessage);
      client->setOutboundMessageLength(outMsg->header.length);
   }

   // search for compute nodes with the same block id and UCI that may have
   // persisted from a previous compute block boot that did not shut down properly
   for ( cnode_list_iterator i = _cnodes.begin(); i != _cnodes.end(); ++i ) {
       const ComputeNodePtr& c = i->second;
       if ( c->getUci() != inMsg->uci ) continue;
       if ( c->getBlockId() != inMsg->blockId ) continue;
       LOG_WARN_MSG( "removing compute node " << *c );
       _cnodes.remove( i->first );
       break;
   }
   
   // Create a ComputeNode object to keep track of the node and add it to the map of connected compute nodes.
   const uint32_t serviceId = _nextServiceId++;
   const ComputeNodePtr cnode(new ComputeNode(serviceId, client, inMsg->blockId, inMsg->uci, inMsg->coords, inMsg->bridge));
   _cnodes.add(serviceId, cnode);
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": added compute node (" << *cnode << ") to connected list"); 

   // Build StartNodeServices message in outbound buffer.
   bgcios::iosctl::StartNodeServicesMessage *reqMsg = (bgcios::iosctl::StartNodeServicesMessage *)_outboundMessage;
   memset(reqMsg, 0, sizeof(bgcios::iosctl::StartNodeServicesMessage));
   reqMsg->header.service = bgcios::IosctlService;
   reqMsg->header.version = bgcios::iosctl::ProtocolVersion;
   reqMsg->header.type = bgcios::iosctl::StartNodeServices;
   reqMsg->header.length = sizeof(bgcios::iosctl::StartNodeServicesMessage);
   reqMsg->serviceId = serviceId;
   client->setUniqueId(serviceId);
   reqMsg->CNtorus = coordsToNodeId(inMsg->coords.aCoord,inMsg->coords.bCoord,inMsg->coords.cCoord,inMsg->coords.dCoord,inMsg->coords.eCoord);

   // Send StartNodeServices message to iosd.
   sendToCommandChannel(_iosdCmdChannelPath, reqMsg);
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": StartNodeServices message sent to iosd for service id " << serviceId);

   return;
}

int
HwJobController::discoverNodeStep2(void)
{
   // Get pointer to inbound  message.
   bgcios::iosctl::ReadyMessage *inMsg = (bgcios::iosctl::ReadyMessage *)_inboundMessage;

   // Find the compute node to send the DiscoverNodeAck message to.
   ComputeNodePtr cnode = _cnodes.get(inMsg->serviceId);
   if (cnode == NULL) {
      LOG_ERROR_MSG("service id " << inMsg->serviceId << " was not found in compute node list");
      return ENOENT;
   }


   RdmaClientPtr client = cnode->getClient();
   cnode->setPort(inMsg->header.service,inMsg->port);
   LOG_CIOS_DEBUG_MSG("received Ready serviceId=" << inMsg->serviceId <<" service="<<(int)inMsg->header.service <<" port="<<inMsg->port);

   
   if (cnode->incToReadyCount()<2){
      return 0; //Need two Ready messages to send Ack to compute node
   }

   // Build DiscoverNodeAck message in outbound message region.
   DiscoverNodeAckMessage *outMsg = (DiscoverNodeAckMessage *)client->getOutboundMessagePtr();
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = DiscoverNodeAck;
   outMsg->header.length = sizeof(DiscoverNodeAckMessage);
   outMsg->header.returnCode = bgcios::Success;
   outMsg->serviceId = cnode->getServiceId();

   outMsg->sysiod_port = cnode->getsysiodPort();
   outMsg->toolctld_port = cnode->gettoolctldPort();
   if ( (outMsg->toolctld_port==0) or (outMsg->sysiod_port==0) ){
      LOG_ERROR_MSG("Zero port not expected:  << outMsg->toolctld_port="<<outMsg->toolctld_port<<"outMsg->sysiod_port=" << outMsg->sysiod_port);
   }
   LOG_CIOS_DEBUG_MSG("outMsg->sysiod_port ="<<outMsg->sysiod_port<<" outMsg->toolctld_port="<<outMsg->toolctld_port);
   outMsg->deviceAddress = _boundServer->getLocalIPv4Address();
   struct timeval now;
   gettimeofday(&now, NULL);
   outMsg->currentTime = ((uint64_t)now.tv_sec * bgcios::MicrosecondsPerSecond) + (uint64_t)now.tv_usec;
   struct utsname ubuf;
   uname(&ubuf);
   strncpy(outMsg->nodeName, ubuf.nodename, MaxUtsnameSize);
   strncpy(outMsg->release, ubuf.release, MaxUtsnameSize);

   // Send DiscoverNodeAck message to compute node.
   try {
      CIOSLOGMSG_QP(BGV_SEND_MSG,outMsg,client->getQpNum() );
      client->postSendMsgSignaled(outMsg->header.length);
   }
   catch (const RdmaError& e) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error posting DiscoverNodeAck message for service id " << outMsg->serviceId <<
                    " for queue pair " << client->getQpNum() << ": " << bgcios::errorString(e.errcode()));
      return e.errcode();
   }
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": DiscoverNodeAck message for service id " << outMsg->serviceId << " is ready for queue pair " << client->getQpNum());

   return 0;
}

int
HwJobController::setupJob(void)
{
   // Get pointer to inbound SetupJob message.
   SetupJobMessage *inMsg = (SetupJobMessage *)_inboundMessage;

   // Build SetupJobAck message in outbound buffer.
   SetupJobAckMessage *outMsg = (SetupJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = SetupJobAck;
   outMsg->header.length = sizeof(SetupJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Construct Job object to track the job and add it to the list.
   JobPtr job(new Job(inMsg->header.jobId, inMsg->blockId, inMsg->corner, inMsg->shape, inMsg->jobLeader, inMsg->numRanks));
   _jobs.add(inMsg->header.jobId, job);
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": added new job (" << *job << ") to list");

   // Add the compute nodes that fit in the job.
   for (cnode_list_iterator iter = _cnodes.begin(); iter != _cnodes.end(); ++iter) {
      ComputeNodePtr cnode = iter->second;
      if (job->isComputeNodeInJob(cnode)) {
         job->addComputeNode(cnode);
         LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": added compute node (" << *cnode << ") to job");
      }
   }
   LOG_CIOS_INFO_MSG_FORCED("Job " << inMsg->header.jobId << " added with " << job->numComputeNodes() << " compute nodes" );

   // Make sure there is at least one compute node in the job.
   if (job->numComputeNodes() == 0) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": no compute nodes matched the specifications for job with " << *job);
      outMsg->header.returnCode = bgcios::NodesInJobError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   // Initialize accumulators for the job.
   job->setupJobAckAccumulator.setLimit((int)job->numComputeNodes());
   job->loadJobAckAccumulator.setLimit((int)job->numComputeNodes());
   job->startJobAckAccumulator.setLimit((int)job->numComputeNodes());
   job->cleanupJobAckAccumulator.setLimit((int)job->numComputeNodes());
   job->signalJobAckAccumulator.setLimit((int)job->numComputeNodes());
   job->exitProcessAccumulator.setLimit(1); // Only one message is sent by CNK
   job->exitJobAccumulator.setLimit(1); // Only one message is sent by CNK

   // Forward the SetupJob message to all of the compute nodes in the job.
   job->sendMessageToAllNodes(&(inMsg->header));
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": SetupJob message forwarded to " << job->numComputeNodes() << " compute nodes");

   return 0;
}

void
HwJobController::setupJobAck(bgcios::MessageHeader * mh)
{
   // Get pointer to inbound SetupJobAck message.
   SetupJobAckMessage *inMsg = (SetupJobAckMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, SetupJobAck message was ignored");
      return;
   }

   // Accumulate messages and forward one message on data channel when all messages have been received.
   const bool ready = job->setupJobAckAccumulator.add(&(inMsg->header));
   if (ready) {
      const int err = sendToDataChannel((void *)job->setupJobAckAccumulator.get());
      if (err == 0) {
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": SetupJobAck message sent on data channel");
         job->setupJobAckAccumulator.resetCount();
      }
      else {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending SetupJobAck message on data channel: " << bgcios::errorString(err));
      }
   }
    


   return;
}

int
HwJobController::loadJob(void)
{
   // Get pointer to inbound LoadJob message.
   LoadJobMessage *inMsg = (LoadJobMessage *)_inboundMessage;

   // Build LoadJobAck message in outbound buffer (only used if error is found).
   LoadJobAckMessage *outMsg = (LoadJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = LoadJobAck;
   outMsg->header.length = sizeof(LoadJobAckMessage);

   // Validate the job id.
   JobPtr job =_jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " was not found when handling LoadJob message");
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   // Setup on the I/O node before running the job.
   bgcios::MessageResult result = job->setup(inMsg);
   if (result.isError()) {
      result.setHeader(outMsg->header);
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error running I/O node setup: " << bgcios::errorString(result.errorCode()));
      return sendToDataChannel(outMsg);
   }


   if (inMsg->numSecondaryGroups >= MaxGroups){
      LOG_CIOS_WARN_MSG("Max 2ndary groups Job:"<<inMsg->header.jobId<<" inMsg->userId("<<inMsg->userId<<") inMsg->groupId("<<inMsg->groupId<<") number secondary groups="<<inMsg->numSecondaryGroups);
#if 0
      for (int i=0;i<inMsg->numSecondaryGroups;i++){
        int gid = (int)inMsg->secondaryGroups[i];
        LOG_INFO_MSG_FORCED("i="<<i<<" groupId="<< gid );
      }
#endif 
   } 

   // Forward the LoadJob message to all of the compute nodes in the job.

   job->sendMessageToAllNodes(&(inMsg->header));
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": LoadJob message forwarded to " << job->numComputeNodes() << " compute nodes");

   return 0;
}

void
HwJobController::loadJobAck(bgcios::MessageHeader * mh)
{
   // Get pointer to inbound LoadJobAck message.
   LoadJobAckMessage *inMsg = (LoadJobAckMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, LoadJobAck message received from completion channel was ignored");
      return;
   }

   // Accumulate messages and forward one message on data channel when all messages have been received.
   bool ready = job->loadJobAckAccumulator.add(&(inMsg->header));

   if (ready) {
      LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": LoadJobAck message received from " << job->loadJobAckAccumulator.getLimit() << " compute nodes");

      // If there was an error reported by the compute nodes or a prolog program is not specified, forward LoadJobAck message.
      LoadJobAckMessage *outMsg = (LoadJobAckMessage *)(job->loadJobAckAccumulator.get());
      std::string prologProgramPath = _config->getJobPrologProgramPath();

      if ((outMsg->header.returnCode != bgcios::Success) || (prologProgramPath.empty())) {
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": LoadJobAck message sent on data channel");
         int err = sendToDataChannel(outMsg);
         if (err == 0) {
            job->loadJobAckAccumulator.resetCount();
         }
         else {
            LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending LoadJobAck message on data channel: " << bgcios::errorString(err));
         }
      }

      // Run the prolog program and forward LoadJobAck message if there is a problem starting program.
      else {
         int err = job->runPrologProgram(prologProgramPath, _config->getJobPrologProgramTimeout());
         if (err != 0) {
            outMsg->header.returnCode = bgcios::PrologPgmStartError;
            outMsg->header.errorCode = (uint32_t)err;
            err = sendToDataChannel(outMsg);
            if (err != 0) {
               LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending LoadJobAck message on data channel: " << bgcios::errorString(err));
            }
         }
      }
   }

    

   return;
}

int
HwJobController::loadJobAck(void)
{
   // Get pointer to inbound LoadJobAck message.
   LoadJobAckMessage *inMsg = (LoadJobAckMessage *)_inboundMessage;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, LoadJobAck message received from command channel was ignored");
      return 0;
   }

   // Forward LoadJobAck message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": LoadJobAck message forwarded on data channel (received from command channel)");
   int err = sendToDataChannel(inMsg);
   if (err == 0) {
      job->loadJobAckAccumulator.resetCount();
   }
   else {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending LoadJobAck message on data channel: " << bgcios::errorString(err));
   }

   return err;
}

int
HwJobController::startJob(void)
{
   // Get pointer to inbound StartJob message.
   StartJobMessage *inMsg = (StartJobMessage *)_inboundMessage;

   // Build StartJobAck message in outbound buffer.
   StartJobAckMessage *outMsg = (StartJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = StartJobAck;
   outMsg->header.length = sizeof(StartJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Validate the job id.
   JobPtr job =_jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " was not found when handling StartJob message");
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   struct timeval now;
   gettimeofday(&now, NULL);
   const uint64_t currentTime = ((uint64_t)now.tv_sec * bgcios::MicrosecondsPerSecond) + (uint64_t)now.tv_usec;
   if ( !inMsg->currentTime ) {
       // backwards compatible with V1R2M0, get the current time for the compute nodes.
       inMsg->currentTime = currentTime;
   } else {
       const uint64_t difference = currentTime > inMsg->currentTime ? currentTime - inMsg->currentTime : inMsg->currentTime - currentTime;
       const uint64_t threshold = _config->getStartTimeThreshold();
       if ( threshold && difference > threshold ) {
           LOG_CIOS_INFO_MSG_FORCED("Job " << inMsg->header.jobId << " " << difference / 1000.0l << "ms difference" );
       }
   }

   //! \todo Why do we care about the numRanksForIONode field.

   // Forward the StartJob message to all of the compute nodes in the job.
   job->sendMessageToAllNodes(&(inMsg->header));
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": StartJob message forwarded to " << job->numComputeNodes() << " compute nodes");

   return 0;
}

void
HwJobController::startJobAck(bgcios::MessageHeader * mh)
{
   // Get pointer to inbound StartJobAck message.
   StartJobAckMessage *inMsg = (StartJobAckMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, StartJobAck message was ignored");
      return;
   }

   // Accumulate messages and forward one message on data channel when all messages have been received.
   bool ready = job->startJobAckAccumulator.add(&(inMsg->header));
   if (ready) {
      LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": StartJobAck message received from " << job->startJobAckAccumulator.getLimit() << " compute nodes");

      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": StartJobAck message sent on data channel");
      int err = sendToDataChannel((void *)job->startJobAckAccumulator.get());
      if (err == 0) {
         job->startJobAckAccumulator.resetCount();
      }
      else {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending StartJobAck message on data channel: " << bgcios::errorString(err));
      }
   }

    

   return;
}

int
HwJobController::startTool(void)
{
   // Get pointer to inbound StartTool message.
   StartToolMessage *inMsg = (StartToolMessage *)_inboundMessage;

   // Build StartToolAck message in outbound buffer.
   StartToolAckMessage *outMsg = (StartToolAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = StartToolAck;
   outMsg->header.length = sizeof(StartToolAckMessage);
   outMsg->header.returnCode = bgcios::Success;
   outMsg->toolId = inMsg->toolId;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   // Start the tool and add it to the job.
   bgcios::MessageResult result = job->startTool(inMsg);
   if (result.isError()) {
      result.setHeader(outMsg->header);
      return sendToDataChannel(outMsg);
   }
   ++_activeTools; // Increment so eventHandler() method checks for status of tool processes.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": " << _activeTools << " tools are active on node");

   // Send StartToolAck message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": StartToolAck message sent on data channel");
   return sendToDataChannel(outMsg); 
}

int
HwJobController::endTool(void)
{
   // Get pointer to inbound EndTool message.
   EndToolMessage *inMsg = (EndToolMessage *)_inboundMessage;

   // Build EndToolAck message in outbound buffer.
   EndToolAckMessage *outMsg = (EndToolAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = EndToolAck;
   outMsg->header.length = sizeof(EndToolAckMessage);
   outMsg->header.returnCode = Success;
   outMsg->toolId = inMsg->toolId;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   // End the tool with the specified signal.
   bgcios::MessageResult result = job->endTool(inMsg->toolId, inMsg->signo);
   if (result.isError()) {
      result.setHeader(outMsg->header);
      return sendToDataChannel(outMsg);
   }

   // Send EndToolAck message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": EndToolAck message sent on data channel");
   return sendToDataChannel(outMsg); 
}

int
HwJobController::checkToolStatus(void)
{
   // Get pointer to inbound CheckToolStatus message.
   CheckToolStatusMessage *inMsg = (CheckToolStatusMessage *)_inboundMessage;

   // Build CheckToolStatusAck message in outbound buffer.
   CheckToolStatusAckMessage *outMsg = (CheckToolStatusAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = CheckToolStatusAck;
   outMsg->header.length = sizeof(CheckToolStatusAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   // Find out how many tools are still active.
   outMsg->activeTools = job->checkAllToolsStatus(inMsg->seconds);

   // Send CheckToolStatusAck message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": CheckToolStatusAck message sent on data channel");
   return sendToDataChannel(outMsg); 
}

int
HwJobController::exitTool(JobPtr job, uint32_t toolId)
{
   // Build ExitTool message in outbound buffer.
   ExitToolMessage *outMsg = (ExitToolMessage *)_outboundMessage;
   outMsg->header.service = bgcios::JobctlService;
   outMsg->header.version = ProtocolVersion;
   outMsg->header.type = ExitTool;
   outMsg->header.rank = 0;
   outMsg->header.sequenceId = 0;
   outMsg->header.returnCode = bgcios::Success;
   outMsg->header.errorCode = 0;
   outMsg->header.length = sizeof(ExitToolMessage);
   outMsg->header.jobId = job->getJobId();
   outMsg->toolId = toolId;

   // Get the status of the tool.
   outMsg->status = job->waitForTool(toolId);

   // Send ExitTool message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << outMsg->header.jobId << ": ExitTool message sent on data channel");
   return sendToDataChannel(outMsg); 
}

int
HwJobController::exitToolAck(void)
{
   // Get pointer to inbound ExitToolAck message.
   ExitToolAckMessage *inMsg = (ExitToolAckMessage *)_inboundMessage;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Note this can happen when the tool is ended as a part of ending the job.  It is not considered an error.
      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << " was not found when handling ExitToolAck message so it was ignored");
      return 0;
   }

   // Remove the tool from the job.
   bgcios::MessageResult result = job->removeTool(inMsg->toolId);
   if (result.isSuccess()) {
      --_activeTools; // Decrement so eventHandler() method can stop checking for status.
      LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": " << _activeTools << " tools are active on node");
   }
   else {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": tool " << inMsg->toolId << " was not removed: " << bgcios::returnCodeToString(result.returnCode()));
   }

   return 0;
}

int
HwJobController::signalJob(void)
{
   // Get pointer to inbound SignalJob message.
   SignalJobMessage *inMsg = (SignalJobMessage *)_inboundMessage;

   // Build SignalJobAck message in outbound buffer.
   SignalJobAckMessage *outMsg = (SignalJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = SignalJobAck;
   outMsg->header.length = sizeof(SignalJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " was not found when handling SignalJob message");
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   // Build an Interrupt message.
   bgcios::iosctl::InterruptMessage interruptMsg;
   interruptMsg.header.service = bgcios::IosctlService;
   interruptMsg.header.version = bgcios::iosctl::ProtocolVersion;
   interruptMsg.header.type = bgcios::iosctl::Interrupt;
   interruptMsg.header.rank = inMsg->header.rank;
   interruptMsg.header.sequenceId = inMsg->header.sequenceId;
   interruptMsg.header.returnCode = bgcios::Success;
   interruptMsg.header.errorCode = 0;
   interruptMsg.header.length = sizeof(bgcios::iosctl::InterruptMessage);
   interruptMsg.header.jobId = inMsg->header.jobId;
   interruptMsg.signo = inMsg->signo;

   // Tell stdiod to interrupt any outstanding operations.
   std::ostringstream stdiodCmdChannelPath;
   stdiodCmdChannelPath << _workDirectory << bgcios::StdioCommandChannelName;
   int err = sendToCommandChannel(stdiodCmdChannelPath.str(), &interruptMsg);
   if (err != 0) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending Interrupt message to '" << stdiodCmdChannelPath.str() << "': " << bgcios::errorString(err));
   }

   // Tell all of the sysiod daemons to interrupt any outstanding operations.
   for (cnode_list_iterator iter = _cnodes.begin(); iter != _cnodes.end(); ++iter) {
      const ComputeNodePtr cnode = iter->second;
      
      // only send interrupt to sysio daemons for this job
      if ( !job->isComputeNodeInJob(cnode)) continue;
     
      std::ostringstream sysiodCmdChannelPath;
      sysiodCmdChannelPath << _workDirectory << bgcios::SysioCommandChannelName << "." << cnode->getServiceId();
      err = sendToCommandChannel(sysiodCmdChannelPath.str(), &interruptMsg);
      if (err != 0) {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending Interrupt message to '" << sysiodCmdChannelPath.str() << "': " << bgcios::errorString(err));
      }
   }

   // Forward the SignalJob message to all of the compute nodes in the job.
   job->sendMessageToAllNodes(&(inMsg->header));
   LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": SignalJob message for signal " << inMsg->signo <<
                       " forwarded to " << job->numComputeNodes() << " compute nodes");

   // End all of the tools if the signal is SIGKILL and the job must end.
   if (inMsg->signo == SIGKILL) {
      job->endAllTools(SIGTERM); // Give the tools a chance to cleanup and end.
   }
   return 0;
}

void
HwJobController::signalJobAck(bgcios::MessageHeader * mh)
{
   // Get pointer to inbound SignalJobAck message.
   SignalJobAckMessage *inMsg = (SignalJobAckMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, SignalJobAck message was ignored");
      return;
   }

   // Accumulate messages and forward one message on data channel when all messages have been received.
   bool ready = job->signalJobAckAccumulator.add(&(inMsg->header));
   if (ready) {
      LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": SignalJobAck message received from " << job->signalJobAckAccumulator.getLimit() << " compute nodes");

      LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": SignalJobAck message sent on data channel");
      int err = sendToDataChannel((void *)job->signalJobAckAccumulator.get());
      if (err == 0) {
         job->signalJobAckAccumulator.resetCount();
      }
      else {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending SignalJobAck message on data channel: " << bgcios::errorString(err));
      }
   }
   return;
}

int
HwJobController::cleanupJob(void)
{
   // Get pointer to the inbound CleanupJob message.
   CleanupJobMessage *inMsg = (CleanupJobMessage *)_inboundMessage;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " was not found when handling CleanupJob message");

      // Build CleanupJobAck message in outbound buffer.
      CleanupJobAckMessage *outMsg = (CleanupJobAckMessage *)_outboundMessage;
      memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
      outMsg->header.type = CleanupJobAck;
      outMsg->header.length = sizeof(CleanupJobAckMessage);
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ESRCH;
      return sendToDataChannel(outMsg);
   }

   if (inMsg->killTimeout) {
       // delivering a KILL signal to the job timed out, we should cleanup now
       // rather than wait for the compute nodes to respond
       job->cleanup();
       _jobs.remove(job->getJobId());

       // no ack is necessary for this message

       return 0;
   }

   // Forward the CleanupJob message to all of the compute nodes in the job.
   job->sendMessageToAllNodes(&(inMsg->header));
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": CleanupJob message forwarded to " << job->numComputeNodes() << " compute nodes");

   return 0;
}

void
HwJobController::cleanupJobAck(bgcios::MessageHeader * mh)
{
   // Get pointer to inbound CleanupJobAck message.
   CleanupJobAckMessage *inMsg = (CleanupJobAckMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " was not found, CleanupJobAck message was ignored");
      return;
   }

   // Accumulate messages and forward one message on data channel when all messages have been received.
   const bool ready = job->cleanupJobAckAccumulator.add(inMsg);
   if (ready) {
      // If an epilog program is not specified, forward CleanupJobAck message.
      CleanupJobAckMessage *outMsg = job->cleanupJobAckAccumulator.get();
      const std::string epilogProgramPath = _config->getJobEpilogProgramPath();

      // By default, finish the cleanup and forward the CleanupJobAck message from here.
      bool cleanup = true;

      // Run the epilog program if needed.
      if (!epilogProgramPath.empty()) {
         int err = job->runEpilogProgram(epilogProgramPath, _config->getJobEpilogProgramTimeout());
         if (err == 0) {
            cleanup = false;
         }
         else {
            outMsg->header.returnCode = bgcios::EpilogPgmStartError;
            outMsg->header.errorCode = (uint32_t)err;
         }
      }

      // Finish cleanup from here.
      if (cleanup) {
         _activeTools -= job->numTools();
         job->cleanup();
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": " << _activeTools << " tools are active on node");

         // Forward the CleanupJobAck message on data channel.
         LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": CleanupJobAck message sent on data channel");
         int err = sendToDataChannel(outMsg);
         if (err == 0) {
            // Remove the job from the map and destroy the Job object.
            job->cleanupJobAckAccumulator.resetCount();
            _jobs.remove(job->getJobId());
         }
         else {
            LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending CleanupJobAck message on data channel: " << bgcios::errorString(err));
         }
      }
   }

    

   return;
}

int
HwJobController::cleanupJobAck(void)
{
   // Get pointer to inbound CleanupJobAck message.
   CleanupJobAckMessage *inMsg = (CleanupJobAckMessage *)_inboundMessage;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, CleanupJobAck message received from command channel was ignored");
      return 0;
   }

   // Finish cleanup of job.
   _activeTools -= job->numTools();
   job->cleanup();
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": " << _activeTools << " tools are active on node");
   _jobs.remove(job->getJobId());

   // Forward CleanupJobAck message on data channel.
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": CleanupJobAck message sent on data channel");
   return sendToDataChannel(inMsg);
}

int
HwJobController::reconnect(void)
{
   // Get pointer to inbound Reconnect message.
   ReconnectMessage *inMsg = (ReconnectMessage *)_inboundMessage;

   // Build ReconnectAck message in outbound buffer.
   ReconnectAckMessage *outMsg = (ReconnectAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = ReconnectAck;
   outMsg->header.length = sizeof(ReconnectAckMessage);
   outMsg->header.returnCode = Success;

   // remember jobs to remove after iterating through our container
   std::queue<uint64_t> jobsToRemove;

   // Run the list of jobs and resend any accumulated messages that are ready.
   for (job_list_iterator iter = _jobs.begin(); iter != _jobs.end(); ++iter) {
      JobPtr job = iter->second;
      int err = 0;

      if (job->setupJobAckAccumulator.atLimit()) {
         err = sendToDataChannel((void *)job->setupJobAckAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": SetupJobAck message sent on data channel when handling Reconnect message (" <<
                       job->setupJobAckAccumulator.getLimit() << " compute nodes)");
         if (err == 0) {
            job->setupJobAckAccumulator.resetCount();
         }
      }

      if (job->loadJobAckAccumulator.atLimit()) {
         err = sendToDataChannel((void *)job->loadJobAckAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": LoadJobAck message sent on data channel when handling Reconnect message (" <<
                       job->loadJobAckAccumulator.getLimit() << " compute nodes)");
         if (err == 0) {
            job->loadJobAckAccumulator.resetCount();
         }
      }

      if (job->startJobAckAccumulator.atLimit()) {
         err = sendToDataChannel((void *)job->startJobAckAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": StartJobAck message sent on data channel when handling Reconnect message (" <<
                       job->startJobAckAccumulator.getLimit() << " compute nodes)");
         if (err == 0) {
            job->startJobAckAccumulator.resetCount();
         }
      }

      if (job->exitProcessAccumulator.atLimit()) {
         err = sendToDataChannel((void *)job->exitProcessAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": ExitProcess message sent on data channel when handling Reconnect message");
         if (err == 0) {
            job->exitProcessAccumulator.resetCount();
         }
      }

      if (job->exitJobAccumulator.atLimit()) {
         err = sendToDataChannel((void *)job->exitJobAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": ExitJob message sent on data channel when handling Reconnect message");
         if (err == 0) {
            job->exitJobAccumulator.resetCount();
         }
      }

      if (job->cleanupJobAckAccumulator.atLimit()) {
         err = sendToDataChannel((void *)job->cleanupJobAckAccumulator.get());
         LOG_CIOS_DEBUG_MSG("Job " << job->getJobId() << ": CleanupJobAck message sent on data channel when handling Reconnect message (" <<
                       job->cleanupJobAckAccumulator.getLimit() << " compute nodes)");

         if (err == 0) {
            job->cleanupJobAckAccumulator.resetCount();
            jobsToRemove.push( job->getJobId() );
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

void
HwJobController::exitProcess(const RdmaClientPtr& client,bgcios::MessageHeader * mh)
{
   // Get pointer to inbound ExitProcess message.
   ExitProcessMessage *inMsg = (ExitProcessMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, ExitProcess message was ignored");
      return;
   }

   // Forward ExitProcess message on data channel.
// job->setExitStatus(inMsg->status);
   if (WIFSIGNALED(inMsg->status)) {
      LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": ExitProcess message sent on data channel (rank " << inMsg->header.rank <<
                          " terminated by signal " << WTERMSIG(inMsg->status) << ")");
   }
   else {
      LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << ": ExitProcess message sent on data channel (rank " << inMsg->header.rank <<
                          " ended with exit status " << WEXITSTATUS(inMsg->status) << ")");
   }
   int err = sendToDataChannel(inMsg);
   if (err != 0) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending ExitProcess message on data channel: " << bgcios::errorString(err));
      job->exitProcessAccumulator.add(inMsg);
   }

   // Build ExitProcessAck message in outbound message region.
   ExitProcessAckMessage *outMsg = (ExitProcessAckMessage *)client->getOutboundMessagePtr();
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ExitProcessAck;
   outMsg->header.length = sizeof(ExitProcessAckMessage);
   outMsg->header.returnCode = bgcios::Success;
   if (err != 0) {
      LOG_ERROR_MSG("error forwarding ExitProcess message to data channel: " << bgcios::errorString(err));
      outMsg->header.returnCode = bgcios::SendError;
      outMsg->header.errorCode = (uint32_t)err;
   }

   client->setOutboundMessageLength(outMsg->header.length);
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": ExitProcessAck message is ready for queue pair " << client->getQpNum());
   return;
}

void
HwJobController::exitJob(const RdmaClientPtr& client,bgcios::MessageHeader * mh)
{
   // Get pointer to inbound ExitJob message.
   ExitJobMessage *inMsg = (ExitJobMessage *)mh;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << " is not active, ExitJob message was ignored");
      return;
   }
   LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << ": ExitJob message sent on data channel (status 0x" << std::hex << std::setw(8) << std::setfill('0') << inMsg->status << ")");

   // Forward ExitJob message on data channel.
   int err = sendToDataChannel(inMsg);
   if (err != 0) {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending ExitJob message on data channel: " << bgcios::errorString(err));
      job->exitJobAccumulator.add(inMsg);
   }

   // Build ExitJobAck message in outbound message region.
   ExitJobAckMessage *outMsg = (ExitJobAckMessage *)client->getOutboundMessagePtr();
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ExitJobAck;
   outMsg->header.length = sizeof(ExitJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;
   if (err != 0) {
      LOG_ERROR_MSG("error forwarding ExitJob message to data channel: " << bgcios::errorString(err));
      outMsg->header.returnCode = bgcios::SendError;
      outMsg->header.errorCode = (uint32_t)err;
   }

   client->setOutboundMessageLength(outMsg->header.length);
   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": ExitJobAck message is ready for queue pair " << client->getQpNum());
   return;
}

int
HwJobController::changeConfig(void)
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
      LOG_INFO_MSG_FORCED("Changed ibm.cios.common log level to '" << inMsg->commonTraceLevel << "'"); 
   }
   setLoggingLevel("ibm.cios.jobctld", inMsg->jobctldTraceLevel);
   if (inMsg->jobctldTraceLevel != 0) {
      LOG_INFO_MSG_FORCED("Changed ibm.cios.jobctld log level to '" << inMsg->jobctldTraceLevel << "'");  
   }

   LOG_CIOS_DEBUG_MSG("Job " << inMsg->header.jobId << ": ChangeConfigAck message sent on data channel");
   return sendToDataChannel(outMsg);
}

int
HwJobController::terminate(void)
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

   // Send TerminateAck message.
   return sendToCommandChannel(_iosdCmdChannelPath, outMsg);
}

