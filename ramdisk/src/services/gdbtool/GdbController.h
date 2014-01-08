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

//! \file  GdbController.h
//! \brief Declaration and inline methods for bgcios::gdbtool::GdbController class.

#ifndef GDBTOOL_GDBCONTROLLER_H
#define GDBTOOL_GDBCONTROLLER_H

// Includes
#include <ramdisk/include/services/common/ServiceController.h>
#include <ramdisk/include/services/common/InetSocket.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include "GdbCommandChannel.h"
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/ToolctlMessages.h>
#include <poll.h>
#include <tr1/memory>
#include <sstream>
#include <iomanip>

using namespace bgcios::toolctl;

namespace bgcios
{

namespace gdbtool
{

//! \brief Handle gdb remote serial protocol and tool control messages.

class GdbController : public bgcios::ServiceController
{

public:

   //! \brief  Default constructor.

   GdbController();

   //! \brief  Default destructor.

   ~GdbController();

   //! \brief  Open all connections needed by the service daemon.
   //! \param  gdbChannelAddr .
   //! \param  gdbChannelPort .
   //! \param  rank Rank of compute node process the gdb client is attached to.
   //! \return 0 when successful, errno when unsuccessful.

   int startup(in_addr_t gdbChannelAddr, in_port_t gdbChannelPort, uint32_t rank);

   //! \brief  Close all connections needed by the service daemon.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanup(void);

   //! \brief  Monitor for events from all connections.
   //! \param  sigtermHandler Handler for TERM signal.
   //! \return Nothing.

   void eventMonitor(bgcios::SigtermHandler& sigtermHandler);

private:

   //! \brief  Handle events from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int commandChannelHandler(void);

   //! \brief  Handle events from tool data channel.
   //! \return 0 when successful, errno when unsuccessful.

   int toolChannelHandler(void);

   //! \brief  Handle events from gdb remote serial protocol channel.
   //! \return 0 when successful, errno when unsuccessful.

   int gdbChannelHandler(void);

   //! \brief  Make a new gdb remote serial protocol channel.
   //! \return 0 when successful, errno when unsuccessful.

   int makeGdbChannel(void);

   //! \brief  Handle a Terminate message received from command channel.
   //! \return 0 when successful, errno when unsuccessful.

   int terminate(void);

   //! \brief  Send an Attach message to the compute node.
   //! \return Nothing.

   void attach(void);

   //! \brief  Handle an AttachAck message received from completion channel.
   //! \return Nothing.

   void attachAck(void);

   //! \brief  Send a Control message to the compute node.
   //! \return Nothing.

   void control(void);

   //! \brief  Handle an ControlAck message received from completion channel.
   //! \return Nothing.

   void controlAck(void);

   //! \brief  Send an Detach message to the compute node.
   //! \param  signum Signal to deliver to compute node process.
   //! \return Nothing.

   void detach(void);

   //! \brief  Handle a DetachAck message received from completion channel.
   //! \return Nothing.

   void detachAck(void);

   //! \brief  Handle a QueryAck message received from completion channel.
   //! \return Nothing.

   void queryAck(void);

   //! \brief  Handle an UpdateAck message received from completion channel.
   //! \return Nothing.

   void updateAck(void);

   //! \brief  Handle a Notify message received from completion channel.
   //! \return Nothing.

   void notify(void);

   //! \brief  Handle the 'c' gdb command to continue a thread.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void cont(std::string& command, std::string& response);

   //! \brief  Handle the 'g' gdb command to read all registers.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void readAllRegisters(std::string& command, std::string& response);

   //! \brief  Build the response to the 'g' gdb command to read all registers.
   //! \param  inMsg Pointer to inbound QueryAck message.
   //! \param  response Response string to gdb client.
   //! \return Nothing.

   void readAllRegistersAck(QueryAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'G' gdb command to write all registers.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void writeAllRegisters(std::string& command, std::string& response);

   //! \brief  Handle the 'H' gdb command to set the thread for future operations.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void setThread(std::string& command, std::string& response);

   //! \brief  Handle the 'k' gdb command to kill a process.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void killProcess(std::string& command, std::string& response);

   //! \brief  Build the response to the kill process command
   //! \param  inMsg Pointer to inbound UpdateAck message.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void killProcessAck(UpdateAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'm' gdb command to read memory.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void readMemory(std::string& command, std::string& response);

   //! \brief  Build the response to the 'm' gdb command to read memory.
   //! \param  inMsg Pointer to inbound QueryAck message.
   //! \param  response Response string to gdb client.
   //! \return Nothing.

   void readMemoryAck(QueryAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'M' gdb command to write memory.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void writeMemory(std::string& command, std::string& response);

   //! \brief  Build the response to the 'M' gdb command to write memory.
   //! \param  inMsg Pointer to inbound UpdateAck message.
   //! \param  response Response string to gdb client.
   //! \return Nothing.

   void writeMemoryAck(UpdateAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'q' gdb command to query current state.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void generalQuery(std::string& command, std::string& response);

   //! \brief  Handle the 'qXfer:auxv:read' gdb command to get auxiliary vector.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void readAuxiliaryVector(std::string& command, std::string& response);

   //! \brief  Build the response to the 'qXfer:auxv:read' gdb command to get auxiliary vector.
   //! \param  inMsg Pointer to inbound QueryAck message.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void readAuxiliaryVectorAck(QueryAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'qfThreadInfo' gdb command to get thread list.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void threadInfo(std::string& command, std::string& response);

   //! \brief  Build the response to the 'qfThreadInfo' gdb command to get thread list.
   //! \param  inMsg Pointer to inbound QueryAck message.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void threadInfoAck(QueryAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'qThreadExtraInfo' gdb command to get thread data.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void threadExtraInfo(std::string& command, std::string& response);

   //! \brief  Build the response to the 'qThreadExtraInfo' gdb command to get thread data.
   //! \param  inMsg Pointer to inbound QueryAck message.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void threadExtraInfoAck(QueryAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'Q' gdb command to set current state.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void generalSet(std::string& command, std::string& response);

   //! \brief  Handle the 'P' gdb command to write one register.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void writeOneRegister(std::string& command, std::string& response);

   //! \brief  Build the response to the 'P' gdb command to write one register.
   //! \param  inMsg Pointer to inbound UpdateAck message.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void writeOneRegisterAck(UpdateAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 's' gdb command to step a thread.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void step(std::string& command, std::string& response);

   //! \brief  Handle the 'T' gdb command to find out if a thread is alive.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void threadAlive(std::string& command, std::string& response);

   //! \brief  Build the response to the 'T' gdb command to find out if a thread is alive.
   //! \param  inMsg Pointer to inbound QueryAck message.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void threadAliveAck(QueryAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'z' gdb command to remove a breakpoint.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void removeBreakpoint(std::string& command, std::string& response);

   //! \brief  Build the response to the 'z' gdb command to remove a breakpoint.
   //! \param  inMsg Pointer to inbound UpdateAck message.
   //! \param  response Response string to gdb client.
   //! \return Nothing.

   void removeBreakpointAck(UpdateAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Handle the 'Z' gdb command to remove a breakpoint.
   //! \param  command Command string from gdb client.
   //! \param  response Response string to gdb client (can be empty).
   //! \return Nothing.

   void insertBreakpoint(std::string& command, std::string& response);

   //! \brief  Build the response to the 'Z' gdb command to insert a breakpoint.
   //! \param  inMsg Pointer to inbound UpdateAck message.
   //! \param  response Response string to gdb client.
   //! \return Nothing.

   void insertBreakpointAck(UpdateAckMessage *inMsg, std::ostringstream& response);

   //! \brief  Send a signal to interrupt the compute node process.
   //! \return Nothing.

   void interrupt(void);

   //! \brief  Terminate after a SIGTERM signal has been delivered.
   //! \return Nothing.

   void term(void);

   //! \brief  Handle the response to any gdb command that does not need an explicit reply command.
   //! \param  inMsg Pointer to inbound UpdateAck message.
   //! \return Nothing.

   void defaultUpdateAck(UpdateAckMessage *inMsg);

   //! \brief  Get binary data from a gdb command.
   //! \param  src Source string with binary data.
   //! \param  dst Destination location for storing binary data.
   //! \return Nothing.

   void getBinaryData(std::string& src, unsigned char *dst);

   //! \brief  Put binary data in a gdb command.
   //! \param  src Source location with binary data.
   //! \param  length Length of binary data.
   //! \param  dst Destination gdb command string.
   //! \return Nothing.

   void putBinaryData(unsigned char *src, uint32_t length, std::ostringstream& dst);

   //! \brief  Put output to the console of the gdb client.
   //! \param  output Output string (should include a newline at the end).
   //! \return Nothing.

   void putConsoleOutput(std::string output);

   //! \brief  Initialize a command descriptor.
   //! \param  desc Pointer to command descriptor.
   //! \param  type Command type.
   //! \param  length Length of command data.
   //! \param  offset Offset to command in message.
   //! \return Offset for next command in message.

   uint32_t initCommand(struct CommandDescriptor *desc, uint16_t type, uint32_t length, uint32_t offset);

   //! \brief  Initialize a message in the outbound message buffer.
   //! \param  type Message type.
   //! \param  length Length of message.
   //! \return Pointer to message.

   void *initMessage(uint16_t type, size_t length);

   //! \brief  Generate a gdb remote serial protocol error response command for a command return code.
   //! \param  returnCode Tool control command return code.
   //! \return Error response command string.

   std::string commandErrorResponse(uint32_t returnCode);

   //! \brief  Send a message to the tool data channel.
   //! \param  buffer Pointer to buffer containing message.
   //! \return 0 when successful, errno when unsuccessful.

   int sendToToolChannel(void *buffer)
   {
      SocketPtr socket = std::tr1::static_pointer_cast<Socket>(_toolChannel);
      return sendMessageToStream(socket, buffer);
   }

   //! \brief  Receive a message from the tool data channel.
   //! \param  buffer Pointer to buffer for storing message.
   //! \return 0 when successful, errno when unsuccessful.

   int recvFromToolChannel(void *buffer)
   {
      SocketPtr socket = std::tr1::static_pointer_cast<Socket>(_toolChannel);
      return recvMessageFromStream(socket, buffer);
   }

   //! Supported gdb general query types.
   enum GdbQueryType
   {
      GdbQueryNone,
      GdbQueryCurrentTID,                 //!< qC command.
      GdbQueryThreadInfo,                 //!< qfThreadInfo or qsThreadInfo command.
      GdbQueryThreadExtraInfo,            //!< qThreadExtraInfo command.
      GdbQueryAuxvRead,                   //!< qXfer:auxv:read command.
   };
      
   //! Unique tool identifier.
   uint32_t _toolId;

   //! Rank of compute node process the gdb client is attached to.
   uint32_t _rank;

   //! Job identifier.
   uint64_t _jobId;

   //! Sequence id for tracking message exchanges.
   uint32_t _sequenceId;

   //! True when attached to the compute node.
   bool _attached;

   //! True when attached to an existing process (i.e. attached after the job started).
   bool _existingProcess;

   //! True when notified that compute node process has ended.
   bool _processEnded;

   //! True when gdb client has killed the process.
   bool _processKilled;

   //! True when there was an error during the attach sequence.  We'll close the connection when gdb client is ready for the bad news.
   bool _closeAfterAttached;

   //! True when a message has been retried and we are waiting for the response.
   bool _retryInProgress;

   //! Current gdb remote serial protocol command being processed.
   char _currentGdbCommand;

   //! Current gdb general query command type.
   GdbQueryType _currentQuery;

   //! Current thread in compute node process for step and continue operations (0 means select any thread).
   BG_ThreadID_t _threadIdForContinueOps;

   //! Current thread in compute node process for general operations (0 means select any thread).
   BG_ThreadID_t _threadIdForGeneralOps;

   //! Gdb remote serial protocol channel.
   GdbCommandChannelPtr _gdbChannel;

   //! Socket listening for gdb remote serial protocol connections.
   bgcios::InetStreamSocketPtr _gdbListener;  // use dataListener instead

   //! Tool control data channel connected to toolctld.
   bgcios::LocalStreamSocketPtr _toolChannel;

   //! Tag to identify this controller in trace points.
   std::string _tag;

   //! Maximum number of descriptors to monitor with poll().
   static const nfds_t MaxPollSetSize = 4;

   //! Index of command channel descriptor in poll set.
   static const int CmdChannel = 0;

   //! Index of tool channel descriptor in poll set.
   static const int ToolChannel = 1;

   //! Index of gdb client channel descriptor in poll set.
   static const int GdbChannel = 2;

   //! Index of gdb client listener descriptor in poll set.
   static const int GdbListener = 3;

   //! Set of descriptors to poll.
   struct pollfd _pollInfo[MaxPollSetSize];
   
   //! Client is waiting for a notification event
   bool _notifyPending;

};

//! Smart pointer for GdbController object.
typedef std::tr1::shared_ptr<GdbController> GdbControllerPtr;

template <class T>
T stringToHex(const std::string& s)
{
   T t;
   std::istringstream iss(s);
   iss >> std::hex >> t;
   return t;
}

} // namespace gdbtool

} // namespace bgcios

#endif // GDBTOOL_GDBCONTROLLER_H


