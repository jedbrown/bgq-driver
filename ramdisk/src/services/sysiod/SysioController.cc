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
#include <cstring>
#include <ramdisk/include/services/common/SignalHandler.h>

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
   _doRasOnce = 1;
   _durationOfSlowSyscall = 0;
   _timeOfSlowSyscall = _timeOfHungSyscall = 0;
}

SysioController::~SysioController()
{
   // Destroy the command channel socket.
   LOG_CIOS_DEBUG_MSG("destroying command channel " << _cmdChannel->getName());
   _cmdChannel.reset();
}

const uint64_t cycles_per_second = 1600000000ULL;

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

   // Grab timeout values from the configuration:
   _SlowSyscallTimeout  = config->getSlowSyscallTimeout() * cycles_per_second;
   _HungSyscallTimeout  = config->getHungSyscallTimeout() * cycles_per_second;

   LOG_CIOS_INFO_MSG("Slow syscall timeout = " << config->getSlowSyscallTimeout() << " hung syscall timeout = " << config->getHungSyscallTimeout() );

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
   const int pipeForSig   = 1;
   const int numFds       = 2;

   pollfd pollInfo[numFds];
   _RAS4SyscallTimeout = 1;
   //No RAS messages for slow or hung syscalls while running for when either timeout is zero
   if (_SlowSyscallTimeout == 0)_RAS4SyscallTimeout=0; 
   if (_HungSyscallTimeout == 0)_RAS4SyscallTimeout=0; 
   //No RAS messages for syscalls still in progress when job is ending (signal)
   if (_RAS4SyscallTimeout==0) _doRasOnce = 0; 
   int timeout = 1000; // Wakeup every 1 seconds

   // Initialize the pollfd structure.
   pollInfo[cmdChannel].fd = _cmdChannel->getSd();
   pollInfo[cmdChannel].events = POLLIN;
   pollInfo[cmdChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

   bgcios::SigWritePipe SigWritePipe(SIGUSR1);
   pollInfo[pipeForSig].fd = SigWritePipe._pipe_descriptor[0];
   pollInfo[pipeForSig].events = POLLIN;
   pollInfo[pipeForSig].revents = 0;

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

	   uint64_t sysCallStart = _clientMonitor->getSyscallStartTimeStamp();
	   uint64_t currentTime = GetTimeBase();
	   uint64_t diff = (currentTime > sysCallStart) ? currentTime - sysCallStart : 0; // guard against jitter

	   // If there is a syscall in flight, and we have not already reported a slow syscall, then check
	   // to see if we are running slowly:
	   if ( (sysCallStart != 0) && ( _timeOfSlowSyscall == 0 ) ) {

	       if (diff >= _SlowSyscallTimeout) {
		   _timeOfSlowSyscall = sysCallStart;
		   if (_RAS4SyscallTimeout) reportHangNoSignal();
	       }
	   }

	   // If we previously reported a slow syscall, there are three possible outcomes:
	   //   1.  If the current sys call does not match the syscall that was slow, then we are no
	   //       longer slow.  In this case, we capture the duration.
	   //   2.  Otherwise, the current syscall does match the syscall that was reported slow and
	   //       either:
	   //         2a) enough time has transpired so that we are now considered hung  or
	   //         2b) we are still just slow.
	   if ( _timeOfSlowSyscall != 0 ) {

	       if ( _timeOfSlowSyscall != sysCallStart ) { // Option 1 above
		   _durationOfSlowSyscall = currentTime - _timeOfSlowSyscall;
		   _timeOfSlowSyscall = _timeOfHungSyscall = 0;
	       }
	       else {

		   if ( ( _timeOfHungSyscall == 0 ) && ( diff > _HungSyscallTimeout ) ) {
		       _timeOfHungSyscall = sysCallStart;
		       if (_RAS4SyscallTimeout) reportHangNoSignal();
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
      
      // Check for an event on the pipe for signal.
      if (pollInfo[pipeForSig].revents & POLLIN) {
         LOG_CIOS_DEBUG_MSG("input event available pipe from signal handler");
         pollInfo[pipeForSig].revents = 0;
         siginfo_t siginfo;
         ssize_t rc = ::read(pollInfo[pipeForSig].fd,&siginfo,sizeof(siginfo_t));
         CIOSLOGMSG_SG(BGV_RECV_SIG, &siginfo); 
         if (rc > 0){
           const size_t BUFSIZE = 1024;
           char buffer[BUFSIZE];
           const size_t HOSTSIZE = 256;
           char hostname[HOSTSIZE];
           hostname[0]=0;
           gethostname(hostname,HOSTSIZE);
           snprintf(buffer,BUFSIZE,"/var/spool/abrt/fl_sysiod-%d.%d.%s.log",_serviceId,getpid(),hostname);
           printLogMsg(buffer); 
           LOG_INFO_MSG_FORCED("Flight log: "<<buffer);
         }
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

   // Get pointer to Interrupt message available from inbound buffer.
   bgcios::iosctl::InterruptMessage *inMsg = (bgcios::iosctl::InterruptMessage *)_inboundMessage;
   CIOSLOGSTAT_SIG(BGV_STAT_SIG,inMsg->signo,source.c_str());
   if ( (inMsg->signo!=SIGKILL)   ) return 0;
   _clientMonitor->markJobKilled(inMsg->header.jobId);
    
    if (_clientMonitor->isJobRunning(inMsg->header.jobId) ){
       interrupt_tracker_t * tracker = (interrupt_tracker_t *)malloc(sizeof(bgcios::iosctl::InterruptMessage) );

       _queuedInterruptTracker.push_back(tracker);
       tracker->jobId = inMsg->header.jobId;
       tracker->startTimestamp=GetTimeBase();
       tracker->rank = inMsg->header.rank;  
       tracker->signo = (uint32_t)inMsg->signo;
       tracker->seconds = 0;
       tracker->progress_value = 0;
    }   

   return 0;
}

int
SysioController::interruptContinue(){
    if ( ! _queuedInterruptTracker.empty() ) { 
        std::list<interrupt_tracker_t *>::iterator it = _queuedInterruptTracker.begin();
        while(it != _queuedInterruptTracker.end()){
          interrupt_tracker_t* tracker = *it;
          if (_clientMonitor->isJobRunning(tracker->jobId) ){ 
              uint64_t CurrentCycles = GetTimeBase();
              uint64_t elapsedCycles = CurrentCycles - tracker->startTimestamp;
              if (  (tracker->progress_value==0)
                    &&
                    ( elapsedCycles >= (50 * cycles_per_second) ) ) {
                _clientMonitor->stopJobInternalKernel(tracker->jobId);
                tracker->progress_value=SIGUSR2;
                _clientMonitor->interrupt(SIGUSR2);  //interrupt
                it++; 
              }
              else if (  (tracker->progress_value==SIGUSR2)&&
                  ( elapsedCycles >= (60 * cycles_per_second) ) ) {
                it = _queuedInterruptTracker.erase(it);  
                free(tracker);
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
                   length = (size_t)snprintf(buffer, buffsize," syscall seconds=%llu _serviceId=%d ", (long long unsigned int)seconds,_serviceId);
                   buffer += length;
                   buffsize -= length;
                   strlen += (int)length;
                   if ( (temp!=NULL)&&( temp[0]!=0) ){
                      length = (size_t)snprintf(buffer, buffsize," involves file=%s",temp);
                       buffer += length;
                       buffsize -= length;
                       strlen += (int)length;
                   }
                   temp = _clientMonitor->getSyscallFileString2();
                  if ( (temp!=NULL)&&( temp[0]!=0) ){
                      length = (size_t)snprintf(buffer, buffsize," involves file=%s",temp);
                       buffer += length;
                       buffsize -= length;
                       strlen += (int)length;
                   }
                   int fd = _clientMonitor->getSyscallFd();
                   if (fd > 0){
                     char procFileBuff[256];
                     snprintf(procFileBuff,256,"/proc/%d/fd/%d",getpid(),fd);

                     length = (size_t)snprintf(buffer, buffsize," involves fd:%s->",procFileBuff);
                     buffer += length;
                     buffsize -= length;
                     strlen += (int)length;
                     
                     length = (size_t)readlink(procFileBuff,buffer,buffsize);
                   }
                   if (_doRasOnce){//Ras once
                     ras.setLength(strlen);
                     ras.send();
                     _doRasOnce = 0;
                   } 
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
              it = _queuedInterruptTracker.erase(it); //remove element and advance to next one in list
              free(tracker);
            }
 
        }//endwhile      
     }
return 0;    
}

void SysioController::reportHangNoSignal() {

    char * temp = _clientMonitor->getSyscallFileString1();
    uint64_t seconds = (GetTimeBase() - _timeOfSlowSyscall)/cycles_per_second; 

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
    length = (size_t)snprintf(buffer, buffsize," syscall %s for seconds=%llu _serviceId=%d ", (_timeOfHungSyscall != 0) ? "hung" : "slow", (long long unsigned int)seconds,_serviceId);
    buffer += length;
    buffsize -= length;
    strlen += (int)length;
    if ( _clientMonitor->getSyscallAccessLength() != 0 ) {
	length = (size_t)snprintf(buffer, buffsize,"length=%ld ", _clientMonitor->getSyscallAccessLength());
	buffer += length;
	buffsize -= length;
	strlen += (int)length;
    }
    if ( _durationOfSlowSyscall != 0 ) {
	length = (size_t)snprintf(buffer, buffsize,"previous=%llu ", (long long unsigned int)(_durationOfSlowSyscall/cycles_per_second));
	buffer += length;
	buffsize -= length;
	strlen += (int)length;
	_durationOfSlowSyscall = 0;
    }
    if ( (temp!=NULL)&&( temp[0]!=0) ){
	length = (size_t)snprintf(buffer, buffsize," involves file=%s",temp);
	buffer += length;
	buffsize -= length;
	strlen += (int)length;
    }
    temp = _clientMonitor->getSyscallFileString2();
    if ( (temp!=NULL)&&( temp[0]!=0) ){
	length = (size_t)snprintf(buffer, buffsize," involves file=%s",temp);
	buffer += length;
	buffsize -= length;
	strlen += (int)length;
    }
    int fd = _clientMonitor->getSyscallFd();
    if (fd > 0){
	char procFileBuff[256];
	snprintf(procFileBuff,256,"/proc/%d/fd/%d",getpid(),fd);

	length = (size_t)snprintf(buffer, buffsize," involves fd:%s->",procFileBuff);
	buffer += length;
	buffsize -= length;
	strlen += (int)length;
                     
	length = (size_t)readlink(procFileBuff,buffer,buffsize);
    }

    ras.setLength(strlen);
    ras.send();
}
