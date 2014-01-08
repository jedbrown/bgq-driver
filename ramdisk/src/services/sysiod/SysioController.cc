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
#include <ramdisk/include/services/common/RasEvent.h>
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
const uint64_t cycles_per_second = 1600000000;
void
SysioController::eventMonitor(void)
{
   const int cmdChannel   = 0;
   const int numFds       = 1;

   pollfd pollInfo[numFds];
   int timeout = 1000; // Wakeup every 1 seconds
   int syscallCheckCycler=0;
   const int oneMinute=60000; 

   uint64_t hang_value = cycles_per_second * 60 * 5; // 5 minutes of cycles

   // Initialize the pollfd structure.
   pollInfo[cmdChannel].fd = _cmdChannel->getSd();
   pollInfo[cmdChannel].events = POLLIN;
   pollInfo[cmdChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

   // Process events until told to stop.
   while (!_done) {

      // Wait for an event on one of the descriptors.
      int rc = poll(pollInfo, numFds, timeout);
      
      if (_clientMonitor->isDone()) {
            LOG_CIOS_INFO_MSG("client monitor in thread 0x" << std::hex << _clientMonitor->getThreadId() << " has ended");
            _done = true;
            _terminated = true;
            //_clientMonitor.reset();
            _cmdChannel.reset();
            _exit(EXIT_SUCCESS);
      }

      // Check on threads.
      if (rc == 0) {
         syscallCheckCycler += timeout;
         // When the client monitor thread ends, the connection closed.

         if (syscallCheckCycler >= oneMinute){
            syscallCheckCycler = 0; //restart 
            uint64_t sysCallStart = _clientMonitor->getSyscallStartTimeStamp();
            if (sysCallStart){
               uint64_t CurrentCycles = GetTimeBase();
               uint64_t diff = CurrentCycles - sysCallStart;
               if (diff >= hang_value) {
                   char * temp = _clientMonitor->getSyscallFileString1();
                   uint64_t seconds = diff/cycles_per_second; 

                   bgcios::MessageHeader * msghdr = (bgcios::MessageHeader * )_clientMonitor->getSyscallMessageHdr();
                   uint32_t entry = CIOSLOGMSG(BG_STUCK_MSG,msghdr);
                   //printLogEntry(entry);
                   //RAS here  
                   RasEvent ras(SysiodSyscallHangNoSignal,RasEvent::charMode);
                   char * buffer = ras.getRasBuff();
                   size_t buffsize = (size_t)ras.getRasBuffSize();
                   size_t length = snprintfLogEntry(buffsize, buffer, entry );
                   int strlen = (int)length;
                   buffer += length;
                   buffsize -= length;
                   strlen += (int)length;
                   length = (size_t)snprintf(buffer, buffsize," PID=%d syscall seconds=%llu",getpid(), (long long unsigned int)seconds);
                   buffer += length;
                   buffsize -= length;
                   strlen += (int)length;
                   if (temp) {
                      length = (size_t)snprintf(buffer, buffsize," involves file=%s",temp);
                       buffer += length;
                       buffsize -= length;
                       strlen += (int)length;
                   }
                   ras.setLength(strlen);
                   ras.send();
                   //LOG_INFO_MSG_FORCED("SysiodSyscallHangNOSignal " <<ras.getRasBuff() );  
              }                
           } 

         }
         interruptContinue();
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
   CIOSLOGMSG(CMD_RECV_MSG,msghdr);

   // Handle the message.
   LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message is available on command channel");
   switch (msghdr->type) {

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
   _clientMonitor->markJobKilled(inMsg->header.jobId);
    
    if (_clientMonitor->isJobRunning(inMsg->header.jobId) ){
       bgcios::iosctl::InterruptMessage *message = (bgcios::iosctl::InterruptMessage *)malloc(sizeof(bgcios::iosctl::InterruptMessage) );
       memcpy(message,_inboundMessage,sizeof(bgcios::iosctl::InterruptMessage) );
       _queuedInterruptMessages.push_back(message);
       uint64_t * startCycles = (uint64_t *)&message->signo;
       *startCycles=GetTimeBase();
    }   

   return 0;
}

int
SysioController::interruptContinue(){
    if ( ! _queuedInterruptMessages.empty() ) { 
        std::list<bgcios::iosctl::InterruptMessage *>::iterator it = _queuedInterruptMessages.begin();
        while(it != _queuedInterruptMessages.end()){
          bgcios::iosctl::InterruptMessage* imsg = *it;
          if (_clientMonitor->isJobRunning(imsg->header.jobId) ){ 
              uint64_t * startCycles = (uint64_t *)&imsg->signo;
              uint64_t CurrentCycles = GetTimeBase();
              uint64_t elapsedCycles = CurrentCycles - (*startCycles);
              if (  (imsg->header.errorCode==0)
                    &&
                    ( elapsedCycles >= (50 * cycles_per_second) ) ) {
                _clientMonitor->stopJobInternalKernel(imsg->header.jobId);
                imsg->header.errorCode=SIGUSR2;
                _clientMonitor->interrupt(SIGUSR2);  //interrupt
                it++; 
              }
              else if (  (imsg->header.errorCode==SIGUSR2)&&
                  ( elapsedCycles >= (60 * cycles_per_second) ) ) {
                it = _queuedInterruptMessages.erase(it);  
                free(imsg);
                //_clientMonitor->interrupt(SIGUSR1);  //interrupt and get flight log
                 
                 uint64_t sysCallStart = _clientMonitor->getSyscallStartTimeStamp();
                 if (sysCallStart){
                   uint64_t CurrentCycles = GetTimeBase();
                   uint64_t diff = CurrentCycles - sysCallStart;
                   char * temp = _clientMonitor->getSyscallFileString1();
                   uint64_t seconds = diff/cycles_per_second; 

                   bgcios::MessageHeader * msghdr = (bgcios::MessageHeader * )_clientMonitor->getSyscallMessageHdr();
                   uint32_t entry = CIOSLOGMSG(BG_STUCK_MSG,msghdr);
                   //printLogEntry(entry);
                   //RAS here  
                   RasEvent ras(SysiodSyscallHangOnSignal,RasEvent::charMode);
                   char * buffer = ras.getRasBuff();
                   size_t buffsize = (size_t)ras.getRasBuffSize();
                   size_t length = snprintfLogEntry(buffsize, buffer, entry );
                   int strlen = (int)length;
                   buffer += length;
                   buffsize -= length;
                   strlen += (int)length;
                   length = (size_t)snprintf(buffer, buffsize," PID=%d syscall seconds=%llu",getpid(), (long long unsigned int)seconds);
                   buffer += length;
                   buffsize -= length;
                   strlen += (int)length;
                   if (temp) {
                      length = (size_t)snprintf(buffer, buffsize," involves file=%s",temp);
                       buffer += length;
                       buffsize -= length;
                       strlen += (int)length;
                   }
                   ras.setLength(strlen);
                   ras.send();
                   //LOG_INFO_MSG_FORCED(ras.getRasBuff() );
                 } 
#if 0
                 else{ //no syscall hang, but timing out
                   LOG_INFO_MSG_FORCED("tail of log entries for job timeout");
                   printlastLogEntries(4);
                 }
#endif
               }
              
            }
            else {
              it = _queuedInterruptMessages.erase(it); //remove element and advance to next one in list
              free(imsg);
            }
 
        }//endwhile      
     }
return 0;    
}
