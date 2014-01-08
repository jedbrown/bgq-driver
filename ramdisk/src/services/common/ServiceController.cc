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

//! \file  ServiceController.cc
//! \brief Methods for bgcios::ServiceController class.

// Includes
#include <ramdisk/include/services/common/ServiceController.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/common/logging.h>
#include <utility/include/version.h>
#include <firmware/include/personality.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <iomanip>
#include <iostream>
#include <fstream>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

//! Number of bytes in each line of formatted output from dumpHex().
const uint32_t BytesPerLine = 32;

ServiceController::ServiceController()
{
   // Initialize data members.
   _done = false;
   _terminated = false;

   // Allocate storage for inbound and outbound messages.
   _inboundMessage = (char *)malloc(bgcios::SmallMessageRegionSize);
   _outboundMessage = (char *)malloc(bgcios::SmallMessageRegionSize);

   // The first core is handling interrupts and the last core is handling I/O link messages.  Stay off of both cores.
   size_t numCpus = getNumCpus();
   if (numCpus == 68)
         pinToCpuRange(4, 65);
   else
         pinToCpuRange(0, numCpus-1);
}

ServiceController::~ServiceController()
{
   free(_inboundMessage);
   free(_outboundMessage);
}

int
ServiceController::sendReady(uint32_t serviceId, in_port_t port, std::string cmdChannelPath,uint8_t service)
{
   LOG_CIOS_DEBUG_MSG("sending Ready to '" << cmdChannelPath << "' using service id " << serviceId <<" service="<<service <<" port="<<port);

   // Create a Ready message.
   bgcios::iosctl::ReadyMessage ready;
   memset(&ready, 0x00, sizeof(ready));
   ready.header.service = service;
   ready.header.version = bgcios::iosctl::ProtocolVersion;
   ready.header.type = bgcios::iosctl::Ready;
   ready.header.length = sizeof(ready);
   ready.serviceId = serviceId;
   ready.port = port;

   // Send Ready message to the specified command channel.
   try {
     CIOSLOGMSG(CMD_SEND_MSG,&ready);
      _cmdChannel->sendTo(cmdChannelPath, &ready, ready.header.length);
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error sending Ready message: " << e.what());
      return e.errcode();
   }
   LOG_CIOS_TRACE_MSG("sent Ready message to '" << cmdChannelPath << "'");

   return 0;
}


InetSocketPtr
ServiceController::makeDataChannel(void)
{
   LOG_CIOS_DEBUG_MSG("waiting for data channel connection ...");
   InetSocketPtr result(
           bgcios::InetSocketPtr(new bgcios::InetSocket())
           );
   try {
      _dataListener->accept(result);
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error accepting new data channel connection: " << e.what());
      return result;
   }
   LOG_CIOS_DEBUG_MSG("got data channel connection from " << result->getPeerName());

   return result;
}

int
ServiceController::sendToCommandChannel(const std::string& dest, void *buffer)
{
   // Check the message length.
   MessageHeader *msghdr = (MessageHeader *)buffer;
   if (msghdr->length > bgcios::SmallMessageDataSize) {
      LOG_ERROR_MSG("error sending message to command channel '" << dest << ": message length " << msghdr->length <<
                    " is greater than maximum size " << bgcios::SmallMessageDataSize);
      return EINVAL;
   }

   // Send the message.
   try {
      LOG_CIOS_DEBUG_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message for '" << dest << "' is ready on command channel, " <<
                    bgcios::returnCodeToString(msghdr->returnCode));
      CIOSLOGMSG(CMD_SEND_MSG,buffer);
      _cmdChannel->sendTo(dest, buffer, msghdr->length);
   } 
   catch (SocketError& e) {
      LOG_ERROR_MSG("error sending message to command channel '" << dest << "': " << bgcios::errorString(e.errcode()));
      return e.errcode();
   }
   catch (SocketClosed& e) {
      LOG_ERROR_MSG("command channel '" << dest << "' closed when sending message: " << e.what());
      return EPIPE;
   }
   catch (SocketTimeout& e) {
      LOG_ERROR_MSG("timeout sending message to command channel '" << dest << "': " << e.what());
      return ETIMEDOUT;
   }
   LOG_CIOS_TRACE_MSG(bgcios::toString(msghdr) << " message with length " << msghdr->length << " sent on command channel to '" << dest << "'");

   return 0;
}

int
ServiceController::recvFromCommandChannel(std::string& source, void *buffer)
{
   // Receive the message.
   LOG_CIOS_TRACE_MSG(_cmdChannel->getBytesAvailable() << " bytes available on command channel");
   MessageHeader *msghdr = (MessageHeader *)buffer;
   try {
      _cmdChannel->recvAvailableFrom(source, msghdr, bgcios::SmallMessageDataSize);
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error receiving message from command channel: " << bgcios::errorString(e.errcode()));
      return e.errcode();
   }
   catch (SocketClosed& e) {
      LOG_ERROR_MSG("command channel closed when receiving message: " << e.what());
      return EPIPE;
   }
   catch (SocketTimeout& e) {
      LOG_ERROR_MSG("timeout receiving message from command channel: " << e.what());
      return ETIMEDOUT;
   }
   LOG_CIOS_TRACE_MSG("received message with length " << msghdr->length << " on command channel from '" << source << "'");

   return 0;
}

int
ServiceController::sendMessageToStream(const SocketPtr& socket, void *buffer)
{
   // Make sure the socket is still open.
   if (socket == NULL) {
      return EPIPE;
   }

   // Send the message header and message body together.
   MessageHeader *msghdr = (MessageHeader *)buffer;
   try {
      LOG_CIOS_TRACE_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message is ready for socket connected to '" <<
                    socket->getPeerName() << "', " << bgcios::returnCodeToString(msghdr->returnCode));
      socket->send(msghdr, msghdr->length);
   } 
   catch (SocketError& e) {
      LOG_ERROR_MSG("error sending message to socket connected to '" << socket->getPeerName() << "': " << bgcios::errorString(e.errcode()));
      return e.errcode();
   }
   catch (SocketClosed& e) {
      LOG_CIOS_DEBUG_MSG("socket closed when sending message to socket connected to '" << socket->getPeerName() << "': " << e.what());
      return EPIPE;
   }
   catch (SocketTimeout& e) {
      LOG_CIOS_WARN_MSG("timeout sending message to socket connected to '" << socket->getPeerName() << "': " << e.what());
      return ETIMEDOUT;
   }
   LOG_CIOS_TRACE_MSG("Job " << msghdr->jobId << ": " << bgcios::toString(msghdr) << " message with length " << msghdr->length <<
                 " sent on socket connected to '" << socket->getPeerName() << "'");

   return 0;
}

int
ServiceController::recvMessageFromStream(const SocketPtr& socket, void *buffer)
{
   // Make sure the socket is still open.
   if (socket == NULL) {
      return EPIPE;
   }

   // Receive the message header.
   LOG_CIOS_TRACE_MSG(socket->getBytesAvailable() << " bytes available for message header on socket connected to '" << socket->getPeerName() << "'");
   MessageHeader *msghdr = (MessageHeader *)buffer;
   try {
      socket->recv(msghdr, sizeof(MessageHeader));
   }
   catch (SocketError& e) {
      LOG_ERROR_MSG("error receiving message header from socket connected to '" << socket->getPeerName() << "': " << bgcios::errorString(e.errcode()));
      return e.errcode();
   }
   catch (SocketClosed& e) {
      LOG_CIOS_DEBUG_MSG("socket closed when receiving message header from socket connected to '" << socket->getPeerName() << "': " << e.what());
      return EPIPE;
   }
   catch (SocketTimeout& e) {
      LOG_CIOS_WARN_MSG("timeout receiving message header from socket connected to '" << socket->getPeerName() << "': " << e.what());
      return ETIMEDOUT;
   }

   // Receive the message body if needed.
   if (msghdr->length > sizeof(MessageHeader)) {
      LOG_CIOS_TRACE_MSG(socket->getBytesAvailable() << " bytes available for message body on socket connected to '" << socket->getPeerName() << "'");
      char *msg = (char *)buffer + sizeof(MessageHeader);
      try {
         socket->recv(msg, bgcios::dataLength(msghdr));
      }
      catch (SocketError& e) {
         LOG_ERROR_MSG("error receiving message body from socket connected to '" << socket->getPeerName() << "': " << bgcios::errorString(e.errcode()));
         return e.errcode();
      }
      catch (SocketClosed& e) {
         LOG_CIOS_WARN_MSG("socket closed when receiving message body from socket connected to '" << socket->getPeerName() << "': " << e.what());
         return EPIPE;
      }
      catch (SocketTimeout& e) {
         LOG_CIOS_WARN_MSG("timeout receiving message body from socket connected to '" << socket->getPeerName() << "': " << e.what());
         return ETIMEDOUT;
      }
   }
   LOG_CIOS_TRACE_MSG("received message with length " << msghdr->length << " from socket connected to '" << socket->getPeerName() << "'");

   return 0;
}

int
ServiceController::sendErrorAckToCommandChannel(std::string dest, uint32_t returnCode, uint32_t errorCode)
{
   // Build ErrorAck message in outbound message region.  Each service defines an ErrorAck message in the same
   // way so we can just use the message from the iosctl service.
   bgcios::MessageHeader *inMsg = (bgcios::MessageHeader *)_inboundMessage;
   bgcios::iosctl::ErrorAckMessage *outMsg = (bgcios::iosctl::ErrorAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), inMsg, sizeof(MessageHeader));
   switch (inMsg->service) {
      case bgcios::IosctlService: outMsg->header.type = bgcios::iosctl::ErrorAck; break;
      case bgcios::JobctlService: outMsg->header.type = bgcios::jobctl::ErrorAck; break;
      case bgcios::StdioService: outMsg->header.type = bgcios::stdio::ErrorAck; break;
      case bgcios::SysioService: outMsg->header.type = bgcios::sysio::ErrorAck; break;
      case bgcios::ToolctlService: outMsg->header.type = bgcios::toolctl::ErrorAck; break;
   }
   outMsg->header.returnCode = returnCode;
   outMsg->header.errorCode = errorCode;
   outMsg->header.length = sizeof(bgcios::iosctl::ErrorAckMessage);

   // Send ErrorAck message.
   return sendToCommandChannel(dest, outMsg);
}

int
ServiceController::sendErrorAckToDataChannel(uint32_t returnCode, uint32_t errorCode)
{
   // Build ErrorAck message in outbound message region.  Each service defines an ErrorAck message in the same
   // way so we can just use the message from the iosctl service.
   bgcios::MessageHeader *inMsg = (bgcios::MessageHeader *)_inboundMessage;
   bgcios::iosctl::ErrorAckMessage *outMsg = (bgcios::iosctl::ErrorAckMessage *)_outboundMessage;
   memcpy(&(outMsg->header), inMsg, sizeof(MessageHeader));
   switch (inMsg->service) {
      case bgcios::IosctlService: outMsg->header.type = bgcios::iosctl::ErrorAck; break;
      case bgcios::JobctlService: outMsg->header.type = bgcios::jobctl::ErrorAck; break;
      case bgcios::StdioService: outMsg->header.type = bgcios::stdio::ErrorAck; break;
      case bgcios::SysioService: outMsg->header.type = bgcios::sysio::ErrorAck; break;
      case bgcios::ToolctlService: outMsg->header.type = bgcios::toolctl::ErrorAck; break;
   }
   outMsg->header.returnCode = returnCode;
   outMsg->header.errorCode = errorCode;
   outMsg->header.length = sizeof(bgcios::iosctl::ErrorAckMessage);

   // Send ErrorAck message.
   return sendToDataChannel(outMsg);
}

int
ServiceController::getEncryptionKey(BF_KEY *bfkey)
{
   // Open the personality file.
   const char *personalityPath = "/proc/device-tree/ibm,bluegene/personality/raw-data";
   std::ifstream personalityFile(personalityPath, std::ios::binary);
   if (!personalityFile) {
      LOG_ERROR_MSG("error opening personality file '" << personalityPath << "'");
      return ENOENT;
   }

   // Read the binary personality from the file.
   Personality_t personality;
   personalityFile.read((char *)&personality, sizeof(personality));
   if (!personalityFile) {
      LOG_ERROR_MSG("error reading from personality file '" << personalityPath << "'");
      return ENODATA;
   }

   std::ostringstream os;
   for ( unsigned i = 0; i < PERSONALITY_LEN_SECKEY; ++i ) {
       os << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(personality.Ethernet_Config.SecurityKey[i]);
   }
   LOG_INFO_MSG( "key: " << os.str() );

   // Generate the key using the shared secret from the secure personality.
   BF_set_key(bfkey, PERSONALITY_LEN_SECKEY, personality.Ethernet_Config.SecurityKey);

   return 0;
}

int
ServiceController::pinToCpu(size_t cpu) const
{
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(cpu, &cpuset);
   return pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}

int
ServiceController::pinToCpuRange(size_t firstCpu, size_t lastCpu) const
{
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   for (size_t cpu = firstCpu; cpu <= lastCpu; ++ cpu) {
      CPU_SET(cpu, &cpuset);
   }
   return pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}

int
ServiceController::stayOffCpu(size_t cpu) const
{
   int rc = 0;
   cpu_set_t cpuset;
   pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
   if (CPU_ISSET(cpu, &cpuset)) {
      CPU_CLR(cpu, &cpuset);
      rc = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
   }
   return rc;
}

void
ServiceController::logAffinity(void) const
{
   cpu_set_t cpuset;
   pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
   std::ostringstream cpuList;
   for (size_t cpu = 0; cpu < getNumCpus(); ++cpu) {
      if (CPU_ISSET(cpu, &cpuset)) {
         cpuList << cpu << " ";
      }
   }
   LOG_INFO_MSG_FORCED("daemon is using cpus " << cpuList.str() << "of " << getNumCpus() << " total cpus");

   return;
}

const std::string
ServiceController::getVersionString(const std::string name, int version) const
{
   std::ostringstream myVersion;
   myVersion << "BG/Q " << name << " " << bgq::utility::DriverName << " built from revision " << bgq::utility::Revision << " at " <<
      __DATE__ << " " << __TIME__ << " with protocol version " << version;
   return myVersion.str();
}

std::string
ServiceController::dumpHex(std::string label, void *ptr, uint32_t length)
{
   std::ostringstream output;
   char remainder[BytesPerLine];
   uint32_t *data = (uint32_t *)ptr;
   uint32_t offset = 0;
   output << label << ": start address " << data << " for 0x" << std::hex << length << std::dec << " (" << length << ") bytes" << std::endl;
   while (length >= BytesPerLine) {
      output << std::hex << std::setfill('0') << std::setw(4) << offset << ": " << std::setw(8) << data[0] << " " << std::setw(8) << data[1] <<
         " " << std::setw(8) << data[2] << " " << std::setw(8) << data[3] << " " << std::setw(8) << data[4] <<
         " " << std::setw(8) << data[5] << " " << std::setw(8) << data[6] << " " << std::setw(8) << data[7] << std::endl;
      length -= BytesPerLine;
      offset += BytesPerLine;
      data += BytesPerLine / sizeof(*data);
   }
   if (length > 0) {
      memset(remainder, 0, sizeof(remainder));
      memcpy(remainder, data, length);
      data = (uint32_t *)remainder;
      output << std::hex << std::setfill('0') << std::setw(4) << offset << ": " << std::setw(8) << data[0] << " " << std::setw(8) << data[1] <<
         " " << std::setw(8) << data[2] << " " << std::setw(8) << data[3] << " " << std::setw(8) << data[4] <<
         " " << std::setw(8) << data[5] << " " << std::setw(8) << data[6] << " " << std::setw(8) << data[7] << std::endl;
   }

   return output.str();
}

