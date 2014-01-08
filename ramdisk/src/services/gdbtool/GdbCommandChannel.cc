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

//! \file  GdbCommandChannel.cc
//! \brief Methods for bgcios::gdbtool::GdbCommandChannel class.

// Includes
#include "GdbCommandChannel.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <boost/lexical_cast.hpp>
#include <iomanip>

using namespace bgcios::gdbtool;

LOG_DECLARE_FILE("cios.gdbtool");


GdbCommandChannel::GdbCommandChannel(uint64_t jobId, uint32_t rank) : InetSocket()
{
   _bufferLength = 0;
   _currentChar = 0;
   _lastCommand.clear();
   _ackMode = true;
   _isNextPacketBinary = false;
   _jobId = jobId;
   _rank = rank;

   std::ostringstream tag;
   tag << "Job " << _jobId << "." << _rank << ": ";
   _tag = tag.str();
}

std::string
GdbCommandChannel::get(void)
{
   // The first character of the packet indicates the packet type.
   std::string packet;
   unsigned char ch = getChar();
   switch (ch) {
      case '+': // ACK
      case '-': // NAK
         packet.push_back(ch);
         LOG_DEBUG_MSG(_tag << "got ack/nak packet from gdb client: '" << packet << "'");
         break;
         
      case '\003': // Interrupted
         packet.push_back(ch);
         LOG_DEBUG_MSG(_tag << "got interrupt packet from gdb client");
         break;
         
      case '$': // Command
         packet.push_back(ch);
         parsePacket(packet);
         break;

      case '%': // Notification
         packet.push_back(ch);
         parsePacket(packet);
         break;

      default:
         LOG_ERROR_MSG(_tag << "first character " << ch << " from input is invalid");
         GdbCommandError e("invalid character found at start of input");
         throw e;
         break;
   }
   
   return packet;
}


void
GdbCommandChannel::parsePacket(std::string& packet)
{
   // The '$' or '%' start of packet delimiter has already been received.  Find the '#' end of packet delimiter.
   unsigned char ch;
   while ((ch = getChar()) != '#') {
      packet.push_back(ch);
   }

   // There are two checksum characters after the end of packet delimiter.
   // We ignore the checksum since we trust TCP to deliver valid data but still need to receive the characters.
   unsigned char chksum1 = getChar();
   unsigned char chksum2 = getChar();

   if ((packet.size() > 1) && (packet.at(1) == 'X')) {
      std::ostringstream binaryPacket;
      for (size_t index = 0; index < packet.size(); ++index) {
         binaryPacket << std::hex << std::setw(2) << std::setfill('0') << (int)packet.at(index);
      }
      LOG_DEBUG_MSG(_tag << "got binary packet from gdb client: " << binaryPacket.str());
   }
   else {
      LOG_DEBUG_MSG(_tag << "got packet from gdb client: '" << packet << "#" << chksum1 << chksum2 << "'")
   }

   return;
}

unsigned char
GdbCommandChannel::getChar(void)
{
   // Get more data when the current buffer has been consumed.
   if (_currentChar == _bufferLength) {
      _bufferLength = (uint32_t)recvAvailable(_packetBuffer, MaxPacketBufferLength);
      _packetBuffer[_bufferLength] = 0;
      _currentChar = 0;
      LOG_TRACE_MSG(_tag << "received " << _bufferLength << " bytes from gdb client: '" << _packetBuffer << "'");
   }

   // Get the current character from the packet buffer.
   unsigned char ch = _packetBuffer[_currentChar];
   ++_currentChar;
   return ch;
}

void
GdbCommandChannel::put(const std::string& packet)
{
   try {
      send(packet.c_str(), packet.size());
      if (_isNextPacketBinary) {
         std::ostringstream binaryPacket;
         for (size_t index = 0; index < packet.size(); ++index) {
            binaryPacket << std::hex << std::setw(2) << std::setfill('0') << (int)packet.at(index);
         }
         LOG_DEBUG_MSG(_tag << "put binary packet with " << packet.size() << " bytes to gdb client: " << binaryPacket.str());
         _isNextPacketBinary = false;
      }
      else {
         LOG_DEBUG_MSG(_tag << "put packet with " << packet.size() << " bytes to gdb client: '" << packet << "'");
      }
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG(_tag << "error putting packet '" << packet << "' to gdb client: " << bgcios::errorString(e.errcode()));
   }
   catch (bgcios::SocketClosed& e) {
      LOG_DEBUG_MSG(_tag << "gdb client closed connection, packet '" << packet << "' was not sent");
   }
   return;
}

void
GdbCommandChannel::makeCommand(std::string& command)
{
   // Generate a checksum for the command (gdb client expects a valid checksum).
   unsigned char chksum = 0;
   for (size_t index = 0; index < command.size(); ++index) {
      unsigned char ch = command.at(index);
      chksum = (unsigned char)(chksum + ch);
   }

   // Add the '$' start of command delimiter to beginning.
   command.insert(0, 1, '$');

   // Add the '#' end of command delimiter to end.
   command.push_back('#');

   // Add the checksum characters to end.
   std::ostringstream checksum;
   checksum << std::hex << ((chksum >> 4) & 0xf) << ((chksum >> 0) & 0xf);
   command.append(checksum.str());

   return;
}

void
GdbCommandChannel::putLastCommand(void)
{
   try {
      send(_lastCommand.c_str(), _lastCommand.length());
      LOG_DEBUG_MSG(_tag << "retransmitted '" << _lastCommand << "' to gdb client");
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG(_tag << "error putting last command '" << _lastCommand << "' to gdb client: " << bgcios::errorString(e.errcode()));
   }
   catch (bgcios::SocketClosed& e) {
      LOG_DEBUG_MSG(_tag << "gdb client closed connection, last command '" << _lastCommand << "' was not sent");
   }
   return;
}

std::string
GdbCommandChannel::errorResponse(int errnum)
{
   std::ostringstream response;
   response << "E" << errnum;
   return response.str();
}


