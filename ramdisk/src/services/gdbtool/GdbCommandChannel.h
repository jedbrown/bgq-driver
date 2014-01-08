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

//! \file  GdbCommandChannel.h
//! \brief Declaration and inline methods for bgcios::gdbtool::GdbCommandChannel class.

#ifndef GDBTOOL_GDBCOMMANDCHANNEL_H
#define GDBTOOL_GDBCOMMANDCHANNEL_H

// Includes
#include <ramdisk/include/services/common/InetSocket.h>
#include <string>
#include <tr1/memory>
#include <stdexcept>

namespace bgcios
{

namespace gdbtool
{

//! Acknowledge command string.
const std::string AckCommand = "+";

//! Retransmit command string.
const std::string NakCommand = "-";

//! Command is unsupported response.
const std::string UnsupportedCommand = "$#00";

//! Success response string.
const std::string SuccessResponse = "OK";

//! \brief Exception for invalid gdb remote serial protocol commands.

class GdbCommandError : public std::runtime_error
{
public:

   //! \brief  Default constructor.
   //! \param  what String describing error.

   GdbCommandError(const std::string what="") : std::runtime_error(what) { }
};

//! \brief Command channel for sending and receiving gdb remote serial protocol commands.

class GdbCommandChannel : public bgcios::InetSocket
{

public:

   //! \brief  Default constructor.
   //! \param  jobId Job identifier.
   //! \param  rank Rank of compute node process the gdb client is attached to.
   //! \throws SocketError.

   GdbCommandChannel(uint64_t jobId, uint32_t rank);

   //! \brief  Default destructor.

   ~GdbCommandChannel() { destroy(); }

   //! \brief  Check if gdb remote serial protocol input is available.
   //! \return True if input is available, otherwise false.

   bool isInputAvailable(void) { return _currentChar < _bufferLength ? true : false; }

   //! \brief  Get gdb remote serial protocol input from the gdb client.
   //! \return Command string.
   //! \throws SocketError, SocketClosed, GdbCommandError.

   std::string get(void);

   //! \brief  Put gdb remote serial protocol output to the gdb client.
   //! \param  command Command string.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed.

   void put(const std::string& command);

   //! \brief  Put gdb remote serial protocol command acknowledgement to the gdb client.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed.

   void putAck(void)
   {
      if (isAckMode()) {
         put(AckCommand);
      }
      return;
   }

   //! \brief  Put gdb remote serial protocol command retransmit to the gdb client.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed.

   void putNak(void)
   {
      if (isAckMode()) {
         put(NakCommand);
      }
      return;
   }

   //! \brief  Make a gdb remote serial protocol command from a string.
   //! \param  command String to make into a command.
   //! \return Nothing.

   void makeCommand(std::string& command);

   //! \brief  Put a gdb remote serial protocol command to the gdb client.
   //! \param  command Command string.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed.

   void putCommand(std::string& command)
   {
      _lastCommand = command;
      put(command);
      return;
   }

   //! \brief  Retransmit the last gdb remote serial protocol command to the gdb client.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed.

   void putLastCommand(void);

   //! \brief  Reset the last gdb remote serial protocol command sent to the gdb client.
   //! \return Nothing.

   void resetLastCommand(void) { _lastCommand.clear(); }

   //! \brief  Get the rank of the process that the gdb client is attached to.
   //! \return Rank of process.

   uint32_t getRank(void) const { return _rank; }

   //! \brief  Set acknowledge mode attribute for the connection.
   //! \param  value New value for attribute (true to enable, false to disable).
   //! \return Nothing.

   void setAckMode(bool value) { _ackMode = value; }

   //! \brief  Check if acknowledge mode is enabled for the connection.
   //! \return True if acknowledge mode is enabled, otherwise false.

   bool isAckMode(void) { return _ackMode; }

   //! \brief  Set indicator that next packet to be sent contains binary data.
   //! \return Nothing.

   void setNextPacketBinary(void) { _isNextPacketBinary = true; }

   //! \brief  Generate a gdb remote serial protocol error response command.
   //! \param  errnum Error number value.
   //! \return Error response command string.

   std::string errorResponse(int errnum);

   //! Maximum length of a packet from the gdb client.
   static const int MaxPacketBufferLength = 1024;

private:

   //! \brief  Get a character from the packet buffer.
   //! \return Next character from buffer.
   //! \throws SocketError, SocketClosed.

   unsigned char getChar(void);

   //! \brief  Parse a gdb remote serial protocol packet.
   //! \param  packet Packet string after validation.
   //! \return Nothing.
   //! \throws GdbCommandError.

   void parsePacket(std::string& packet);

   //! Buffer for inbound packets from gdb client.
   char _packetBuffer[MaxPacketBufferLength];

   //! Length of data in packet buffer.
   uint32_t _bufferLength;

   //! Offset to current character in packet buffer.
   uint32_t _currentChar;

   //! Last command sent to gdb client (in case it needs to be retransmitted).
   std::string _lastCommand;

   //! Acknowledge mode attribute for connection (when enabled "+/-" acknowledgements are used).
   bool _ackMode;

   //! True if next packet contains binary data.
   bool _isNextPacketBinary;

   //! Job identifier.
   uint64_t _jobId;

   //! Rank of compute node process the gdb client is attached to.
   uint32_t _rank;

   //! Tag to identify this connection in trace points.
   std::string _tag;

};

//! Smart pointer for GdbCommandChannel object.
typedef std::tr1::shared_ptr<GdbCommandChannel> GdbCommandChannelPtr;

} // namespace gdbtool

} // namespace bgcios

#endif // GDBTOOL_GDBCOMMANDCHANNEL_H


