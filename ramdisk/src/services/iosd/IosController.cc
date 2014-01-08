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

//! \file  IosController.cc
//! \brief Methods for bgcios::iosctl::IosController class.

// Includes
#include "IosController.h"
#include <ramdisk/include/services/IosctlMessages.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>
#include <poll.h>
#include <errno.h>
#include <stdlib.h>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");


IosController::~IosController()
{
   free(_inboundMessage);
   free(_outboundMessage);
}

int
#ifdef LOG_CIOS_INFO_DISABLE
IosController::startup(in_port_t)
#else
IosController::startup(in_port_t dataChannelPort)
#endif
{
   LOG_CIOS_INFO_MSG("data channel listening on port " << dataChannelPort << " was ignored");
   return 0;
}

void
IosController::eventMonitor(bgcios::SigtermHandler& sigtermHandler)
{
   const int cmdChannel   = 0;
   const int dataChannel  = 1;
   const int dataListener = 2;
   const int numFds       = 3;

   pollfd pollInfo[numFds];
   int timeout = 1000; // Wakeup every second to check for termination.

   // Initialize the pollfd structure.
   pollInfo[cmdChannel].fd = _cmdChannel->getSd();
   pollInfo[cmdChannel].events = POLLIN;
   pollInfo[cmdChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

   pollInfo[dataChannel].fd = _dataChannel == NULL ? -1 : _dataChannel->getSd();
   pollInfo[dataChannel].events = POLLIN;
   pollInfo[dataChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added data channel using fd " << pollInfo[dataChannel].fd << " to descriptor list");

   pollInfo[dataListener].fd = _dataListener->getSd();
   pollInfo[dataListener].events = POLLIN;
   pollInfo[dataListener].revents = 0;
   LOG_CIOS_TRACE_MSG("added data channel listener using fd " << pollInfo[dataListener].fd << " to descriptor list");

   // Process events until told to stop.
   while ((!_done) && (!sigtermHandler.isCaught())) {

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
            LOG_CIOS_TRACE_MSG("interrupted while polling for events");
            continue;
         }

         LOG_ERROR_MSG("error polling for events: " << bgcios::errorString(err));
         return;
      } 

      // Check for an event on the command channel.
      if (pollInfo[cmdChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on command channel");
         commandChannelHandler();
         pollInfo[cmdChannel].revents = 0;
      }

      // Check for an event on the data channel.
      if (pollInfo[dataChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on data channel");
         dataChannelHandler();
         pollInfo[dataChannel].revents = 0;
      }

      // Check for an event on the data channel listener.
      if (pollInfo[dataListener].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on data channel listener");
         pollInfo[dataListener].revents = 0;

         // Make a new data channel connected to runjob.
         _dataChannel = makeDataChannel();
         if (!_dataChannel) {
            LOG_ERROR_MSG("error making new data channel");
            continue;
         }

         // Stop monitoring the data channel listener and start monitoring the data channel.
         pollInfo[dataListener].fd = -1;
         pollInfo[dataChannel].fd = _dataChannel->getSd();
      }

   }

   // Reset for next time.
   _done = false;

   return;
}

void
IosController::dataChannelHandler(void)
{
   // Receive a message from the data channel.
   int err = recvFromDataChannel(_inboundMessage, _dataChannel);

   // When data channel closes, stop handling events.
   if (err == EPIPE) {
      LOG_ERROR_MSG("data channel connected to " << _dataChannel->getPeerName() << " is closed");
      _done = true;
      _dataChannel.reset();
      return;
   }

   // An error occurred receiving a message.
   if (err != 0) {
      LOG_ERROR_MSG("error receiving message from data channel: " << bgcios::errorString(err));
      return;
   }

   // Make sure the service field is correct.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
   if (msghdr->service != bgcios::IosctlService) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << msghdr->service << " is wrong for " << bgcios::toString(msghdr) <<
                    " message received from data channel, header: " << bgcios::printHeader(*msghdr));
      sendErrorAckToDataChannel(bgcios::WrongService, 0);
      return;
   }

   // Make the sure protocol version is a match.
   if (msghdr->version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message protocol version " << (int)msghdr->version << " does not match iosd version " <<
                    (int)ProtocolVersion << " for " << toString(msghdr->type) << " message received from data channel");
      sendErrorAckToDataChannel(bgcios::VersionMismatch, ProtocolVersion);
      return;
   }

   // Handle the message.
   LOG_CIOS_DEBUG_MSG(toString(msghdr->type) << " message is available on data channel");
   switch (msghdr->type) {

      default:
         LOG_ERROR_MSG("Message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         msghdr->returnCode = bgcios::UnsupportedType;
         msghdr->length = sizeof(bgcios::MessageHeader);
         err = sendToDataChannel(msghdr);
         break;
   }

   if (err != 0) {
      LOG_ERROR_MSG("error handling " << toString(msghdr->type) << " message: " << bgcios::errorString(err));
   }
   return;
}

int
IosController::commandChannelHandler(void)
{
   // Receive a message from the command channel.
   std::string peer;
   int err = recvFromCommandChannel(peer, _inboundMessage);

   // An error occurred receiving a message.
   if (err != 0) {
      LOG_ERROR_MSG("error receiving message from command channel: " << bgcios::errorString(err));
      if (err == EPIPE) { // When command channel closes, stop handling events.
         _done = true;
      }
      return err;
   }

   // Make sure the service field is correct.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
   if (msghdr->service != bgcios::IosctlService) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << msghdr->service << " is wrong for " << bgcios::toString(msghdr) <<
                    " message received from command channel, header: " << bgcios::printHeader(*msghdr));
      sendErrorAckToCommandChannel(peer, bgcios::WrongService, 0);
      return 0;
   }

   // Make the sure protocol version is a match.
   if (msghdr->version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message protocol version " << (int)msghdr->version << " does not match iosd version " <<
                    (int)ProtocolVersion << " for " << toString(msghdr->type) << " message received from command channel '" << peer << "'");
      sendErrorAckToCommandChannel(peer, bgcios::VersionMismatch, ProtocolVersion);
      return 0;
   }

   // Handle the message.
   LOG_CIOS_DEBUG_MSG(toString(msghdr->type) << " message is available on command channel");
   switch (msghdr->type) {

      case Ready:
         err = ready(peer);
         break;

      case TerminateAck:
         _terminateAckAccumulator.add(msghdr);
         break;

      case StartNodeServices:
         err = startNodeServices(peer);
         break;

      case AllocateRegion:
         err = allocateRegion(peer);
         break;

      case ReleaseRegion:
         err = releaseRegion(peer);
         break;

      default:
         LOG_ERROR_MSG("message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         err = sendErrorAckToCommandChannel(peer, bgcios::UnsupportedType, 0);
         break;
   }

   if (err != 0) {
      LOG_ERROR_MSG("error sending ack message on command channel: " << bgcios::errorString(err));
   }

   return 0;
}

int
IosController::waitForReadyMessages(int numServices)
{
   // Set the receive timeout for the command channel.
   try {
      _cmdChannel->setRecvTimeout(60); //! \todo Does this timeout need to be configurable?
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error setting receive timeout before getting Ready messages: " << e.what());
      return e.errcode();
   }

   // Get messages from the command channel until all services have sent a Ready message.
   LOG_CIOS_DEBUG_MSG("waiting for " << numServices << " Ready messages ...");
   _readyAccumulator.setLimit(numServices);
   int rc = 0;
   while (!_readyAccumulator.atLimit()) {
      if ((rc = commandChannelHandler()) != 0) {
         break;
      }
   }

   // Reset the accumulator for the next time.
   _readyAccumulator.resetCount();
   _readyAccumulator.setLimit(0);

   // Reset the receive timeout for the command channel.
   try {
      _cmdChannel->setRecvTimeout(0);
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error setting receive timeout after getting Ready messages: " << e.what());
      if (rc == 0) {
         rc = e.errcode();
      }
   }

   return rc;
}

int
IosController::waitForTerminateAckMessages(int numServices)
{
   // Get messages from the command channel until all services have sent a TerminateAck message.
   LOG_CIOS_DEBUG_MSG("waiting for " << numServices << " TerminateAck messages ...");
   _terminateAckAccumulator.setLimit(numServices);
   while (!_terminateAckAccumulator.atLimit()) {
      commandChannelHandler();
   }

   // Reset the accumulator for the next time.
   _terminateAckAccumulator.resetCount();
   _terminateAckAccumulator.setLimit(0);

   return 0;
}

int
#ifdef LOG_CIOS_DEBUG_DISABLE
IosController::ready(std::string)
#else
IosController::ready(std::string dest)
#endif
{
   // Get pointer to Ready message available from inbound buffer.
   ReadyMessage *inMsg = (ReadyMessage *)_inboundMessage;
   LOG_CIOS_DEBUG_MSG("Ready message received from service at '" << dest << "'");  // Not sure this is needed?

   // If accumulating Ready messages, just increment the accumulator.
   if (_readyAccumulator.getLimit() > 0) {
      _readyAccumulator.add(&(inMsg->header));      
   }
   return 0;
}

int
#ifdef LOG_CIOS_WARN_DISABLE
IosController::startNodeServices(std::string)
#else
IosController::startNodeServices(std::string dest)
#endif
{

   // For now, this message is not supported.
   LOG_CIOS_WARN_MSG(toString(StartNodeServices) << " message is not supported yet dest="<<dest);
   return -1;
}

int
IosController::allocateRegion(std::string dest)
{
   // Get pointer to AllocateRegion message available from inbound buffer.
   AllocateRegionMessage *inMsg = (AllocateRegionMessage *)_inboundMessage;

   // Build AllocateRegionAck message in outbound buffer.
   AllocateRegionAckMessage *outMsg = (AllocateRegionAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = AllocateRegionAck;
   outMsg->header.length = sizeof(AllocateRegionAckMessage);

   // For now, this message is not supported.
   LOG_CIOS_WARN_MSG(toString(AllocateRegion) << " message is not supported yet");
   outMsg->header.returnCode = bgcios::RequestFailed;
   outMsg->header.errorCode = ENOTSUP;

   // Send AllocateRegionAck message.
   return sendToCommandChannel(dest, outMsg);
}

int
IosController::releaseRegion(std::string dest)
{
   // Get pointer to ReleaseRegion message available from inbound buffer.
   ReleaseRegionMessage *inMsg = (ReleaseRegionMessage *)_inboundMessage;

   // Build ReleaseRegionAck message in outbound buffer.
   ReleaseRegionAckMessage *outMsg = (ReleaseRegionAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ReleaseRegionAck;
   outMsg->header.length = sizeof(ReleaseRegionAckMessage);

   // For now, this message is not supported.
   LOG_CIOS_WARN_MSG(toString(ReleaseRegion) << " message is not supported yet");
   outMsg->header.returnCode = bgcios::RequestFailed;
   outMsg->header.errorCode = ENOTSUP;

   // Send ReleaseRegionAck message.
   return sendToCommandChannel(dest, outMsg);
}

