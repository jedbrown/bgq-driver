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

//! \file  SysioController.cc
//! \brief Methods for bgcios::sysio::SysioController class.

// Includes
#include "SysioController.h"
#include <ramdisk/include/services/common/Counter.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace bgcios::sysio;

LOG_DECLARE_FILE("cios.sysiod");


SysioController::SysioController() : bgcios::ServiceController()
{
   // Set work directory.
   _workDirectory = bgcios::WorkDirectory;

   // Set path to iosd command channel.
   std::ostringstream iosdPath;
   iosdPath << _workDirectory << bgcios::IosctlCommandChannelName;
   _iosdCmdChannelPath = iosdPath.str();

   // Initialize private data.
   _serviceId = 0;
}

SysioController::~SysioController()
{
   // Destroy the command channel socket.
   LOG_CIOS_DEBUG_MSG("destroying command channel " << _cmdChannel->getName());
   _cmdChannel.reset();
}

int
SysioController::startup(SysioConfigPtr config)
{
   // Reset umask to known value.
   ::umask(0);

   // Increase the number of descriptors that can be allocated to handle worst case.
   struct rlimit rl;
   rl.rlim_max = rl.rlim_cur = (125 * bgcios::MaxRanksPerNode) + 25; // Add some more for extra files needed by this daemon
   if (::setrlimit(RLIMIT_NOFILE, &rl) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error increasing NOFILE rlimit to " << rl.rlim_max << ": " << bgcios::errorString(err));
   }

   // Build the path to the command channel.
   _serviceId = config->getServiceId();
   std::ostringstream cmdChannelPath;
   cmdChannelPath << _workDirectory << bgcios::SysioCommandChannelName << "." << _serviceId;

   // Create the command channel socket.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(cmdChannelPath.str()));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating command channel '" << cmdChannelPath.str() << "': " << e.what());
      return e.errcode();
   }

   // Create a counter to wait for threads to complete their startup.
   bgcios::CounterPtr threadsReady = bgcios::CounterPtr(new bgcios::Counter());

   // Create and start a thread to monitor the RDMA client connection.
   _clientMonitor = ClientMonitorPtr(new ClientMonitor(threadsReady, config));
   _clientMonitor->start();
   LOG_CIOS_INFO_MSG("started client monitor in thread 0x" << std::hex << _clientMonitor->getThreadId());

   // Wait for all threads to complete startup.
   threadsReady->waitForValue(1);

   // See if all of the threads started successfully.
   if (_clientMonitor->isDone()) {
      return _clientMonitor->getLastError();
   }

   return 0;
}

int
SysioController::cleanup(void)
{
   return 0;
}

void
SysioController::eventMonitor(void)
{
   const int cmdChannel   = 0;
   const int numFds       = 1;

   pollfd pollInfo[numFds];
   int timeout = 1000; // Wakeup every second

   // Initialize the pollfd structure.
   pollInfo[cmdChannel].fd = _cmdChannel->getSd();
   pollInfo[cmdChannel].events = POLLIN;
   pollInfo[cmdChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

   // Process events until told to stop.
   while (!_done) {

      // Wait for an event on one of the descriptors.
      int rc = poll(pollInfo, numFds, timeout);

      // Check on threads.
      if (rc == 0) {
         // When the client monitor thread ends, the connection closed.
         if (_clientMonitor->isDone()) {
            LOG_CIOS_INFO_MSG("client monitor in thread 0x" << std::hex << _clientMonitor->getThreadId() << " has ended");
            _done = true;
            _terminated = true;
            _clientMonitor.reset();
         }
         continue;
      } 

      // There was an error so log the failure and try again.
      if (rc == -1) {
         int err = errno;
         if (err == EINTR) {
            LOG_CIOS_TRACE_MSG("poll returned EINTR, continuing ...");
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
   }

   // Reset for next time.
   _done = 0;

   return;
}

int
SysioController::commandChannelHandler(void)
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

   // Make sure the service field is correct.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;

   // Handle the message.
   LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message is available on command channel");
   switch (msghdr->type) {

      case bgcios::iosctl::Terminate:
         err = terminate(source);
         break;

      case bgcios::iosctl::Interrupt:
         if (msghdr->service == ToolctlService){
            err = interruptForToolCtl(source);
         }
         else {
            err = interrupt(source);
         }
         break;

      case bgcios::iosctl::ErrorAck:  break;
      case bgcios::jobctl::ErrorAck:  break;
      case bgcios::stdio::ErrorAck:   break;
      case bgcios::sysio::ErrorAck:   break;
      case bgcios::toolctl::ErrorAck: break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         err = sendErrorAckToCommandChannel(source, bgcios::UnsupportedType, ENOTSUP);
         break;
   }

   if (err == 0) {
      LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": ack message sent successfully");
   }
   else {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": error sending ack message: " << bgcios::errorString(err));
   }

   return 0;
}

int
SysioController::terminate(std::string source)
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
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
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
   return sendToCommandChannel(source, outMsg);
}

int 
SysioController::interruptForToolCtl(std::string source){
   (void)source;
   // Get pointer to Interrupt message available from inbound buffer.
   bgcios::iosctl::InterruptMessage *inMsg = (bgcios::iosctl::InterruptMessage *)_inboundMessage;

   _clientMonitor->addRankEINTRforJob(inMsg->header.jobId,inMsg->header.rank);
   int err = _clientMonitor->interrupt();
   return err;
}

int
SysioController::interrupt(std::string source)
{
   (void)source;
   // Get pointer to Interrupt message available from inbound buffer.
   bgcios::iosctl::InterruptMessage *inMsg = (bgcios::iosctl::InterruptMessage *)_inboundMessage;
   int err = 0;
   unsigned int time = 0;
   int do_once=1;
   if (!_clientMonitor->isJobRunning(inMsg->header.jobId) ) return 0;
    _clientMonitor->markJobKilled(inMsg->header.jobId);
    LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << " is running.  Interrupt source="<<source<< ": signal=" << inMsg->signo<<" Channel-name="<<_cmdChannel->getName() );
    // If the job is still running, signal the client monitor thread to break out of any blocked system calls.
    // Wait for the job to end.
    while (_clientMonitor->isJobRunning(inMsg->header.jobId)) {       
               sleep(KillJobTimeout);
               if (do_once==1){
                  time += KillJobTimeout;
                  if (time>5){
                    do_once=0;
                    err = _clientMonitor->interrupt();
                    LOG_INFO_MSG_FORCED("_serviceId="<<_serviceId<<" Jobid=" << inMsg->header.jobId << ": signal=" << inMsg->signo << " wait=" << time <<" thread 0x"<<
                          std::hex << _clientMonitor->getThreadId()<<" Channel-name="<<_cmdChannel->getName() );
                  }
               }
     }   
     LOG_CIOS_INFO_MSG("Job " << inMsg->header.jobId << " clean for sysiod.");

   return 0;
}
