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

//! \file  GdbController.cc
//! \brief Methods for bgcios::gdbtool::GdbController class.

// Includes
#include "GdbController.h"
#include <ramdisk/include/services/ToolctlMessages.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sstream>
#include <iomanip>

//#define BREAKPOINT_INSTRUCTION 0x7d821008

using namespace bgcios::gdbtool;
using namespace bgcios::toolctl;

LOG_DECLARE_FILE("cios.gdbtool");

const std::string GdbtoolCommandChannelName = "/gdbtool";

// Supported gdb remote serial protocol commands.
const char GdbNoCommand              = ' ';
const char GdbInterrupt              = 0x03;
const char GdbGetHaltReason          = '?';
const char GdbContinue               = 'c';
const char GdbContinueWithSignal     = 'C';
const char GdbReadAllRegisters       = 'g';
const char GdbWriteAllRegisters      = 'G';
const char GdbSetThread              = 'H';
const char GdbKillProcess            = 'k';
const char GdbReadMemory             = 'm';
const char GdbWriteMemory            = 'M';
const char GdbGeneralQuery           = 'q';
const char GdbGeneralSet             = 'Q';
const char GdbWriteOneRegister       = 'P';
const char GdbStep                   = 's';
const char GdbStepWithSignal         = 'S';
const char GdbThreadAlive            = 'T';
const char GdbWriteMemoryBinary      = 'X';
const char GdbRemoveBreakpoint       = 'z';
const char GdbInsertBreakpoint       = 'Z';

// Register numbers for getting and setting individual registers.
const uint32_t GdbRegisterGPR00 =  0;
const uint32_t GdbRegisterGPR31 = 31;
const uint32_t GdbRegisterFPR00 = 32;
const uint32_t GdbRegisterFPR31 = 63;
const uint32_t GdbRegisterIAR   = 64;
const uint32_t GdbRegisterMSR   = 65;
const uint32_t GdbRegisterCR    = 66;
const uint32_t GdbRegisterLR    = 67;
const uint32_t GdbRegisterCTR   = 68;
const uint32_t GdbRegisterXER   = 69;
const uint32_t GdbRegisterFPSCR = 70;
const uint32_t GdbRegisterEnd   = 71;

GdbController::GdbController() : bgcios::ServiceController()
{
   // Set work directory.
   _workDirectory = bgcios::WorkDirectory;

   // Set path to iosd command channel.
   std::ostringstream iosdPath;
   iosdPath << _workDirectory << bgcios::IosctlCommandChannelName;
   _iosdCmdChannelPath = iosdPath.str();

   // Initialize private data.
   _toolId = 0;
   _rank = 0;
   _jobId = 0;
   _sequenceId = 1;
   _attached = false;
   _existingProcess = false;
   _processEnded = false;
   _processKilled = false;
   _closeAfterAttached = false;
   _retryInProgress = false;
   _currentGdbCommand = GdbNoCommand;
   _currentQuery = GdbQueryNone;
   _threadIdForGeneralOps = 0;
   _threadIdForContinueOps = 0;
   _notifyPending = 0;
}

GdbController::~GdbController()
{
   free(_inboundMessage);
   free(_outboundMessage);
}

int
GdbController::startup(in_addr_t gdbChannelAddr, in_port_t gdbChannelPort, uint32_t rank)
{
   _rank = rank;
   if ( !getenv("BG_JOBID") ) {
       LOG_FATAL_MSG("could not find BG_JOBID");
       exit(EXIT_FAILURE);
   } else if ( !getenv("BG_TOOLID") ) {
       LOG_FATAL_MSG("could not find BG_TOOLID");
       exit(EXIT_FAILURE);
   }

   _jobId = boost::lexical_cast<uint64_t>(getenv("BG_JOBID"));
   _toolId = boost::lexical_cast<uint32_t>(getenv("BG_TOOLID"));

   std::ostringstream tag;
   tag << "Job " << _jobId << "." << _rank << ": ";
   _tag = tag.str();
   LOG_DEBUG_MSG(_tag << "assigned tool id " << _toolId);

   // Create the tool data channel socket and connect to toolctld.
   try {
      std::ostringstream toolctldPath;
      toolctldPath << bgcios::JobsDirectory << _jobId << bgcios::ToolctlRankDirectory << _rank;
      _toolChannel = LocalStreamSocketPtr(new LocalStreamSocket());
      _toolChannel->connect(toolctldPath.str());
   }
   catch (bgcios::SocketError& e) {
      if (e.errcode() == ENOENT) {
         LOG_CIOS_INFO_MSG("rank " << _rank << " is not running on this I/O node, exited normally");
         exit(EXIT_SUCCESS);
      }
      LOG_ERROR_MSG("error creating tool data channel socket: " << e.what());
      return e.errcode();
   }
   LOG_DEBUG_MSG(_tag << "connected to toolctld at '" << _toolChannel->getPeerName() << "'");

   // Create listening socket for gdb channel.
   try {
      _gdbListener = InetStreamSocketPtr(new InetStreamSocket(gdbChannelAddr, gdbChannelPort, true));
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating listening socket for gdb channel: " << e.what());
      _dataListener.reset();
      return e.errcode();
   }

   // Prepare socket to listen for connections.
   try {
      _gdbListener->listen(1);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error listening for new gdb client connections: " << e.what());
      return e.errcode();
   }
   LOG_DEBUG_MSG(_tag << "listening for new gdb client connections on " << _gdbListener->getName());

   return 0;
}

int
GdbController::cleanup(void)
{
   // Close the listening socket for gdb channel.
   _gdbListener.reset();
   LOG_DEBUG_MSG(_tag << "closed listener for new gdb client connections");

   // Close tool data channel socket.
   _toolChannel.reset();
   LOG_DEBUG_MSG(_tag << "closed tool data channel socket to the daemon");

   // Close the channel to the tool
   _gdbChannel.reset();
   LOG_DEBUG_MSG(_tag << "closed tool channel to the gdb client");

   return 0;
}

void
GdbController::eventMonitor(bgcios::SigtermHandler& sigtermHandler)
{
   // Initialize the pollfd structure.
   _pollInfo[CmdChannel].fd = _cmdChannel == NULL ? -1 : _cmdChannel->getSd();
   _pollInfo[CmdChannel].events = POLLIN;
   _pollInfo[CmdChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added command channel using fd " << _pollInfo[CmdChannel].fd << " to descriptor list");

   _pollInfo[ToolChannel].fd = _toolChannel->getSd();
   _pollInfo[ToolChannel].events = POLLIN;
   _pollInfo[ToolChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added tool data channel using fd " << _pollInfo[ToolChannel].fd << " to descriptor list");

   _pollInfo[GdbChannel].fd = _gdbChannel == NULL ? -1 : _gdbChannel->getSd();
   _pollInfo[GdbChannel].events = POLLIN;
   _pollInfo[GdbChannel].revents = 0;
   LOG_CIOS_TRACE_MSG("added gdb channel using fd " << _pollInfo[GdbChannel].fd << " to descriptor list");

   _pollInfo[GdbListener].fd = _gdbListener->getSd();
   _pollInfo[GdbListener].events = POLLIN;
   _pollInfo[GdbListener].revents = 0;
   LOG_CIOS_TRACE_MSG("added gdb channel listener using fd " << _pollInfo[GdbListener].fd << " to descriptor list");

   // Process events until told to stop.
   int timeout = -1;
   while (!_done) {

      // When a SIGTERM signal is delivered and the process is still running, assume this is a stop_tool request.
      if ((!_processKilled) && (!_processEnded) && (sigtermHandler.isCaught())) {
          if (_attached)
          {
             term();
          }
          else
          { 
              _done = 1;
              continue;
          }
      }

      // Wait for an event on one of the descriptors.
      int rc = poll(_pollInfo, MaxPollSetSize, timeout);

      // There was no data so try again.
      if (rc == 0) {
         continue;
      } 

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

      // Check for an event on the command channel.
      if (_pollInfo[CmdChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on command channel");
         _pollInfo[CmdChannel].revents = 0;
         commandChannelHandler();
      }

      // Check for an event on the tool data channel.
      if (_pollInfo[ToolChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on tool data channel");
         _pollInfo[ToolChannel].revents = 0;
         toolChannelHandler();
      }

      // Check for an event on the gdb channel.
      if (_pollInfo[GdbChannel].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on gdb channel");
         _pollInfo[GdbChannel].revents = 0;
         gdbChannelHandler();
      }

      // Check for an event on the gdb channel listener.
      if (_pollInfo[GdbListener].revents & POLLIN) {
         LOG_CIOS_TRACE_MSG("input event available on gdb channel listener");
         _pollInfo[GdbListener].revents = 0;

         // Make a new gdb channel connected to a gdb client.
         int err = makeGdbChannel();
         if (err != 0) {
            LOG_ERROR_MSG("error making new gdb channel: " << bgcios::errorString(err));
            continue;
         }

         // Stop monitoring the gdb channel listener and start monitoring the gdb channel.
         _gdbListener->close();
         _pollInfo[GdbListener].fd = -1;
         _pollInfo[GdbChannel].fd = _gdbChannel->getSd();
      }

   }

   // Reset for next time.
   // This is where we bail.
   _done = false;

   return;
}

int
GdbController::commandChannelHandler(void)
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
   if ((msghdr->service != bgcios::ToolctlService) && (msghdr->service != bgcios::IosctlService)) {
      LOG_ERROR_MSG(_tag << "message service " << (int)msghdr->service << " is wrong, header: " << bgcios::printHeader(*msghdr));
      sendErrorAckToCommandChannel(peer, bgcios::WrongService, 0);
      return 0;
   }

   // Handle the message.
   LOG_DEBUG_MSG(_tag << bgcios::toString(msghdr) << " message is available on command channel");
   switch (msghdr->type) {

      case bgcios::iosctl::Terminate:
         err = terminate();
         break;

      default:
         LOG_ERROR_MSG(_tag << "message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
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
GdbController::toolChannelHandler(void)
{
   // Receive a message from the tool data channel.
   int err = recvFromToolChannel(_inboundMessage);

   // An error occurred receiving a message.
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error receiving message from tool data channel: " << bgcios::errorString(err));
      if (err == EPIPE) { // When tool data channel closes, stop handling events.
         _done = true;
      }
      return err;
   }

   // Make sure the service field is correct.
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_inboundMessage;
   if (msghdr->service != bgcios::ToolctlService) {
      LOG_ERROR_MSG(_tag << "message service " << (int)msghdr->service << " is wrong, header: " << bgcios::printHeader(*msghdr));
//    sendErrorAckToCommandChannel(peer, bgcios::WrongService, 0);
      return 0;
   }

   // Handle the message.
   LOG_DEBUG_MSG(_tag << bgcios::toString(msghdr) << " message is available on tool data channel");
   switch (msghdr->type) {

      case AttachAck:  attachAck();  break;
      case DetachAck:  detachAck();  break;
      case QueryAck:   queryAck();   break;
      case UpdateAck:  updateAck();  break;
      case Notify:     notify();     break;
      case ControlAck: controlAck(); break;

      default:
         LOG_ERROR_MSG("Job " << msghdr->jobId << ": message type " << msghdr->type << " is not supported, header: " << bgcios::printHeader(*msghdr));
//       err = sendErrorAckToCommandChannel(peer, bgcios::UnsupportedType, 0);
         break;
   }

   return 0;
}

int
GdbController::gdbChannelHandler(void)
{
   try {
      do {
         // Get input from the gdb client.
         std::string command = _gdbChannel->get();

         if (command.at(0) == '+') {
            LOG_DEBUG_MSG(_tag << "last command sent was acknowledged by gdb client");
            _gdbChannel->resetLastCommand();
         }

         else if (command.at(0) == '-') {
            LOG_DEBUG_MSG(_tag << "gdb client requested retransmit of last command");
            _gdbChannel->putLastCommand();
         }

         else if (command.at(0) == GdbInterrupt) {
            LOG_DEBUG_MSG(_tag << "gdb client requested interrupt");
            interrupt();
         }

         else if (command.at(0) == '$') {
            // Acknowledge command was received correctly.
            _gdbChannel->putAck();

            // Remove the '$' character.
            command.erase(0, 1);

            // The first character identifies the command.
            std::string response;
            switch (command.at(0)) {
               case GdbGetHaltReason:
                  if (!_attached) {
                     attach();
                  }
                  break;

               case GdbContinue:
               case GdbContinueWithSignal:
                  cont(command, response);
                  break;

               case GdbReadAllRegisters:
                  readAllRegisters(command, response);
                  break;

               case GdbWriteAllRegisters:
                  writeAllRegisters(command, response);
                  break;

               case GdbSetThread:
                  setThread(command, response);
                  break;

               case GdbKillProcess:
                  killProcess(command, response);
                  _processKilled = true;
                  break;

               case GdbReadMemory:
                  readMemory(command, response);
                  break;

               case GdbWriteMemory:
               case GdbWriteMemoryBinary:
                  writeMemory(command, response);
                  break;

               case GdbGeneralQuery:
                  generalQuery(command, response);
                  break;

               case GdbGeneralSet:
                  generalSet(command, response);
                  break;

               case GdbWriteOneRegister:
                  writeOneRegister(command, response);
                  break;

               case GdbStep:
               case GdbStepWithSignal:
                  step(command, response);
                  break;

               case GdbThreadAlive:
                  threadAlive(command, response);
                  break;

               case GdbRemoveBreakpoint:
                  removeBreakpoint(command, response);
                  break;

               case GdbInsertBreakpoint:
                  insertBreakpoint(command, response);
                  break;

               default:
                  LOG_DEBUG_MSG(_tag << "gdb client command '" << command << "' is unsupported");
                  response.append(UnsupportedCommand); // Command is unsupported.
                  break;
            }

            // Send the response to the gdb client if needed.
            if (!response.empty()) {
               if (response[0] == '$') {
                  _gdbChannel->putCommand(response);
               }
               else {
                  _gdbChannel->makeCommand(response);
                  _gdbChannel->putCommand(response);
               }
            }
         }

         else if (command.at(0) == '%') {
            LOG_CIOS_WARN_MSG(_tag << "got notification packet from gdb client which is ignored");
         }

         else {
            LOG_FATAL_MSG(_tag << "packet is unknown: '" << command << "'");
         }

      } while (_gdbChannel->isInputAvailable());
   }

   catch (GdbCommandError& e) {
      // Send a NAK.
   }

   catch (bgcios::SocketError& e) {
   }

   catch (bgcios::SocketClosed& e) {
      LOG_DEBUG_MSG(_tag << "gdb client closed connection");

      // Keep handling messages if the gdb client has killed the process and the process has not ended.
      if (_processKilled && !_processEnded) {
         LOG_DEBUG_MSG("waiting for notification that process has ended");
         _pollInfo[GdbChannel].fd = -1;
         _pollInfo[GdbChannel].events = 0;
      }
      else {
         _done = true;
      }
   }

   return 0;
}

int
GdbController::terminate(void)
{
   return 0;
}

void
GdbController::attach(void)
{
   // Build the Attach message in the outbound buffer.
   AttachMessage *outMsg = (AttachMessage *)initMessage(Attach, sizeof(AttachMessage));
   outMsg->toolId = _toolId;
   outMsg->procSelect = RankInHeader; // We want to attach to the specified rank.
   outMsg->priority = MaxUpdatePriorityLevel; // We want exclusive control.
   strncpy(outMsg->toolTag, "GdbTool", sizeof(outMsg->toolTag));

   // Send the Attach message to toolctld.
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Attach message: " << bgcios::errorString(err));
      std::ostringstream errorMsg;
      errorMsg << "(E) gdbtool failed to send Attach message: " << bgcios::errorString(err) << std::endl;
      putConsoleOutput(errorMsg.str());
      _closeAfterAttached = true;
      return;
   }
   LOG_DEBUG_MSG(_tag << "Attach message sent on tool data channel");

   return;
}

void
GdbController::attachAck(void)
{
   // Get pointer to AttachAck message available from inbound buffer.
   AttachAckMessage *inMsg = (AttachAckMessage *)_inboundMessage;

   // Make sure the Attach message was handled successfully.
   if (inMsg->header.returnCode != bgcios::Success) {
      LOG_ERROR_MSG(_tag << "Attach message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
      std::ostringstream errorMsg;
      errorMsg << "(E) gdbtool failed to attach rank " << inMsg->header.rank << ": " << bgcios::returnCodeToString(inMsg->header.returnCode) << std::endl;
      putConsoleOutput(errorMsg.str());
      _closeAfterAttached = true;
      return;
   }

   _attached = true;

   LOG_DEBUG_MSG(_tag << "successfully attached to rank");

   // Send the Control message to gain control of the compute node.
   control();

   return;
}

void
GdbController::control(void)
{
   // Build the Control message in the outbound buffer.
   ControlMessage *outMsg = (ControlMessage *)initMessage(Control, sizeof(ControlMessage));
   outMsg->toolId = _toolId;
   outMsg->sndSignal = SIGTRAP;
   sigset_t signalSet; 
   sigfillset(&signalSet); // We want to be notified of all signals.
   outMsg->notifySignalSet(&signalSet);
   outMsg->dacTrapMode = bgcios::toolctl::TrapAfterDAC;
   outMsg->dynamicNotifyMode = bgcios::toolctl::DynamicNotifyStart;

   // Send the Control message to toolctld.
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Control message: " << bgcios::errorString(err));
      std::ostringstream errorMsg;
      errorMsg << "(E) gdbtool failed to send Control message: " << bgcios::errorString(err) << std::endl;
      putConsoleOutput(errorMsg.str());
      _closeAfterAttached = true;
      return;
   }
   LOG_DEBUG_MSG(_tag << "Control message sent on tool data channel");

   return;
}

void
GdbController::controlAck(void)
{
   // Get pointer to ControlAck message available from inbound buffer.
   ControlAckMessage *inMsg = (ControlAckMessage *)_inboundMessage;

   // Make sure the Control message was handled successfully.
   if (inMsg->header.returnCode != bgcios::Success) {
      if (inMsg->header.returnCode == ToolControlConflict) {
         LOG_CIOS_INFO_MSG(_tag << "waiting for '" << inMsg->toolTag << "' (" << inMsg->controllingToolId <<
                      ") with priority " << (uint32_t)inMsg->priority << " to release control authority");
         return;
      }
      else {
         LOG_ERROR_MSG(_tag << "Control message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
         std::ostringstream errorMsg;
         errorMsg << "(E) gdbtool failed to obtain control authority for rank " << inMsg->header.rank << ": " << bgcios::returnCodeToString(inMsg->header.returnCode) << std::endl;
         putConsoleOutput(errorMsg.str());
         _closeAfterAttached = true;
         return;
      }
   }
   _notifyPending = 1; // Client is expecting a notify message from the compute node.

   LOG_DEBUG_MSG(_tag << "obtained control authority for rank");
   return;
}

void
GdbController::detach()
{
   // Build the Detach message in the outbound buffer.
   DetachMessage *outMsg = (DetachMessage *)initMessage(Detach, sizeof(DetachMessage));
   outMsg->toolId = _toolId;

   // Send the Detach message to toolctld.
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Detach message: " << bgcios::errorString(err));
      _done = true; // There is nothing we can do to recover.
   }
   LOG_DEBUG_MSG(_tag << "Detach message sent on tool data channel");

   return;
}

void
GdbController::detachAck(void)
{
   // Get pointer to DetachAck message available from inbound buffer.
   DetachAckMessage *inMsg = (DetachAckMessage *)_inboundMessage;

   // Make sure the Detach message was handled successfully.
   if (inMsg->header.returnCode != bgcios::Success) {
      LOG_ERROR_MSG(_tag << "Detach message failed: " << bgcios::returnCodeToString(inMsg->header.returnCode));
   }

   // There is nothing else to do now that we are detached.
   _done = true;

   // No longer attached.
   _attached = false;

   return;
}

void
GdbController::queryAck(void)
{
   // Get pointer to QueryAck message available from inbound buffer.
   QueryAckMessage *inMsg = (QueryAckMessage *)_inboundMessage;
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("QueryAck message", inMsg, inMsg->header.length));

   // Build the response for the gdb client.
   std::ostringstream response;
   switch (_currentGdbCommand) {
      case GdbReadAllRegisters:
         readAllRegistersAck(inMsg, response);
         break;

      case GdbReadMemory:
         readMemoryAck(inMsg, response);
         break;

      case GdbGeneralQuery:
      {
         switch (_currentQuery) {
            case GdbQueryAuxvRead:
               readAuxiliaryVectorAck(inMsg, response);
               break;

            case GdbQueryThreadInfo:
               threadInfoAck(inMsg, response);
               break;

            case GdbQueryThreadExtraInfo:
               threadExtraInfoAck(inMsg, response);
               break;

            default:
               LOG_ERROR_MSG("bad query");
               break;
         }
         break;
      }

      case GdbThreadAlive:
         threadAliveAck(inMsg, response);
         break;

      default:
         LOG_ERROR_MSG(_tag << "current gdb command '" << _currentGdbCommand << "' is not supported");
         break;
   }
   _currentGdbCommand = GdbNoCommand;

   // Send the response to the gdb client.
   std::string command = response.str();
   if (!command.empty()) {
      _gdbChannel->makeCommand(command);
      _gdbChannel->putCommand(command);
   }

   return;
}

void
GdbController::updateAck(void)
{
   // Get pointer to UpdateAck message available from inbound buffer.
   UpdateAckMessage *inMsg = (UpdateAckMessage *)_inboundMessage;
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("UpdateAck message", inMsg, inMsg->header.length));

   // Build the response for the gdb client.
   std::ostringstream response;
   switch (_currentGdbCommand) {
      case GdbWriteMemory:
      case GdbWriteMemoryBinary:
         writeMemoryAck(inMsg, response);
         break;

      case GdbContinue:
      case GdbContinueWithSignal:
      case GdbStep:
      case GdbInterrupt:
         defaultUpdateAck(inMsg);
         _notifyPending = 1; // client is expecting a notify message from the compute node. 
         break;

      case GdbWriteOneRegister:
         writeOneRegisterAck(inMsg, response);
         break;

      case GdbRemoveBreakpoint:
         removeBreakpointAck(inMsg, response);
         break;

      case GdbInsertBreakpoint:
         insertBreakpointAck(inMsg, response);
         break;

      case GdbKillProcess:
         killProcessAck(inMsg, response);
         break;

      default:
         LOG_ERROR_MSG(_tag << "current gdb command '" << _currentGdbCommand << "' is not supported");
         break;
   }
   if (!_retryInProgress) {
      _currentGdbCommand = GdbNoCommand;
   }
   else {
      _retryInProgress = false;
   }

   // Send the response to the gdb client.
   std::string command = response.str();
   if (!command.empty()) {
      _gdbChannel->makeCommand(command);
      _gdbChannel->putCommand(command);
   }

   return;
}

void
GdbController::defaultUpdateAck(UpdateAckMessage *inMsg)
{
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "UpdateAck message in response to gdb command " << _currentGdbCommand << " has bad return code " << inMsg->cmdList[0].returnCode);
      return;
   }

   LOG_DEBUG_MSG(_tag << "UpdateAck message in response to gdb command '" << _currentGdbCommand << "' received from tool data channel");
   return;
}

void
GdbController::notify(void)
{
   // Get pointer to Notify message available from inbound buffer.
   NotifyMessage *inMsg = (NotifyMessage*)_inboundMessage;
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("NotifyMessage", inMsg, inMsg->header.length));

   // Build the response for the gdb client.
   std::ostringstream response;

   switch (inMsg->notifyMessageType) {
      case NotifyMessageType_Signal:
      {
         // Every signal reason includes the signal number and thread in the response.
         response << "T" << std::hex << std::setw(2) << std::setfill('0') << inMsg->type.signal.signum << "thread:" << inMsg->type.signal.threadID << ";";

         // Add additional information based on the signal reason.
         switch (inMsg->type.signal.reason) {
            case NotifySignal_Generic:
            case NotifySignal_Breakpoint:
            case NotifySignal_StepComplete:
               _threadIdForGeneralOps = _threadIdForContinueOps = inMsg->type.signal.threadID;
               LOG_DEBUG_MSG(_tag << "Notify message for tid " << inMsg->type.signal.threadID << " says thread received signal " << inMsg->type.signal.signum <<
                             " for reason " << (int)inMsg->type.signal.reason);
               break;

            case NotifySignal_WatchpointRead:
               response << "rwatch:" << std::hex << std::setw(16) << std::setfill('0') << inMsg->type.signal.dataAddress << ";";
               _threadIdForGeneralOps = _threadIdForContinueOps = inMsg->type.signal.threadID;
               LOG_DEBUG_MSG(_tag << "Notify message for tid " << inMsg->type.signal.threadID << " says thread received signal " << inMsg->type.signal.signum <<
                             " for read watchpoint at address 0x" << std::hex << std::setw(16) << std::setfill('0') << inMsg->type.signal.dataAddress);
               break;

            case NotifySignal_WatchpointWrite:
               response << "watch:" << std::hex << std::setw(16) << std::setfill('0') << inMsg->type.signal.dataAddress << ";";
               _threadIdForGeneralOps = _threadIdForContinueOps = inMsg->type.signal.threadID;
               LOG_DEBUG_MSG(_tag << "Notify message for tid " << inMsg->type.signal.threadID << " says thread received signal " << inMsg->type.signal.signum <<
                             " for write watchpoint at address 0x" << std::hex << std::setw(16) << std::setfill('0') << inMsg->type.signal.dataAddress);
               break;

            default:
               response.clear();
               LOG_ERROR_MSG(_tag << "Notify message contains invalid signal reason " << (uint32_t)inMsg->type.signal.reason);
               break;
         }
         break;
      }

      case NotifyMessageType_Termination:
      {
          _processEnded = true;
         if (_processKilled) { // Did the GDB client request that the process be killed?
             return; 
         }

         // Is the client expecting a response?
         if (_notifyPending) {
             if (WIFEXITED(inMsg->type.termination.exitStatus)) {
                 response << "W" << std::hex << std::setw(2) << std::setfill('0') << WEXITSTATUS(inMsg->type.termination.exitStatus);
                 LOG_DEBUG_MSG(_tag << "Notify message says process exited with status " << WEXITSTATUS(inMsg->type.termination.exitStatus));
             }
             else if (WIFSIGNALED(inMsg->type.termination.exitStatus)) {
                 response << "X" << std::hex << std::setw(2) << std::setfill('0') << WTERMSIG(inMsg->type.termination.exitStatus);
                 LOG_DEBUG_MSG(_tag << "Notify message says process ended by signal " << WTERMSIG(inMsg->type.termination.exitStatus));
             }
             else {
                 LOG_ERROR_MSG(_tag << "Notify message contains invalid exit status 0x" << std::hex << inMsg->type.termination.exitStatus);
             }
         }
         break;
      }

      case NotifyMessageType_Control:
      {
         switch (inMsg->type.control.reason) {
            case NotifyControl_Conflict:
               LOG_CIOS_WARN_MSG(_tag << "Notify message says '" << inMsg->type.control.toolTag << "' (" << inMsg->type.control.toolid << ") wants control, request was not granted");
               break;

            case NotifyControl_Available:
               LOG_DEBUG_MSG(_tag << "Notify message says control authority is available, trying to obtain control authority");
               control();
               break;

            default:
               LOG_ERROR_MSG(_tag << "Notify message contains invalid control reason " << (uint32_t)inMsg->type.control.reason);
               break;
         }


         break;
      }

      default:
      {
         LOG_ERROR_MSG(_tag << "Notify message contains unrecognized type " << (uint32_t)(inMsg->notifyMessageType));
         break;
      }
   }

   // Send the response to the gdb client.
   std::string command = response.str();
   if (!command.empty()) {
      _gdbChannel->makeCommand(command);
      _gdbChannel->putCommand(command);
   }
   _notifyPending = 0;
   return;
}

void
GdbController::cont(std::string& command, std::string& response)
{
   // Validate the continue gdb command.
   uint32_t signum = 0;
   if (command.at(0) == 'c') {
      if (command.size() > 1) {
         // An optional address is not supported.
         response = _gdbChannel->errorResponse(EINVAL);
         return;
      }
      _currentGdbCommand = GdbContinue;
   }
   else {
      size_t pos = command.find_first_of(';', 1);
      if (pos != std::string::npos) {
         // An optional address is not supported.
         response = _gdbChannel->errorResponse(EINVAL);
         return;
      }
      signum = stringToHex<uint32_t>(command.substr(1));

      _currentGdbCommand = GdbContinueWithSignal;
   }

   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 2;
   uint32_t nextOffset = sizeof(UpdateMessage);

   // Add set continuation signal command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), SetContinuationSignal, sizeof(SetContinuationSignalCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(SetContinuationSignalCmd);
   SetContinuationSignalCmd *cmd1 = (SetContinuationSignalCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd1->threadID = _threadIdForContinueOps;
   cmd1->signum = signum;

   // Add continue command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[1]), ContinueProcess, sizeof(ContinueProcessCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(ContinueProcessCmd);
   ContinueProcessCmd *cmd2= (ContinueProcessCmd *)(_outboundMessage + outMsg->cmdList[1].offset);
   cmd2->threadID = _threadIdForContinueOps;

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to continue", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to continue: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   LOG_DEBUG_MSG(_tag << "Update message for tid " << cmd2->threadID << " to continue sent on tool data channel");

   return;
}

void
GdbController::readAllRegisters(std::string& command, std::string& response)
{
   // Validate the read all registers gdb command.
   if (command.size() > 1) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }

   // Build the Query message in the outbound buffer.
   QueryMessage *outMsg = (QueryMessage *)initMessage(Query, sizeof(QueryMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 3;
   uint32_t nextOffset = sizeof(QueryMessage);

   // Add get general registers command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), GetGeneralRegs, sizeof(GetGeneralRegsCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetGeneralRegsCmd);
   GetGeneralRegsCmd *generalRegs = (GetGeneralRegsCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   generalRegs->threadID = _threadIdForGeneralOps;

   // Add get floating point registers command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[1]), GetFloatRegs, sizeof(GetFloatRegsCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetFloatRegsCmd);
   GetFloatRegsCmd *floatRegs = (GetFloatRegsCmd *)(_outboundMessage + outMsg->cmdList[1].offset);
   floatRegs->threadID = _threadIdForGeneralOps;

   // Add get special registers command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[2]), GetSpecialRegs, sizeof(GetSpecialRegsCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetSpecialRegsCmd);
   GetSpecialRegsCmd *specialRegs = (GetSpecialRegsCmd *)(_outboundMessage + outMsg->cmdList[2].offset);
   specialRegs->threadID = _threadIdForGeneralOps;

   // Send the Query message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Query message to read all registers", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Query message to read registers: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbReadAllRegisters;
   LOG_DEBUG_MSG(_tag << "Query message for tid " << _threadIdForGeneralOps << " to read all registers sent on tool data channel");

   return;
}

void
GdbController::readAllRegistersAck(QueryAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure all of the commands were successful.
   if ((inMsg->cmdList[0].type != GetGeneralRegsAck) || (inMsg->cmdList[0].returnCode != CmdSuccess)) {
      LOG_ERROR_MSG(_tag << "QueryAck message to read all registers first command " << inMsg->cmdList[0].type <<
                    " has bad return code " << inMsg->cmdList[0].returnCode);

      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }
   
   if ((inMsg->cmdList[1].type != GetFloatRegsAck) || (inMsg->cmdList[1].returnCode != CmdSuccess)) {
      LOG_ERROR_MSG(_tag << "QueryAck message to read all registers second command " << inMsg->cmdList[1].type <<
                    " has bad return code " << inMsg->cmdList[1].returnCode);
      response << commandErrorResponse(inMsg->cmdList[1].returnCode);
      return;
   }

   if ((inMsg->cmdList[2].type != GetSpecialRegsAck) || (inMsg->cmdList[2].returnCode != CmdSuccess)) {
      LOG_ERROR_MSG(_tag << "QueryAck message to read all registers third command " << inMsg->cmdList[1].type <<
                    " has bad return code " << inMsg->cmdList[1].returnCode);
      response << commandErrorResponse(inMsg->cmdList[2].returnCode);
      return;
   }

   // Note that the order and size of the registers in the response must match the register format defined in the 
   // bgq/gnu/gdb-7.1/gdb/regformats/rs6000/powerpc-bgq64.dat file.

   // Add the general purpose registers to the response.
   GetGeneralRegsAckCmd *generalRegs = (GetGeneralRegsAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   for (uint32_t index = 0; index < NumGPRegs; ++index) {
      response << std::hex << std::setw(16) << std::setfill('0') << generalRegs->gpr[index];
   }

   // Add the first part (of four) of the floating point registers to the response.
   GetFloatRegsAckCmd *floatRegs = (GetFloatRegsAckCmd *)((char *)inMsg + inMsg->cmdList[1].offset);
   for (uint32_t index = 0; index < NumFPRegs; ++index) {
      response << std::hex << std::setw(16) << std::setfill('0') << floatRegs->fpr[index].ll[0];
   }

   // Add the special registers to the response.
   GetSpecialRegsAckCmd *specialRegs = (GetSpecialRegsAckCmd *)((char *)inMsg + inMsg->cmdList[2].offset);
   response << std::hex << std::setw(16) << std::setfill('0') << specialRegs->sregs.iar;
   response << std::hex << std::setw(16) << std::setfill('0') << specialRegs->sregs.msr;
   response << std::hex << std::setw(8) << std::setfill('0') << specialRegs->sregs.cr;
   response << std::hex << std::setw(16) << std::setfill('0') << specialRegs->sregs.lr;
   response << std::hex << std::setw(16) << std::setfill('0') << specialRegs->sregs.ctr;
   response << std::hex << std::setw(8) << std::setfill('0') << specialRegs->sregs.xer;
   response << std::hex << std::setw(8) << std::setfill('0') << specialRegs->sregs.fpscr;

   // Add the remaining three parts (of four) of the floating point registers to the response.
   for (uint32_t index = 0; index < NumFPRegs; ++index) {
      response << std::hex << std::setw(16) << std::setfill('0') << floatRegs->fpr[index].ll[1];
      response << std::hex << std::setw(16) << std::setfill('0') << floatRegs->fpr[index].ll[2];
      response << std::hex << std::setw(16) << std::setfill('0') << floatRegs->fpr[index].ll[3];
   }

   LOG_DEBUG_MSG(_tag << "QueryAck message for tid " << specialRegs->threadID << " to read all registers received from tool data channel");
   return;
}

void
GdbController::writeAllRegisters(std::string& command, std::string& response)
{
   // Validate the write all registers gdb command.
   if (command.size() > 1) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   _currentGdbCommand = GdbWriteAllRegisters;

   // Not supported yet.
   response = _gdbChannel->errorResponse(EINVAL);

   return;
}

void
GdbController::writeOneRegister(std::string& command, std::string& response)
{
   // Validate the write one register gdb command.
   size_t valuepos = command.find_first_of('=', 1);
   if (valuepos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++valuepos;
   std::string registerNumber = command.substr(1, valuepos-2);
   std::string value = command.substr(valuepos);
   LOG_DEBUG_MSG(_tag << "validated write one register command, number=" << registerNumber << " value=" << value);

   // Make sure the register number is supported.
   uint32_t gdbRegisterNumber = stringToHex<uint32_t>(registerNumber);
   if (gdbRegisterNumber >= GdbRegisterEnd) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }

   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(UpdateMessage);

   // Add a set general register command to the message.
   if (gdbRegisterNumber <= GdbRegisterGPR31) {
      nextOffset = initCommand(&(outMsg->cmdList[0]), SetGeneralReg, sizeof(SetGeneralRegCmd), nextOffset);
      outMsg->header.length += (uint32_t)sizeof(SetGeneralRegCmd);
      SetGeneralRegCmd *cmd = (SetGeneralRegCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
      cmd->threadID = _threadIdForGeneralOps;
      cmd->reg_select = (GeneralRegSelect)gdbRegisterNumber;
      cmd->value = stringToHex<BG_Reg_t>(value);
   }

   // Add a set float register command to the message.
   else if ((gdbRegisterNumber >= GdbRegisterFPR00) && (gdbRegisterNumber <= GdbRegisterFPR31)) {
      nextOffset = initCommand(&(outMsg->cmdList[0]), SetFloatReg, sizeof(SetFloatRegCmd), nextOffset);
      outMsg->header.length += (uint32_t)sizeof(SetFloatRegCmd);
      SetFloatRegCmd *cmd = (SetFloatRegCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
      cmd->threadID = _threadIdForGeneralOps;
      cmd->reg_select = (FloatRegSelect)(gdbRegisterNumber - GdbRegisterFPR00);
      cmd->value.ll[0] = stringToHex<BG_Reg_t>(value);
   }

   // Add a set special register command to the message.
   else {
      nextOffset = initCommand(&(outMsg->cmdList[0]), SetSpecialReg, sizeof(SetSpecialRegCmd), nextOffset);
      outMsg->header.length += (uint32_t)sizeof(SetSpecialRegCmd);
      SetSpecialRegCmd *cmd = (SetSpecialRegCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
      cmd->threadID = _threadIdForGeneralOps;
      switch (gdbRegisterNumber) {
         case GdbRegisterIAR:   cmd->reg_select = iar;   break;
         case GdbRegisterMSR:   cmd->reg_select = msr;   break;
         case GdbRegisterCR:    cmd->reg_select = cr;    break;
         case GdbRegisterLR:    cmd->reg_select = lr;    break;
         case GdbRegisterCTR:   cmd->reg_select = ctr;   break;
         case GdbRegisterXER:   cmd->reg_select = xer;   break;
         case GdbRegisterFPSCR: cmd->reg_select = fpscr; break;
      }
      cmd->value = stringToHex<BG_Reg_t>(value);
   }

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to set a register", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to set a register: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbWriteOneRegister;
   LOG_DEBUG_MSG(_tag << "Update message for tid " << _threadIdForGeneralOps << " to set register " << gdbRegisterNumber << " sent on tool data channel");

   return;
}

void
GdbController::writeOneRegisterAck(UpdateAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "UpdateAck message to set a register has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response to the gdb client.
   ToolCommand *cmd = (ToolCommand *)((char *)inMsg + inMsg->cmdList[0].offset);
   response << SuccessResponse;
   LOG_DEBUG_MSG(_tag << "UpdateAck message for tid " << cmd->threadID << " to set a register received from tool data channel");
   return;
}

void
GdbController::setThread(std::string& command, std::string& response)
{
   // Find the operation.
   BG_ThreadID_t threadId = 0;
   switch (command.at(1)) {
      case 'c':
         if (command.find("-1", 2) == 2) { //! \todo -1 means all threads
            threadId = 0;
         }
         else {
            std::string substr = command.substr(2);
            threadId = stringToHex<BG_ThreadID_t>(substr);
         }
         _threadIdForContinueOps = threadId;
         LOG_DEBUG_MSG(_tag << "thread ID for continue operations set to " << threadId);
         response = SuccessResponse;
         break;

      case 'g':
         if (command.find("-1", 2) == 2) { //! \todo -1 means all threads
            threadId = 0;
         }
         else {
            std::string substr = command.substr(2);
            threadId = stringToHex<BG_ThreadID_t>(substr);
         }
         _threadIdForGeneralOps = threadId;
         LOG_DEBUG_MSG(_tag << "thread ID for general operations set to " << threadId);
         response = SuccessResponse;
         break;

      default:
         response = _gdbChannel->errorResponse(EINVAL);
         break;
   }

   return;
}

void
GdbController::killProcess(std::string& command, std::string& response)
{
   // Validate the kill process gdb command.
   if (command.size() > 1) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(UpdateMessage);
   
   // setup the send signal to process message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), SendSignalToProcess, sizeof(SendSignalToProcessCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(SendSignalToProcessCmd);
   SendSignalToProcessCmd *cmd1 = (SendSignalToProcessCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd1->threadID = 0; // Let CNK pick the thread for us
   cmd1->signum = SIGKILL;
   
   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to send signal SIGKILL", outMsg, outMsg->header.length));
   sendToToolChannel(outMsg);
   _currentGdbCommand = GdbKillProcess;
   response.clear(); // No response is necessary
   return;
}

void
GdbController::killProcessAck(UpdateAckMessage *inMsg, std::ostringstream& )
{
   ToolCommand *cmd = (ToolCommand *)((char *)inMsg + inMsg->cmdList[0].offset);

   // ignore errors. The process may already being dieing

   LOG_DEBUG_MSG(_tag << "UpdateAck message for tid " << cmd->threadID << " to send SIGKILL ");
   return;
}


void
GdbController::readMemory(std::string& command, std::string& response)
{
   // Validate the read memory gdb command.
   size_t lenpos = command.find_first_of(',', 1);
   if (lenpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++lenpos;
   std::string addr = command.substr(1, lenpos-2);
   std::string length = command.substr(lenpos);
   LOG_DEBUG_MSG(_tag << "validated read memory command, addr=" << addr << " length=" << length);

   // Build the Query message in the outbound buffer.
   QueryMessage *outMsg = (QueryMessage *)initMessage(Query, sizeof(QueryMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(QueryMessage);

   // Add get memory command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), GetMemory, sizeof(GetMemoryCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetMemoryCmd);
   GetMemoryCmd *cmd = (GetMemoryCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = _threadIdForGeneralOps;
   cmd->addr = stringToHex<BG_Addr_t>(addr);
   cmd->length = stringToHex<uint32_t>(length);

   // Send the Query message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Query message to read memory", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Query message to read memory: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbReadMemory;
   LOG_DEBUG_MSG(_tag << "Query message for tid " << cmd->threadID << " to read memory from address 0x" << std::hex << std::setw(16) << std::setfill('0') <<
                 cmd->addr << " for " << std::dec << cmd->length << " bytes sent on tool data channel");

   return;
}

void
GdbController::readMemoryAck(QueryAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   if (inMsg->cmdList[0].type != GetMemoryAck) {
      LOG_ERROR_MSG(_tag << "QueryAck message to read memory first command " << inMsg->cmdList[0].type << " is invalid");
      _done = true;
      return;
   }
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "QueryAck message to ready memory first command has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response with the memory contents.
   GetMemoryAckCmd *cmd = (GetMemoryAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   for (uint32_t index = 0; index < cmd->length; ++index) {
      response << std::hex << std::setw(2) << std::setfill('0') << (int)cmd->data[index];
   }
   LOG_DEBUG_MSG(_tag << "QueryAck message for tid " << cmd->threadID << " to read memory from address 0x" << std::hex << std::setw(16) << std::setfill('0') <<
                 cmd->addr << " for " << std::dec << cmd->length << " bytes received from tool data channel");
   return;
}

void
GdbController::writeMemory(std::string& command, std::string& response)
{
   // Validate the write memory gdb command. 
   size_t lenpos = command.find_first_of(',', 1);
   if (lenpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++lenpos;
   size_t datapos = command.find_first_of(':', lenpos);
   if (datapos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++datapos;
   std::string addr = command.substr(1, lenpos-2);
   std::string length = command.substr(lenpos, datapos-lenpos-1);
   std::string data = command.substr(datapos);
   LOG_DEBUG_MSG(_tag << "validated write memory command, addr=" << addr << " length=" << length);

   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(UpdateMessage);

   // Add set memory command to the message.
   uint32_t datalength = stringToHex<uint32_t>(length);
   if (datalength > MaxMemorySize) {
      response = _gdbChannel->errorResponse(E2BIG);
      return;
   }
   nextOffset = initCommand(&(outMsg->cmdList[0]), SetMemory, (uint32_t)(sizeof(SetMemoryCmd)) + datalength, nextOffset);
   outMsg->header.length += (uint32_t)sizeof(SetMemoryCmd) + datalength;
   SetMemoryCmd *cmd = (SetMemoryCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = _threadIdForGeneralOps;
   cmd->addr = stringToHex<BG_Addr_t>(addr);
   cmd->length = stringToHex<uint32_t>(length);
   if (command.at(0) == 'M') {
      for (uint32_t index = 0; index < cmd->length; ++index) {
         std::string byte;
         byte.push_back(data.at(index));
         cmd->data[index] = stringToHex<unsigned char>(byte);
      }
   }
   else {
      LOG_DEBUG_MSG(_tag << "write memory binary command source length " << data.size() << " and destination length " << cmd->length);
      getBinaryData(data, cmd->data);
   }

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to write memory", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to write memory: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbWriteMemory;
   LOG_DEBUG_MSG(_tag << "Update message for tid " << cmd->threadID << " to write memory at address 0x" << std::hex << std::setw(16) << std::setfill('0') <<
                 cmd->addr << " for " << std::dec << cmd->length << " bytes sent on tool data channel");

   return;
}

void
GdbController::writeMemoryAck(UpdateAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   if (inMsg->cmdList[0].type != SetMemoryAck) {
      LOG_ERROR_MSG(_tag << "UpdateAck message to write memory first command " << inMsg->cmdList[0].type << " is invalid");
      _done = true;
      return;
   }
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "UpdateAck message to write memory first command has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response to the gdb client.
   SetMemoryAckCmd *cmd = (SetMemoryAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   response << SuccessResponse;
   LOG_DEBUG_MSG(_tag << "UpdateAck message for tid " << cmd->threadID << " to write memory at address 0x" << std::hex << std::setw(16) << std::setfill('0') <<
                 cmd->addr << " for " << std::dec << cmd->length << " bytes received from tool data channel");
   return;
}

void
GdbController::generalQuery(std::string& command, std::string& response)
{
   // Initialize response.
   response.clear();

   // Handle the supported features query command.
   if (command.find("Supported", 1) != std::string::npos) {

      // Parse the features that the gdb client supports.
      size_t pos = command.find_first_of(':');
      if (pos != std::string::npos) {
         LOG_DEBUG_MSG("Rank " << _gdbChannel->getRank() << ": gdb client supports these features: " << command.substr(pos));
#if 0
         if (command.find("multiprocess") != std::string::npos) {
            LOG_CIOS_INFO_MSG("Rank " _gdbChannel->getRank() << ": gdb client supports multiprocess extensions to remote protocol");
            gdbChannel->setMultiprocess(true);
         }
#endif
      }

      // Build the response for the client.
      response.append("QStartNoAckMode+;qXfer:auxv:read+;PacketSize=4000"); // PacketSize is in hex so 16384 bytes (decimal)
   }

   // Handle the get current thread id command.
   else if (command.at(1) == 'C') {
      std::ostringstream resp;
      resp << "QC" << std::hex << _threadIdForGeneralOps;
      response.append(resp.str());
   }

   // Handle the auxillary vector read command.
   else if (command.find("qXfer:auxv:read") != std::string::npos) {
      readAuxiliaryVector(command, response);
      _currentGdbCommand = GdbGeneralQuery;
      _currentQuery = GdbQueryAuxvRead;
   }

   // Handle the first get list of active threads command.
   else if (command.find("qfThreadInfo") != std::string::npos) {
      threadInfo(command, response);
      _currentGdbCommand = GdbGeneralQuery;
      _currentQuery = GdbQueryThreadInfo;
   }

   // Handle the second get list of active threads command.
   else if (command.find("qsThreadInfo") != std::string::npos) {
      response = "l";
   }

   // Handle the get thread extra info command.
   else if (command.find("qThreadExtraInfo") != std::string::npos) {
      threadExtraInfo(command, response);
      _currentGdbCommand = GdbGeneralQuery;
      _currentQuery = GdbQueryThreadExtraInfo;
   }

   // Handle the attached command.
   else if (command.find("qAttached") != std::string::npos) {
      if (_existingProcess) {
         response = "1";
      }
      else {
         response = "0";
      }
   }

   // Handle the get section offsets command.
   else if (command.find("qOffsets") != std::string::npos) {
      response.append(UnsupportedCommand);
      if (_closeAfterAttached) {
         LOG_CIOS_INFO_MSG(_tag << "closing connection to gdb client after error attaching to process");
         _done = true;
      }
   }

   // The query command is not supported.
   else {
      response.append(UnsupportedCommand);
   }

   return;
}

void
GdbController::readAuxiliaryVector(std::string& command, std::string& response)
{
   // Validate the read auxiliary vector command.
   size_t offsetPos = command.find("::");
   if (offsetPos == std::string::npos) {
      response = _gdbChannel->errorResponse(0);
      return;
   }
   offsetPos += 2;
   size_t lengthPos = command.find_first_of(',', offsetPos);
   if (lengthPos == std::string::npos) {
      response = _gdbChannel->errorResponse(0);
      return;
   }
   std::string offset = command.substr(offsetPos, lengthPos-offsetPos);
   std::string length = command.substr(lengthPos+1);
   LOG_DEBUG_MSG(_tag << "validated read auxiliary vector command, offset=" << offset << " length=" << length);

   // CNK only supports one request to get auxiliary vector.
   uint32_t offsetValue = stringToHex<uint32_t>(offset); 
   if (offsetValue != 0) {
      LOG_ERROR_MSG(_tag << "read auxiliary buffer offset " << offsetValue << " is not valid");
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }

   // Length is hex.

   // Build the Query message in the outbound buffer.
   QueryMessage *outMsg = (QueryMessage *)initMessage(Query, sizeof(QueryMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(QueryMessage);

   // Add get auxiliary vector command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), GetAuxVectors, sizeof(GetAuxVectorsCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetAuxVectorsCmd);
   GetAuxVectorsCmd *cmd = (GetAuxVectorsCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = _threadIdForGeneralOps;

   // Send the Query message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Query message to read auxiliary vector", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Query message to read auxiliary vector: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   LOG_DEBUG_MSG(_tag << "Query message for tid " << cmd->threadID << " to read auxiliary vector sent on tool data channel");

   return;
}

void
GdbController::readAuxiliaryVectorAck(QueryAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   GetAuxVectorsAckCmd *cmd = (GetAuxVectorsAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "QueryAck message to read auxiliary vector command has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response with the auxiliary vector data.
   response << "l";
   putBinaryData((unsigned char *)cmd->data, (uint32_t)(cmd->length * sizeof(uint64_t)), response);
   LOG_DEBUG_MSG(_tag << "QueryAck message for tid " << cmd->threadID << " to read auxiliary vector received from tool data channel");

   return;
}

void
GdbController::threadInfo(std::string& command, std::string& response)
{
   // Validate the get thread list command.
   if (command.length() != 12) {
      LOG_ERROR_MSG(_tag << "get thread list command has extra data");
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }

   // Build the Query message in the outbound buffer.
   QueryMessage *outMsg = (QueryMessage *)initMessage(Query, sizeof(QueryMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(QueryMessage);

   // Add get thread list command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), GetThreadList, sizeof(GetThreadListCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetThreadListCmd);
   GetThreadListCmd *cmd = (GetThreadListCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = _threadIdForGeneralOps;

   // Send the Query message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Query message to get thread list", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Query message to get thread list: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   LOG_DEBUG_MSG(_tag << "Query message for tid " << cmd->threadID << " to get thread list sent on tool data channel");

   return;
}

void
GdbController::threadInfoAck(QueryAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   GetThreadListAckCmd *cmd = (GetThreadListAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "QueryAck message to get thread list command has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response with the thread list data.
   response << "m";
   for (uint32_t index = 0; index < cmd->numthreads; ++index) {
      response << std::hex << cmd->threadlist[index].tid;
      if (index < (cmd->numthreads-1)) {
         response << ",";
      }
   }
   LOG_DEBUG_MSG(_tag << "QueryAck message for tid " << cmd->threadID << " to get thread list received from tool data channel");

   return;
}

void
GdbController::threadExtraInfo(std::string& command, std::string& response)
{
   // Validate the get thread extra info command.
   size_t tidpos = command.find_first_of(',', 1);
   if (tidpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++tidpos;
   std::string threadId = command.substr(tidpos);
   LOG_DEBUG_MSG(_tag << "validated thread extra info command, threadId=" << threadId);

   // Build the Query message in the outbound buffer.
   QueryMessage *outMsg = (QueryMessage *)initMessage(Query, sizeof(QueryMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(QueryMessage);

   // Add get thread data command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), GetThreadData, sizeof(GetThreadDataCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetThreadDataCmd);
   GetThreadDataCmd *cmd = (GetThreadDataCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = stringToHex<BG_ThreadID_t>(threadId);

   // Send the Query message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Query message to get thread data", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Query message to get thread data: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   LOG_DEBUG_MSG(_tag << "Query message for tid " << cmd->threadID << " to get thread data sent on tool data channel");

   return;
}

void
GdbController::threadExtraInfoAck(QueryAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   GetThreadDataAckCmd *cmd = (GetThreadDataAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "QueryAck message to get thread data command has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response with the thread's attributes.
   std::ostringstream text;
   switch (cmd->state) {
      case Run:
         text << "Running ";
         break;
      case Sleep:
         text << "Sleeping ";
         break;
      case FutexWait:
         text << "Futex waiting ";
         break;
      case Idle:
         text << "Idle ";
         break;
      default:
         text << "Unknown ";
         break;
   }
   text << "on core " << std::setw(2) << std::setfill('0') << cmd->core << " hwthread " << cmd->thread;
   for (size_t index = 0; index < text.str().size(); ++index) {
      response << std::hex << std::setw(2) << std::setfill('0') << (int)text.str().at(index);
   }
   LOG_DEBUG_MSG(_tag << "QueryAck message for tid " << cmd->threadID << " to get thread data received from tool data channel");

   return;
}

void
GdbController::generalSet(std::string& command, std::string& response)
{
   // Process the StartNoAckMode set command.
   if (command.find("StartNoAckMode", 1) != std::string::npos) {
      _gdbChannel->setAckMode(false);
      response = SuccessResponse;
   }

   return;
}

void
GdbController::step(std::string& command, std::string& response)
{
   // Validate the step gdb command.
   uint32_t signum = 0;
   if (command.at(0) == 's') {
      if (command.size() > 1) {
         // An optional address is not supported.
         response = _gdbChannel->errorResponse(EINVAL);
         return;
      }
      _currentGdbCommand = GdbStep;
   }
   else {
      size_t pos = command.find_first_of(';', 1);
      if (pos != std::string::npos) {
         // An optional address is not supported.
         response = _gdbChannel->errorResponse(EINVAL);
         return;
      }
      signum = stringToHex<uint32_t>(command.substr(1));
      _currentGdbCommand = GdbStepWithSignal;
   }

   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 2;
   uint32_t nextOffset = sizeof(UpdateMessage);

    // Add set continuation signal command to the message.
    nextOffset = initCommand(&(outMsg->cmdList[0]), SetContinuationSignal, sizeof(SetContinuationSignalCmd), nextOffset);
    outMsg->header.length += (uint32_t)sizeof(SetContinuationSignalCmd);
    SetContinuationSignalCmd *cmd1 = (SetContinuationSignalCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
    cmd1->threadID = _threadIdForContinueOps;
    cmd1->signum = signum;

   // Add continue command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[1]), StepThread, sizeof(StepThreadCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(StepThreadCmd);
   StepThreadCmd *cmd2 = (StepThreadCmd *)(_outboundMessage + outMsg->cmdList[1].offset);
   cmd2->threadID = _threadIdForContinueOps;

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to step", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to step: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   LOG_DEBUG_MSG(_tag << "Update message for tid " << cmd2->threadID << " to step sent on tool data channel");

   return;
}

void
GdbController::threadAlive(std::string& command, std::string& response)
{
   // Validate the thread alive command.
   if (command.length() < 2) {
      LOG_ERROR_MSG(_tag << "thread id not specified on thread alive command");
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   BG_ThreadID_t threadId = stringToHex<BG_ThreadID_t>(command.substr(1));;

   // Build the Query message in the outbound buffer.
   QueryMessage *outMsg = (QueryMessage *)initMessage(Query, sizeof(QueryMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(QueryMessage);

   // Add get thread list command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), GetThreadList, sizeof(GetThreadListCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(GetThreadListCmd);
   GetThreadListCmd *cmd = (GetThreadListCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = threadId;

   // Send the Query message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Query message to find out if thread is alive", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Query message to find out if thread is alive: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbThreadAlive;
   LOG_DEBUG_MSG(_tag << "Query message for tid " << cmd->threadID << " to find out if thread is alive sent on tool data channel");

   return;
}

void
GdbController::threadAliveAck(QueryAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   GetThreadListAckCmd *cmd = (GetThreadListAckCmd *)((char *)inMsg + inMsg->cmdList[0].offset);
   if (inMsg->cmdList[0].returnCode == CmdTIDinval) {
       response << _gdbChannel->errorResponse(ESRCH);
       return;
   }
   else if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "QueryAck message to find out if thread is alive command has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }
   // Build the response with the thread list data.
   for (uint32_t index = 0; index < cmd->numthreads; ++index) {
      if (cmd->threadID == cmd->threadlist[index].tid) {
         response << "OK";
      }
   }
   if (response.str().length() == 0) {
      response << _gdbChannel->errorResponse(ESRCH);
   }
   LOG_DEBUG_MSG(_tag << "QueryAck message for tid " << cmd->threadID << " to find out if thread is alive received from tool data channel");

   return;
}

void
GdbController::removeBreakpoint(std::string& command, std::string& response)
{
   // Validate the remove breakpoint command.
   size_t addrpos = command.find_first_of(',', 1);
   if (addrpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++addrpos;
   size_t kindpos = command.find_first_of(',', addrpos);
   if (kindpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++kindpos;
   std::string type = command.substr(1, addrpos-2);
   std::string addr = command.substr(addrpos, kindpos-addrpos-1);
   std::string kind = command.substr(kindpos);
   LOG_DEBUG_MSG(_tag << "validated remove breakpoint command, type=" << type << " addr=" << addr << " kind=" << kind);

   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(UpdateMessage);

   switch (type.at(0)) {
      case '2':
      case '3':
      case '4':
      {
         // Add reset watchpoint command to the message.
         nextOffset = initCommand(&(outMsg->cmdList[0]), ResetWatchpoint, sizeof(ResetWatchpointCmd), nextOffset);
         outMsg->header.length += (uint32_t)sizeof(ResetWatchpointCmd);
         ResetWatchpointCmd *cmd = (ResetWatchpointCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
         cmd->threadID = _threadIdForGeneralOps;
         cmd->addr = stringToHex<BG_Addr_t>(addr);
         break;
      }

      default:
         LOG_DEBUG_MSG(_tag << "remove breakpoint command type " << type << " is not supported");
         response.append(UnsupportedCommand); // Command is unsupported.
         return;
   }

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to remove breakpoint or watchpoint", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to remove breakpoint or watchpoint: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbRemoveBreakpoint;
   LOG_DEBUG_MSG(_tag << "Update message for tid " << _threadIdForGeneralOps << " to remove breakpoint or watchpoint sent on tool data channel");

   return;
}

void
GdbController::removeBreakpointAck(UpdateAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "UpdateAck message to remove breakpoint or watchpoint has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response to the gdb client.
   ToolCommand *cmd = (ToolCommand *)((char *)inMsg + inMsg->cmdList[0].offset);
   response << SuccessResponse;
   LOG_DEBUG_MSG(_tag << "UpdateAck message for tid " << cmd->threadID << " to remove breakpoint or watchpoint received from tool data channel");
   return;
}

void
GdbController::insertBreakpoint(std::string& command, std::string& response)
{
   // Validate the insert breakpoint command.
   size_t addrpos = command.find_first_of(',', 1);
   if (addrpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++addrpos;
   size_t kindpos = command.find_first_of(',', addrpos);
   if (kindpos == std::string::npos) {
      response = _gdbChannel->errorResponse(EINVAL);
      return;
   }
   ++kindpos;
   std::string type = command.substr(1, addrpos-2);
   std::string addr = command.substr(addrpos, kindpos-addrpos-1);
   std::string kind = command.substr(kindpos);
   LOG_DEBUG_MSG(_tag << "validated insert breakpoint command, type=" << type << " addr=" << addr << " kind=" << kind);

   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(UpdateMessage);

   //! \note The gdb remote protocol does not include enough information in the Z0 and z0 packets to use the
   //!       SetBreakpointCmd and ResetBreakpointCmd commands.

   switch (type.at(0)) {
      case '2':
      {
         // Add set write watchpoint command to the message. 
         nextOffset = initCommand(&(outMsg->cmdList[0]), SetWatchpoint, sizeof(SetWatchpointCmd), nextOffset);
         outMsg->header.length += (uint32_t)sizeof(SetWatchpointCmd);
         SetWatchpointCmd *cmd = (SetWatchpointCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
         cmd->threadID = _threadIdForGeneralOps;
         cmd->addr = stringToHex<BG_Addr_t>(addr);
         cmd->length = stringToHex<uint32_t>(kind);
         cmd->type = WatchWrite;
         break;
      }

      case '3':
      {
         // Add set read watchpoint command to the message. 
         nextOffset = initCommand(&(outMsg->cmdList[0]), SetWatchpoint, sizeof(SetWatchpointCmd), nextOffset);
         outMsg->header.length += (uint32_t)sizeof(SetWatchpointCmd);
         SetWatchpointCmd *cmd = (SetWatchpointCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
         cmd->threadID = _threadIdForGeneralOps;
         cmd->addr = stringToHex<BG_Addr_t>(addr);
         cmd->length = stringToHex<uint32_t>(kind);
         cmd->type = WatchRead;
         break;
      }

      case '4':
      {
         // Add set read/write watchpoint command to the message. 
         nextOffset = initCommand(&(outMsg->cmdList[0]), SetWatchpoint, sizeof(SetWatchpointCmd), nextOffset);
         outMsg->header.length += (uint32_t)sizeof(SetWatchpointCmd);
         SetWatchpointCmd *cmd = (SetWatchpointCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
         cmd->threadID = _threadIdForGeneralOps;
         cmd->addr = stringToHex<BG_Addr_t>(addr);
         cmd->length = stringToHex<uint32_t>(kind);
         cmd->type = WatchReadWrite;
         break;
      }

      default:
         LOG_DEBUG_MSG(_tag << "insert breakpoint command type " << type << " is not supported");
         response.append(UnsupportedCommand); // Command is unsupported.
         return;
   }

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to insert breakpoint or watchpoint", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to insert breakpoint or watchpoint: " << bgcios::errorString(err));
      response = _gdbChannel->errorResponse(err);
      _done = true; // There is nothing we can do to recover.
      return;
   }
   _currentGdbCommand = GdbInsertBreakpoint;
   LOG_DEBUG_MSG(_tag << "Update message for tid " << _threadIdForGeneralOps << " to insert breakpoint or watchpoint sent on tool data channel");

   return;
}

void
GdbController::insertBreakpointAck(UpdateAckMessage *inMsg, std::ostringstream& response)
{
   // Make sure the command was successful.
   if (inMsg->cmdList[0].returnCode != CmdSuccess) {
      LOG_ERROR_MSG(_tag << "UpdateAck message to insert breakpoint or watchpoint has bad return code " << inMsg->cmdList[0].returnCode);
      response << commandErrorResponse(inMsg->cmdList[0].returnCode);
      return;
   }

   // Build the response to the gdb client.
   ToolCommand *cmd = (ToolCommand *)((char *)inMsg + inMsg->cmdList[0].offset);
   response << SuccessResponse;
   LOG_DEBUG_MSG(_tag << "UpdateAck message for tid " << cmd->threadID << " to insert breakpoint or watchpoint received from tool data channel");
   return;
}

void
GdbController::interrupt(void)
{
   // Build the Update message in the outbound buffer.
   UpdateMessage *outMsg = (UpdateMessage *)initMessage(Update, sizeof(UpdateMessage));
   outMsg->toolId = _toolId;
   outMsg->numCommands = 1;
   uint32_t nextOffset = sizeof(UpdateMessage);

   // Add send signal command to the message.
   nextOffset = initCommand(&(outMsg->cmdList[0]), SendSignal, sizeof(SendSignalToProcessCmd), nextOffset);
   outMsg->header.length += (uint32_t)sizeof(SendSignalToProcessCmd);
   SendSignalToProcessCmd *cmd = (SendSignalToProcessCmd *)(_outboundMessage + outMsg->cmdList[0].offset);
   cmd->threadID = _threadIdForGeneralOps;
   cmd->signum = SIGINT;

   // Send the Update message to the compute node.
   LOG_CIOS_TRACE_MSG(_tag << dumpHex("Update message to interrupt process", outMsg, outMsg->header.length));
   int err = sendToToolChannel(outMsg);
   if (err != 0) {
      LOG_ERROR_MSG(_tag << "error sending Update message to interrupt process: " << bgcios::errorString(err));
      return;
   }
   _currentGdbCommand = GdbInterrupt;
   LOG_DEBUG_MSG(_tag << "Update message for tid " << _threadIdForGeneralOps << " to interrupt process sent on tool data channel");

   return;
}

void
GdbController::term(void)
{
    // The job has been ended by an external event. Either a Ctl-C from runjob or a stop_tool. If this is a Ctl-C, the process
    // exit message will handle this. However, it is possible this is being driven by an end_tool command. If so, just send a SIGKILL to the
    // process for good measure to clean things up since we really cannot safely recover the process because we do not know what the
    // client may have done to the target process. 
    // 
    // Simulate the gdb kill process command to terminate the process.
    std::string killCommand = "k";
    std::string unused;
    killProcess(killCommand, unused);
    return;
}

void
GdbController::getBinaryData(std::string& src, unsigned char *dst)
{
   uint32_t dstpos = 0;
   for (uint32_t srcpos = 0; srcpos < src.size(); ++srcpos, ++dstpos) {
      unsigned char byte = src.at(srcpos);
      if (byte == 0x7d) { // Escape character, next character is actual data
         ++srcpos;
         byte = src.at(srcpos);
         LOG_DEBUG_MSG("escaped byte " << std::hex << (int)byte);
         byte |= 0x20; // Special value that encodes original character
         LOG_DEBUG_MSG("modified byte " << std::hex << (int)byte);
      }
      dst[dstpos] = byte;
   }
   return;
}

void
GdbController::putBinaryData(unsigned char *src, uint32_t length, std::ostringstream& dst)
{
   for (uint32_t srcpos = 0; srcpos < length; ++srcpos) {
      unsigned char byte = src[srcpos];
      if ((byte == 0x23) || (byte == 0x24) || (byte == 0x2a) || (byte == 0x7d)) {
         dst << 0x7d;
         byte ^= 0x20;
         dst << byte;
         LOG_DEBUG_MSG("modified byte " << std::hex << (int)byte);
      }
      dst << byte;
   }
   _gdbChannel->setNextPacketBinary();
   LOG_DEBUG_MSG("binary data source length " << length << " dest length " << dst.str().size());
   return;
}

void
GdbController::putConsoleOutput(std::string output)
{
   // Build the repsonse to the gdb client.
   std::ostringstream response;
   response << "O";
   for (size_t index = 0; index < output.size(); ++index) {
      response << std::hex << std::setw(2) << std::setfill('0') << (int)output.at(index);
   }
   LOG_DEBUG_MSG(_tag << "console output: " << response.str());

   // Send the response to the gdb client.
   std::string command = response.str();
   _gdbChannel->makeCommand(command);
   _gdbChannel->putCommand(command);

   return;
}

int
GdbController::makeGdbChannel(void)
{
   LOG_DEBUG_MSG("waiting for gdb channel connection ...");
   _gdbChannel = GdbCommandChannelPtr(new GdbCommandChannel(_jobId, _rank));
   InetSocketPtr channel = std::tr1::static_pointer_cast<InetSocket>(_gdbChannel);

   try {
      _gdbListener->accept(channel);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error accepting new gdb channel connection: " << e.what());
      return e.errcode();
   }
   LOG_DEBUG_MSG("got gdb channel connection from " << _gdbChannel->getPeerName());

   return 0;
}

uint32_t
GdbController::initCommand(struct CommandDescriptor *desc, uint16_t type, uint32_t length, uint32_t offset)
{
   desc->type = type;
   desc->reserved = 0;
   desc->length = length;
   desc->offset = offset;
   desc->returnCode = 0;
   memset((char *)_outboundMessage + offset, 0x00, length);
   return (offset + length);
}

void *
GdbController::initMessage(uint16_t type, size_t length)
{
   bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)_outboundMessage;
   memset(msghdr, 0x00, length);
   msghdr->service = bgcios::ToolctlService;
   msghdr->type = type;
   msghdr->version = ProtocolVersion;
   msghdr->rank = _rank;
   msghdr->sequenceId = _sequenceId++;
   msghdr->length = (uint32_t)length;
   msghdr->jobId = _jobId;
   return (void *)msghdr;
}

std::string
GdbController::commandErrorResponse(uint32_t returnCode)
{
   int errnum = 0;
   switch (returnCode) {
      case CmdTIDinval:
         errnum = ESRCH;
         break;

      case CmdInval:
         errnum = ENOTSUP;
         break;

      case CmdTimeout:
         errnum = ETIMEDOUT;
         break;

      case CmdAllocErr:
      case CmdMemUnavail:
         errnum = ENOMEM;
         break;

      case CmdParmErr:
      case CmdBrkptFail:
      case CmdAddrErr:
      case CmdLngthErr:
      case CmdConflict:
      case CmdPrevCmdFail:
         errnum = EINVAL;
         break;

      case CmdHwdUnavail:
      case CmdPendingNotify:
      case CmdProcessExiting:
         errnum = EBUSY;
         break;

      case CmdFileNotFound:
         errnum = ENOENT;
         break;
   }

   return _gdbChannel->errorResponse(errnum);
}

