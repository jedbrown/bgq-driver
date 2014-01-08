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

//! \file  SimJobMonitor.cc
//! \brief Methods for bgcios::jobctl::SimJobMonitor class.

// Includes
#include "SimJobMonitor.h"
#include "SimulationManager.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/common/logging.h>
#include <poll.h>
#include <sstream>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");

SimJobMonitor::~SimJobMonitor()
{
   LOG_CIOS_TRACE_MSG("Job " << _simProc->getJobId() << ": destroying SimJobMonitor object");
   _simProc.reset();
   pthread_attr_destroy(&_attributes);
}

void *
SimJobMonitor::run(void)
{
   // Create a socket for communicating with stdiod and jobctld.
   std::ostringstream monitorPath;
   monitorPath << bgcios::SimulationDirectory << _sim->getSimulationId() << "/job." << _simProc->getJobId();
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(monitorPath.str()));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating command channel '" << monitorPath.str() << "': " << bgcios::errorString(e.errcode()));
      return NULL;
   }

   // Build path to stdiod command channel.
   std::ostringstream stdioPath;
   stdioPath << "/tmp/cios" << _sim->getSimulationId() << "/" << bgcios::StdioCommandChannelName << "." << getppid();

   // Create path to jobctld command channel.
   std::ostringstream jobctlPath;
   jobctlPath << "/tmp/cios" << _sim->getSimulationId() << "/" << bgcios::JobctlCommandChannelName << "." << getppid();

   // Create array of poll structures.
   const size_t pollSize = 2;
   struct pollfd pollInfo[pollSize];

   // Initialize poll structures.
   pollInfo[0].fd = _simProc->getStdoutFd();
   pollInfo[0].events = POLLIN;
   pollInfo[1].fd = _simProc->getStderrFd();
   pollInfo[1].events = POLLIN;

   LOG_CIOS_DEBUG_MSG("polling on descriptors " << pollInfo[0].fd << ", " << pollInfo[1].fd);

   int timeout = 1000;

   // Keep polling until all of the descriptors are closed.
   uint32_t closed = 0;
   while (closed < pollSize) {

      // Wait for data on the pipes.
      int ready = poll(pollInfo, pollSize, timeout);

      // There was an error from poll().
      if (ready == -1) {
         int err = errno;
         LOG_ERROR_MSG("Job " << _simProc->getJobId() << ": error polling pipe descriptors, " << bgcios::errorString(err));
         continue;
      }

      // At least one descriptor had an event.
      else if (ready > 0) {

         // There is data available on the stdout pipe.
         if (pollInfo[0].revents & POLLIN) {
            
            bgcios::stdio::WriteStdioMessage msg;
            ssize_t bytes = read(pollInfo[0].fd, msg.data, sizeof(msg.data));
            if (bytes > 0) {
               LOG_CIOS_DEBUG_MSG("Job " << _simProc->getJobId() << ": stdout data is available");
               msg.data[bytes] = 0;

               // Build a WriteStdout message to use for every line of output.
               msg.header.service = bgcios::StdioService;
               msg.header.version = bgcios::stdio::ProtocolVersion;
               msg.header.type = bgcios::stdio::WriteStdout;
               msg.header.sequenceId = 0;
               msg.header.returnCode = bgcios::Success;
               msg.header.errorCode = 0;
               msg.header.jobId = _simProc->getJobId();
               msg.header.length = static_cast<uint32_t>(sizeof(bgcios::MessageHeader) + static_cast<uint32_t>(bytes));
               sendMultiple(stdioPath.str(), &(msg.header));
            }
            else {
               int err = errno;
               LOG_ERROR_MSG("Job " << _simProc->getJobId() << ": error reading from stdout pipe, " << bgcios::errorString(err));
            }
         }

         // The stdout pipe closed.
         if (pollInfo[0].revents & POLLHUP) {
            LOG_CIOS_TRACE_MSG("Job " << _simProc->getJobId() << ": stdout is closed");
            pollInfo[0].fd = -1;
            pollInfo[0].events = 0;
            ++closed;
         }

         // There is data available on the stderr pipe.
         if (pollInfo[1].revents & POLLIN) {

            bgcios::stdio::WriteStdioMessage msg;
            ssize_t bytes = read(pollInfo[1].fd, msg.data, sizeof(msg.data));
            if (bytes > 0) {
               LOG_CIOS_DEBUG_MSG("Job " << _simProc->getJobId() << ": stderr data is available");
               msg.data[bytes] = 0;

               // Construct a WriteStderr message to use for every line of output.
               msg.header.service = bgcios::StdioService;
               msg.header.version = bgcios::stdio::ProtocolVersion;
               msg.header.type = bgcios::stdio::WriteStderr;
               msg.header.sequenceId = 0;
               msg.header.returnCode = bgcios::Success;
               msg.header.errorCode = 0;
               msg.header.jobId = _simProc->getJobId();
               msg.header.length = static_cast<uint32_t>(sizeof(bgcios::MessageHeader) + static_cast<uint32_t>(bytes));
               sendMultiple(stdioPath.str(), &(msg.header));
            }
            else {
               int err = errno;
               LOG_ERROR_MSG("Job " << _simProc->getJobId() << ": failed to read from stderr pipe: " << bgcios::errorString(err));
            }
         }

         // The stderr pipe closed.
         if (pollInfo[1].revents & POLLHUP) {
            LOG_CIOS_TRACE_MSG("Job " << _simProc->getJobId() << ": stderr is closed");
            pollInfo[1].fd = -1;
            pollInfo[1].events = 0;
            ++closed;
         }
      }
   }

   // Send CloseStdio message to stdiod.
   bgcios::stdio::CloseStdioMessage closeStdioMsg;
   bgcios::initHeader(&(closeStdioMsg.header));
   closeStdioMsg.header.service = bgcios::StdioService;
   closeStdioMsg.header.version = bgcios::stdio::ProtocolVersion;
   closeStdioMsg.header.type = bgcios::stdio::CloseStdio;
   closeStdioMsg.header.length = sizeof(bgcios::stdio::CloseStdioMessage);
   closeStdioMsg.header.jobId = _simProc->getJobId();
   int err = sendMessage(stdioPath.str(), &closeStdioMsg.header);
   if (err == 0) {
      LOG_CIOS_DEBUG_MSG("Job " << closeStdioMsg.header.jobId << ": CloseStdio message sent to stdiod");
   }
   else {
      LOG_ERROR_MSG("Job " << closeStdioMsg.header.jobId << ": error sending CloseStdio message: " << bgcios::errorString(err));
   }

   // Receive CloseStdioAck message from stdiod.
   bgcios::stdio::CloseStdioAckMessage closeStdioAckMsg;
   err = recvMessage(&closeStdioAckMsg);
   if (err == 0) {
      LOG_CIOS_DEBUG_MSG("Job " << closeStdioAckMsg.header.jobId << ": CloseStdioAck message received from stdiod");
   }
   else {
      LOG_ERROR_MSG("Job " << _simProc->getJobId() << ": error receiving CloseStdioAck message: " << bgcios::errorString(err));
   }

   // Wait for the simulated process to end.
   err = _simProc->waitFor();
   if (err != 0) {
      LOG_ERROR_MSG("Job " << _simProc->getJobId() << ": error waiting for process " << _simProc->getProcessId() << " to end, " << bgcios::errorString(err));
   }
   if (_simProc->isExited()) {
      LOG_CIOS_DEBUG_MSG("Job " << _simProc->getJobId() << ": process " << _simProc->getProcessId() << " exited with status " << _simProc->getExitStatus());
   }
   if (_simProc->isSignaled()) {
      LOG_CIOS_DEBUG_MSG("Job " << _simProc->getJobId() << ": process " << _simProc->getProcessId() << " ended by signal " << _simProc->getTermSignal());
   }

   if (_sim->getStartRank() == 0) {

      // Build ExitJob message.
      ExitJobMessage exitJobMsg;
      bgcios::initHeader(&(exitJobMsg.header));
      exitJobMsg.header.service = bgcios::JobctlService;
      exitJobMsg.header.version = ProtocolVersion;
      exitJobMsg.header.type = ExitJob;
      exitJobMsg.header.length = sizeof(ExitJobMessage);
      exitJobMsg.header.jobId = _simProc->getJobId();
      exitJobMsg.status = _simProc->getStatus();
      exitJobMsg.endTime = time(NULL);
      
      // Send ExitJob message to jobctld.
      err = sendMessage(jobctlPath.str(), &exitJobMsg.header);
      if (err == 0) {
         LOG_CIOS_DEBUG_MSG("Job " << exitJobMsg.header.jobId << ": ExitJob message sent to jobctld");
      }
      else {
         LOG_ERROR_MSG("Job " << exitJobMsg.header.jobId << ": error sending ExitJob message to jobctld: " << bgcios::errorString(err));
      }
      
      // Receive ExitJobAck message from jobctld.
      ExitJobAckMessage exitJobAckMsg;
      err = recvMessage(&exitJobAckMsg);
      if (err == 0) {
         LOG_CIOS_DEBUG_MSG("Job " << exitJobAckMsg.header.jobId << ": ExitJobAck message received from jobctld");
      }
      else {
         LOG_ERROR_MSG("Job " << _simProc->getJobId() << ": error receiving ExitJobAck message: " << bgcios::errorString(err));
      }
   }

   return NULL;
}

void
SimJobMonitor::sendMultiple(std::string dest, void *buffer)
{
   // Send a separate message for each simulated rank.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)buffer;
   uint32_t startRank = _sim->getStartRank();
   for (uint32_t index = 0; index < _sim->getNumRanks(); ++index) {
      // Set the rank in the message header.
      msghdr->rank = index + startRank;

      // skip this rank if the message is stdout/stderr and we're above the np value
      if (
             _sim->getNp() &&
             msghdr->service == bgcios::StdioService &&
             ( msghdr->type == bgcios::stdio::WriteStdout || msghdr->type == bgcios::stdio::WriteStderr ) &&
             msghdr->rank >= _sim->getNp()   
         )
      {
          continue;
      }

      // Send message.
      int err = sendMessage(dest, msghdr);
      if (err != 0) {
         LOG_ERROR_MSG("error sending message for rank " << index << ": " << bgcios::errorString(err));
      }

      // Receive ack message (we don't really care about the contents).
      bgcios::MessageHeader reply;
      err = recvMessage(&reply);
      if (err != 0) {
         LOG_ERROR_MSG("error receiving message for rank " << index << ": " << bgcios::errorString(err));
      }
   }

   return;
}

int
SimJobMonitor::sendMessage(std::string dest, void *buffer)
{
   // Send the message header.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)buffer;
   try {
      _cmdChannel->sendTo(dest, msghdr, msghdr->length);
   } 
   catch (SocketError& e) {
      LOG_ERROR_MSG("error sending message header to socket: " << e.what());
      return e.errcode();
   }
   catch (SocketClosed& e) {
      LOG_ERROR_MSG("socket closed when sending message header: " << e.what());
      return EPIPE;
   }
   catch (SocketTimeout& e) {
      LOG_ERROR_MSG("timeout sending message header: " << e.what());
      return ETIMEDOUT;
   }
   LOG_CIOS_DEBUG_MSG("sent message with length " << msghdr->length << " on command channel");

   return 0;
}

int
SimJobMonitor::recvMessage(void *buffer)
{
   // Receive the message header.
   std::string source;
   MessageHeader *msghdr = (MessageHeader *)buffer;
   try {
      _cmdChannel->recvAvailableFrom(source, msghdr, bgcios::SmallMessageDataSize);
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error receiving message header from socket: " << e.what());
      return e.errcode();
   }
   catch (SocketClosed& e) {
      LOG_ERROR_MSG("socket closed when receiving message header: " << e.what());
      return EPIPE;
   }
   LOG_CIOS_DEBUG_MSG("received message with length " << msghdr->length << " on command channel");

   return 0;
}

