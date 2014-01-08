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

//! \file  SimJobController.cc
//! \brief Methods for bgcios::jobctl::SimJobController class.

// Includes
#include "SimJobController.h"
#include "SimJobMonitor.h"
#include <ramdisk/include/services/common/logging.h>
#include <stdlib.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <poll.h>

#include <boost/foreach.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


SimJobController::SimJobController(JobctlConfigPtr config) : JobController(config)
{
   // Initialize simulation manager object.
   _sim = SimulationManagerPtr(new SimulationManager(_config->getSimulationId()));

   // Set work directory.
   std::ostringstream wdirPath;
   wdirPath << bgcios::SimulationDirectory << _sim->getSimulationId() << "/";
   _workDirectory = wdirPath.str();

   // Set path to iosd command channel.
   std::ostringstream iosdPath;
   iosdPath << _workDirectory << bgcios::IosctlCommandChannelName << "." << getppid();
   _iosdCmdChannelPath = iosdPath.str();
}

SimJobController::~SimJobController()
{
   _dataListener.reset();
   _dataChannel.reset();
   _sim.reset();
   free(_inboundMessage);
   free(_outboundMessage);
}

int
SimJobController::startup(in_port_t dataChannelPort)
{
   // Build the path to the command channel.
   std::ostringstream cmdChannelPath;
   cmdChannelPath << _workDirectory << bgcios::JobctlCommandChannelName << "." << getppid();

   // Create the command channel socket.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket(cmdChannelPath.str()));
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error creating command channel '" << cmdChannelPath.str() << "': " << e.what());
      return e.errcode();
   }
   LOG_DEBUG_MSG("[" << _cmdChannel->getSd() << "] created command channel at '" << _cmdChannel->getName() << "'");

   // Create listening socket for data channel.
   try {
      _dataListener = bgcios::InetStreamSocketPtr(new bgcios::InetStreamSocket(dataChannelPort, true));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating listening socket for data channel: " << e.what());
      _dataListener.reset();
      return e.errcode();
   }
   LOG_DEBUG_MSG("listening for new data channel connections on fd " << _dataListener->getSd() << " using address " << _dataListener->getName());

   // Get the address assigned by TCP and store port number in a file.
   std::ostringstream portFileName;
   portFileName << _workDirectory << "jobcontrol.port";
   std::ofstream portFile(portFileName.str().c_str(), std::ios_base::trunc);
   portFile << ntohs(_dataListener->getPort()) << std::endl;
   LOG_DEBUG_MSG("stored port number " << ntohs(_dataListener->getPort()) << " in file " << portFileName.str());

   // Prepare socket to listen for connections.
   try {
      _dataListener->listen(1);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error listening for new connections: " << e.what());
      return e.errcode();
   }

   return 0;
}

int
SimJobController::cleanup(void)
{
   LOG_INFO_MSG("running cleanup ...");

   // Remove the port number file.
   std::ostringstream portFileName;
   portFileName << _workDirectory << "jobcontrol.port";
   if (::unlink(portFileName.str().c_str()) != 0) {
      LOG_ERROR_MSG("error removing port number file " << portFileName.str() << ": " << bgcios::errorString(errno));
   }

   // Remove the command channel.
   _cmdChannel.reset();

   return 0;
}

void
SimJobController::eventMonitor(void)
{
   const int cmdChannel   = 0;
   const int dataChannel  = 1;
   const int dataListener = 2;
   const int numFds       = 3;

   pollfd pollInfo[numFds];
   int timeout = -1;

   // Initialize the pollfd structure.
   pollInfo[cmdChannel].fd = _cmdChannel->getSd();
   pollInfo[cmdChannel].events = POLLIN;
   pollInfo[cmdChannel].revents = 0;
   LOG_TRACE_MSG("added command channel using fd " << pollInfo[cmdChannel].fd << " to descriptor list");

   pollInfo[dataChannel].fd = _dataChannel == NULL ? -1 : _dataChannel->getSd();
   pollInfo[dataChannel].events = POLLIN;
   pollInfo[dataChannel].revents = 0;
   LOG_TRACE_MSG("added data channel using fd " << pollInfo[dataChannel].fd << " to descriptor list");

   pollInfo[dataListener].fd = _dataListener->getSd();
   pollInfo[dataListener].events = POLLIN;
   pollInfo[dataListener].revents = 0;
   LOG_TRACE_MSG("added data channel listener using fd " << pollInfo[dataListener].fd << " to descriptor list");

   // Process events until told to stop.
   while (!_done) {
      // Wait for an event on one of the descriptors.
      LOG_DEBUG_MSG("polling for events with timeout " << timeout << "...")
      const int rc = poll(pollInfo, numFds, timeout);
      LOG_TRACE_MSG("poll rc=" << rc);

      // There was no data so try again.
      if (rc == 0) {
         continue;
      } 

      // There was an error so log the failure and try again.
      if (rc == -1) {
         int err = errno;
         if (err == EINTR) {
            LOG_TRACE_MSG("poll returned EINTR, continuing ...");
            continue;
         }

         LOG_ERROR_MSG("poll failed, error " << err);
         return;
      } 

      // Check for an event on the command channel.
      if (pollInfo[cmdChannel].revents & POLLIN) {
         LOG_TRACE_MSG("input event available on command channel");
         commandChannelHandler();
      }

      // Check for an event on the data channel.
      if (pollInfo[dataChannel].revents & POLLIN) {
         LOG_TRACE_MSG("input event available on data channel");
         if (dataChannelHandler() == EPIPE) {
             pollInfo[dataChannel].fd = -1;
         }
      }

      // Check for an event on the data channel listener.
      if (pollInfo[dataListener].revents & POLLIN) {
         LOG_TRACE_MSG("input event available on data channel listener");
         pollInfo[dataListener].revents = 0;

         // Make a new data channel connected to runjob.
         const InetSocketPtr incoming = makeDataChannel();
         if (!incoming) {
            LOG_ERROR_MSG("error making new data channel");
            continue;
         }

         // Continue monitoring the data channel listener for restarted connection Start monitoring the data channel.
         LOG_INFO_MSG("data channel is connected to " << incoming->getPeerName() << " using fd " << incoming->getSd());

         if (!dataChannelHandler(incoming)) {
             _dataChannel = incoming;
             pollInfo[dataChannel].fd = _dataChannel->getSd();
             LOG_INFO_MSG("data channel is authenticated with " << _dataChannel->getPeerName() << " using fd " << _dataChannel->getSd());
         }
      }
   }

   // Reset for next time.
   _done = false;

   return;
}

int
SimJobController::dataChannelHandler(InetSocketPtr authOnly)
{
   InetSocketPtr& dataChannel( authOnly ? authOnly : _dataChannel );

   // Receive a message from the data channel.
   int err = recvFromDataChannel(_inboundMessage, dataChannel);

   // When data channel closes, stop handling events.
   if (err == EPIPE) {
      LOG_DEBUG_MSG("data channel connected to " << dataChannel->getPeerName() << " is closed");
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

   // Make the sure protocol version is a match.
   if (msghdr->version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message protocol version " << (int)msghdr->version << " does not match jobctld version " << (int)ProtocolVersion);
      sendErrorAckToDataChannel(bgcios::VersionMismatch, ProtocolVersion);
      return 1;
   }

   // Make sure the Authenticate message is arriving when expected.
   if (authOnly && msghdr->type != Authenticate) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": expected Authenticate message: " << bgcios::printHeader(*msghdr) );
      return 1;
   } else if (!authOnly && msghdr->type == Authenticate) {
      LOG_TRACE_MSG("Job " << msghdr->jobId << ": unexpected Authenticate message");
      dataChannel.reset();
      return 1;
   }

   // Handle the message.
   LOG_DEBUG_MSG("Job " << msghdr->jobId << ": " << toString(msghdr->type) << " message is available on data channel");
   switch (msghdr->type) {
      case SetupJob:
         err = setupJob();
         break;

      case LoadJob: 
         err = loadJob();
         break;

      case StartJob:
         err = startJob();
         break;

      case SignalJob:
         err = signalJob();
         break;

      case CleanupJob:
         err = cleanupJob();
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

      case ChangeConfig:
         err = changeConfig();
         break;

      case Authenticate:
         err = authenticate(authOnly);
         break;

      case Reconnect:
         err = reconnect();
         break;

      case Heartbeat:
         err = heartbeat();
         break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported");
         msghdr->returnCode = bgcios::UnsupportedType;
         msghdr->length = sizeof(bgcios::MessageHeader);
         err = sendToDataChannel(msghdr);
         break;
   }

   if (err != 0) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": error sending ack message: " << bgcios::errorString(err));
   }

   return err;
}

int
SimJobController::commandChannelHandler(void)
{
   // Receive a message from the command channel.
   std::string peer;
   int err = recvFromCommandChannel(_inboundMessage);

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
   if ((msghdr->service != bgcios::JobctlService) && (msghdr->service != bgcios::IosctlService)) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message service " << (int)msghdr->service << " is wrong, header: " << bgcios::printHeader(*msghdr));
      sendErrorAckToCommandChannel(peer, bgcios::WrongService, 0);
      return 0;
   }

   // Make the sure protocol version is a match.
   if (msghdr->version != ProtocolVersion) {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": message protocol version " << (int)msghdr->version << " does not match jobctld version " << (int)ProtocolVersion);
      sendErrorAckToCommandChannel(peer, bgcios::VersionMismatch, ProtocolVersion);
      return 0;
   }

   // Handle the message.
   LOG_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message is available on command channel");
   switch (msghdr->type) {
      case ExitJob:
         err = exitJob();
         break;

      case bgcios::iosctl::Terminate:
         err = terminate();
         break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
         err = sendErrorAckToCommandChannel(peer, bgcios::UnsupportedType, 0);
         break;
   }

   if (err == 0) {
      LOG_DEBUG_MSG("Job " << msghdr->jobId << ": ack message sent successfully");
   }
   else {
      LOG_ERROR_MSG("Job " << msghdr->jobId << ": error sending ack message: " << bgcios::errorString(err));
   }

   return 0;
}

int
SimJobController::setupJob(void)
{
   // Get pointer to inbound SetupJob message.
   SetupJobMessage *inMsg = (SetupJobMessage *)_inboundMessage;
   LOG_DEBUG_MSG( 
           "corner (" <<
           static_cast<unsigned>(inMsg->corner.aCoord) << "," <<
           static_cast<unsigned>(inMsg->corner.bCoord) << "," <<
           static_cast<unsigned>(inMsg->corner.cCoord) << "," <<
           static_cast<unsigned>(inMsg->corner.dCoord) << "," <<
           static_cast<unsigned>(inMsg->corner.eCoord) << ")"
           );

   LOG_DEBUG_MSG( 
           "shape " <<
           static_cast<unsigned>(inMsg->shape.aCoord) << "x" <<
           static_cast<unsigned>(inMsg->shape.bCoord) << "x" <<
           static_cast<unsigned>(inMsg->shape.cCoord) << "x" <<
           static_cast<unsigned>(inMsg->shape.dCoord) << "x" <<
           static_cast<unsigned>(inMsg->shape.eCoord)
           );
   
   // Build SetupJobAck message in outbound buffer.
   SetupJobAckMessage *outMsg = (SetupJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = SetupJobAck;
   outMsg->header.length = sizeof(SetupJobAckMessage);
   outMsg->header.returnCode = Success;

   // Construct Job object to track the job and add it to the list.
   const JobPtr job(new Job(inMsg->header.jobId, 0));
   _sim->setNp( inMsg->numRanks );
   _jobs.add(inMsg->header.jobId, job);
   LOG_INFO_MSG("Job " << inMsg->header.jobId << " added with " << job->numComputeNodes() << " compute nodes" );

   // Initialize accumulators for the job, they are always 1 for simulation
   job->setupJobAckAccumulator.setLimit(1);
   job->loadJobAckAccumulator.setLimit(1);
   job->startJobAckAccumulator.setLimit(1);
   job->cleanupJobAckAccumulator.setLimit(1);
   job->exitProcessAccumulator.setLimit(1);
   job->exitJobAccumulator.setLimit(1);

   // Send SetupJobAck message.
   return sendToDataChannel(outMsg);
}

int
SimJobController::loadJob(void)
{
   // Get pointer to inbound LoadJob message.
   LoadJobMessage *inMsg = (LoadJobMessage *)_inboundMessage;

   // Build LoadJobAck message in outbound buffer.
   LoadJobAckMessage *outMsg = (LoadJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = LoadJobAck;
   outMsg->header.length = sizeof(LoadJobAckMessage);
   outMsg->header.returnCode = Success;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send LoadJobAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToDataChannel(outMsg);
   }

   // Start the simulated process.
   LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": starting process to run program '" << inMsg->arguments << "'");
   const SimProcessPtr simProc = SimProcessPtr(new SimProcess(inMsg->arguments, inMsg->workingDirectoryPath, inMsg->umask, inMsg->header.jobId));
   char *ap = inMsg->arguments;
   for (uint16_t index = 0; index < inMsg->numArguments; ++index) {
      simProc->addArgument(ap);
      while (*ap++ != 0); // Find start of next argument.
   }
   char *vp = inMsg->variables;
   for (uint16_t index = 0; index < inMsg->numVariables; ++index) {
      simProc->addEnvironVariable(vp);
      while (*vp++ != 0); // Find start of next variable.
   }
   bgcios::MessageResult result = simProc->start();
   if (result.isSuccess()) {
      LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": started process " << simProc->getProcessId() << " to simulate " << _sim->getNumRanks() << " ranks");
      SimJobMonitorPtr monitor = SimJobMonitorPtr(new SimJobMonitor(_sim, simProc)); // Note the simProc object is now owned by the monitor.
      job->setSimJobMonitor(monitor);
   }
   else {
      LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error starting process, " << bgcios::errorString(result.errorCode()));
      result.setHeader(outMsg->header);
   }

   // Send LoadJobAck message.
   return sendToDataChannel(outMsg);
}

int
SimJobController::startJob(void)
{
   // Get pointer to inbound StartJob message.
   StartJobMessage *inMsg = (StartJobMessage *)_inboundMessage;

   LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": starting with " << inMsg->numRanksForIONode << " ranks for IO node");

   // Build a StartJobAck message in outbound buffer.
   StartJobAckMessage *outMsg = (StartJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = StartJobAck;
   outMsg->header.length = sizeof(StartJobAckMessage);
   outMsg->header.returnCode = Success;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send StartJobAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToDataChannel(outMsg);
   }

   struct timeval now;
   gettimeofday(&now, NULL);
   const uint64_t currentTime = ((uint64_t)now.tv_sec * bgcios::MicrosecondsPerSecond) + (uint64_t)now.tv_usec;
   if ( !inMsg->currentTime ) {
       // Get the current time for the compute nodes.
       inMsg->currentTime = currentTime;
   } else {
       const uint64_t difference = currentTime > inMsg->currentTime ? currentTime - inMsg->currentTime : inMsg->currentTime - currentTime;
       const uint64_t threshold = _config->getStartTimeThreshold();
       if ( threshold && difference > threshold ) {
           LOG_INFO_MSG_FORCED("Job " << inMsg->header.jobId << " " << difference / 1000.0l << "ms difference" );
       } else {
           LOG_DEBUG_MSG("Job " << inMsg->header.jobId << " " << difference << "ms difference" );
       }
   }

   // Set number of ranks for the job.
   _sim->setNumRanks(inMsg->numRanksForIONode);
   _sim->setStartRank(inMsg->header.rank);

   // Start a thread to handle stdout and stderr from the simulated process.
   SimJobMonitorPtr monitor = job->getSimJobMonitor();
   if (monitor != NULL) {
      int err = monitor->start();
      if (err != 0) {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error starting job, " << bgcios::errorString(err));
         outMsg->header.returnCode = RequestFailed;
         outMsg->header.errorCode = (uint32_t)err;
      }
   }

   // Send StartJobAck message.
   return sendToDataChannel(outMsg);
}

int
SimJobController::signalJob(void)
{
   // Get pointer to inbound SignalJob message.
   SignalJobMessage *inMsg = (SignalJobMessage *)_inboundMessage;

   LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": sending signal " << inMsg->signo);

   // Build SignalJobAck message in outbound buffer.
   SignalJobAckMessage *outMsg = (SignalJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = SignalJobAck;
   outMsg->header.returnCode = Success;
   outMsg->header.length = sizeof(SignalJobAckMessage);

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send SignalJobAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToDataChannel(outMsg);
   }

   // Send a signal to the simulated process.
   SimJobMonitorPtr monitor = job->getSimJobMonitor();
   if (monitor != NULL) {
      int err = monitor->signalJob(inMsg->signo);
      if (err != 0) {
         LOG_ERROR_MSG("Job " << inMsg->header.jobId << ": error sending signal " << inMsg->signo << ", " << bgcios::errorString(err));
         outMsg->header.returnCode = RequestFailed;
         outMsg->header.errorCode = (uint32_t)err;
      }
   }
   
   // End all of the tools if the signal is SIGKILL and the job must end.
   if (inMsg->signo == SIGKILL) {
      job->endAllTools(SIGTERM); // Give the tools a chance to cleanup and end.
   }

   // Send SignalJobAck message.
   return sendToDataChannel(outMsg);
}

int
SimJobController::cleanupJob(void)
{
   // Get pointer to CleanupJob message available from inbound buffer.
   CleanupJobMessage *inMsg = (CleanupJobMessage *)_inboundMessage;

   // Build CleanupJobAck message in outbound buffer.
   CleanupJobAckMessage *outMsg = (CleanupJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = CleanupJobAck;
   outMsg->header.length = sizeof(CleanupJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;
   outMsg->epilogReturnCode = bgcios::Success;
   outMsg->epilogErrorCode = 0;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send CleanupJobAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToDataChannel(outMsg);
   }

   // Cleanup the monitor thread for the simulated job.
   SimJobMonitorPtr monitor = job->getSimJobMonitor();
   if (monitor != NULL) {
      // When the monitor thread is started, wait for it to end.
      if (monitor->isStarted()) {
         monitor->join();
      }

      // Otherwise, make sure the simulated process is ended.
      else {
         monitor->signalJob(SIGKILL);
      }
   }
   
   job->cleanup();

   // Remove the job from the map and destroy the Job object.
   _jobs.remove(job->getJobId());
   job.reset();

   if (inMsg->killTimeout) {
       // no ack is required
       return 0;
   }

   return sendToDataChannel(outMsg);
}

int
SimJobController::exitJob(void)
{
   // Get pointer to ExitJob message available from inbound buffer.
   ExitJobMessage *inMsg = (ExitJobMessage *)_inboundMessage;

   LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": exited with status " << inMsg->status);

   // Build ExitJobAck message in outbound buffer.
   ExitJobAckMessage *outMsg = (ExitJobAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ExitJobAck;
   outMsg->header.length = sizeof(ExitJobAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Build the path to monitor command channel.
   std::ostringstream monitorPath;
   monitorPath << bgcios::SimulationDirectory << _sim->getSimulationId() << "/job." << inMsg->header.jobId;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send ExitJobAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToCommandChannel(monitorPath.str(), outMsg);
   }

   // Forward the message to the data channel.
   int err = sendToDataChannel(inMsg);
   if (err != 0) {
      outMsg->header.returnCode = bgcios::SendError;
      outMsg->header.errorCode = (uint32_t)err;
      job->exitJobAccumulator.add(inMsg);
   }

   // Send ExitJobAck message.
   return sendToCommandChannel(monitorPath.str(), outMsg);
}

int
SimJobController::terminate(void)
{
   // Get pointer to Terminate message available from inbound buffer.
   bgcios::iosctl::TerminateMessage *inMsg = (bgcios::iosctl::TerminateMessage *)_inboundMessage;

   // Set flags to stop processing messages.
   _done = true;
   _terminated = true;

   // Build TerminateAck message in outbound buffer.
   bgcios::iosctl::TerminateAckMessage *outMsg = (bgcios::iosctl::TerminateAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = bgcios::iosctl::TerminateAck;
   outMsg->header.length = sizeof(bgcios::iosctl::TerminateAckMessage);
   outMsg->header.returnCode = bgcios::Success;

   // Send TerminateAck message.
   return sendToCommandChannel(_iosdCmdChannelPath, outMsg);
}

int
SimJobController::startTool(void)
{
    // Get pointer to inbound StartTool message.
    StartToolMessage *inMsg = (StartToolMessage *)_inboundMessage;

   // Build StartToolAck message in outbound buffer.
   StartToolAckMessage *outMsg = (StartToolAckMessage *)_outboundMessage;
   outMsg->toolId = inMsg->toolId;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = StartToolAck;
   outMsg->header.length = sizeof(StartToolAckMessage);
   outMsg->header.returnCode = Success;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send StartToolAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToDataChannel(outMsg);
   }

   // Start the tool and add it to the job.
   bgcios::MessageResult result = job->startTool(inMsg, true);
   if (result.isError()) {
      result.setHeader(outMsg->header);
      return sendToDataChannel(outMsg);
   }
   
   // Send StartToolAck message.
   return sendToDataChannel(outMsg);
}

int
SimJobController::endTool(void)
{
    // Get pointer to inbound EndTool message.
    EndToolMessage *inMsg = (EndToolMessage *)_inboundMessage;

   // Build EndToolAck message in outbound buffer.
   EndToolAckMessage *outMsg = (EndToolAckMessage *)_outboundMessage;
   outMsg->toolId = inMsg->toolId;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = EndToolAck;
   outMsg->header.length = sizeof(EndToolAckMessage);
   outMsg->header.returnCode = Success;

   // Validate the job id.
   JobPtr job = _jobs.get(inMsg->header.jobId);
   if (job == NULL) {
      // Send EndToolAck message to report error.
      outMsg->header.returnCode = bgcios::JobIdError;
      outMsg->header.errorCode = ENOENT;
      LOG_ERROR_MSG("Job " << outMsg->header.jobId << ": job was not found");
      return sendToDataChannel(outMsg);
   }

   // End the tool with the specified signal.
   bgcios::MessageResult result = job->endTool(inMsg->toolId, inMsg->signo);
   if (result.isError()) {
      result.setHeader(outMsg->header);
      return sendToDataChannel(outMsg);
   }

   // Send EndToolAck message.
   return sendToDataChannel(outMsg);
}

int
SimJobController::checkToolStatus(void)
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

   // There are never any tools active.
   outMsg->activeTools = 0;

   // Send CheckToolStatusAck message on data channel.
   LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": CheckToolStatusAck message sent on data channel");
   return sendToDataChannel(outMsg); 
}

int
SimJobController::changeConfig(void)
{
   // Get pointer to inbound ChangeConfig message.
   ChangeConfigMessage *inMsg = (ChangeConfigMessage *)_inboundMessage;

   // Build ChangeConfigAck message in outbound buffer.
   ChangeConfigAckMessage *outMsg = (ChangeConfigAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = ChangeConfigAck;
   outMsg->header.length = sizeof(ChangeConfigAckMessage);
   outMsg->header.returnCode = Success;

   // update logging levels
   setLoggingLevel( "ibm.cios", inMsg->commonTraceLevel );
   setLoggingLevel( "ibm.cios.jobctl", inMsg->jobctldTraceLevel );

   return sendToDataChannel(outMsg);
}

int
SimJobController::authenticate(InetSocketPtr channel)
{
   // Get pointer to inbound Authenticate message.
   AuthenticateMessage *inMsg = (AuthenticateMessage *)_inboundMessage;

   std::ostringstream os;
   BOOST_FOREACH( const unsigned char i, inMsg->plainData ) {
       os << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(i);
   }
   LOG_TRACE_MSG( "plain: " << os.str() );
   os.str("");
   BOOST_FOREACH( const unsigned char i, inMsg->encryptedData ) {
       os << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(i);
   }
   LOG_TRACE_MSG( "encrypted: " << os.str() );

   // special case to simulate authentication failures, plain and encrypted will be the same
   if ( memcmp(inMsg->plainData, inMsg->encryptedData, sizeof(inMsg->plainData)) == 0 ) {
       LOG_ERROR_MSG( "authentication failed" );
      channel.reset();
      _done = 1;
       return EPERM;
   }

   // Build AuthenticateAck message in outbound buffer.
   AuthenticateAckMessage *outMsg = (AuthenticateAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(MessageHeader));
   outMsg->header.type = AuthenticateAck;
   outMsg->header.length = sizeof(AuthenticateAckMessage);
   outMsg->header.returnCode = Success;

   return sendToDataChannel(outMsg, channel);
}

int
SimJobController::reconnect(void)
{
   // Get pointer to inbound Reconnect message.
   ReconnectMessage *inMsg = (ReconnectMessage *)_inboundMessage;

   // Build ReconnectAck message in outbound buffer.
   ReconnectAckMessage *outMsg = (ReconnectAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), &(inMsg->header), sizeof(bgcios::MessageHeader));
   outMsg->header.type = ReconnectAck;
   outMsg->header.length = sizeof(ReconnectAckMessage);
   outMsg->header.returnCode = Success;

   // Run the list of jobs and resend any accumulated messages that are ready.
   for (job_list_iterator iter = _jobs.begin(); iter != _jobs.end(); ++iter) {
      JobPtr job = iter->second;
      int err = 0;

      if (job->exitJobAccumulator.atLimit()) {
         err = sendToDataChannel(const_cast<ExitJobMessage*>(job->exitJobAccumulator.get()));
         LOG_DEBUG_MSG("Job " << job->getJobId() << ": ExitJob message sent on data channel when handling Reconnect message");
         if (err == 0) {
            job->exitJobAccumulator.resetCount();
         }
      }
   }

   // Send ReconnectAck message on data channel.
   LOG_DEBUG_MSG("Job " << inMsg->header.jobId << ": Reconnect message sent on data channel");
   return sendToDataChannel(outMsg); 
}

