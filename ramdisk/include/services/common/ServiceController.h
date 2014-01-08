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

//! \file  ServiceController.h
//! \brief Declaration and inline methods for bgcios::ServiceController class.

#ifndef COMMON_SERVICECONTROLLER_H
#define COMMON_SERVICECONTROLLER_H

// Includes
#include "LocalSocket.h"
#include "InetSocket.h"
#include <openssl/blowfish.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <ramdisk/include/services/IosctlMessages.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/ToolctlMessages.h>
#include <ramdisk/include/services/common/Cioslog.h>

// Control whether 
#define DISABLE_DEBUG_TRACE_INFO_WARNING

namespace bgcios
{

//! \brief Base class for I/O service message handlers.

class ServiceController
{
public:

   inline std::string getIosdCmdChannelPath(){return _iosdCmdChannelPath;}

   //! \brief  Default constructor.

   ServiceController();

   //! \brief  Default destructor.

   virtual ~ServiceController();

   //! \brief  Get the path to the command channel.
   //! \return Path name string.

   std::string getCommandChannelPath(void) const { return _cmdChannel->getName(); }

   //! \brief  Get the path to the work directory.
   //! \return Path name string.

   std::string getWorkDirectory(void) const { return _workDirectory; }

   //! \brief  Check if simulation mode is enabled.
   //! \return True if simulation mode is enabled, false if running on hardware.

   virtual bool isSimulation(void) const { return false; }

   //! \brief  Check if running on hardware.
   //! \return True if running on hardware, false if simulation mode is enabled.

   virtual bool isHardware(void) const { return true; }

   //! \brief  Check if Terminate message has been received.
   //! \return True if Terminate message has been received, otherwise false.

   bool isTerminated(void) const { return _terminated; }

   //! \brief  Send Ready message to iosd.
   //! \param  serviceId Unique service id.
   //! \param  port for inbound connections
   //! \param  cmdChannelPath to receiver of message
   //! \param  destination service
   //! \return 0 when successful, errno when unsuccessful.

   int sendReady(uint32_t serviceId, in_port_t port, std::string cmdChannelPath,uint8_t service);

   //! \brief  Complete a socket with iosd (for jobctld and stdiod)
   //! \param  serviceId Unique service id.
   //! \return 0 when successful, errno when unsuccessful.

   //int iosdSocketReady(uint32_t serviceId);


   //! \brief  Make a new data channel by accepting a connection.
   //! \return socket when successful, null pointer when unsuccessful.

   InetSocketPtr makeDataChannel(void);

   //! \brief  Send a message to the data channel.
   //! \param  buffer Pointer to buffer containing message.
   //! \return 0 when successful, errno when unsuccessful.

   int sendToDataChannel(void *buffer, InetSocketPtr channel = InetSocketPtr())
   {
      const SocketPtr& socket = std::tr1::static_pointer_cast<Socket>(
              channel ? channel : _dataChannel
              );
      CIOSLOGMSG(DTA_SEND_MSG,buffer);
      return sendMessageToStream(socket, buffer);
   }

   //! \brief  Receive a message from the data channel.
   //! \param  buffer Pointer to buffer for storing message.
   //! \param  channel data channel
   //! \return 0 when successful, errno when unsuccessful.

   int recvFromDataChannel(void *buffer, const InetSocketPtr& channel)
   {
      SocketPtr socket = std::tr1::static_pointer_cast<Socket>(channel);
      return recvMessageFromStream(socket, buffer);
   }

   //! \brief  Send a message to the specified command channel.
   //! \param  dest Path to destination command channel.
   //! \param  buffer Pointer to buffer containing message.
   //! \return 0 when successful, errno when unsuccessful.

   int sendToCommandChannel(const std::string& dest, void *buffer);

   //! \brief  Receive a message from the command channel.
   //! \param  source Path to source command channel that message was received from.
   //! \param  buffer Pointer to buffer for storing message.
   //! \return 0 when successful, errno when unsuccessful.

   int recvFromCommandChannel(std::string& source, void *buffer);

   //! \brief  Receive a message from the command channel.
   //! \param  buffer Pointer to buffer for storing message.
   //! \return 0 when successful, errno when unsuccessful.

   int recvFromCommandChannel(void *buffer)
   {
      std::string source;
      return recvFromCommandChannel(source, buffer);
   }

   //! \brief  Send an ErrorAck message to the specified command channel.
   //! \param  dest Path to destination command channel.
   //! \param  returnCode Result of previous request.
   //! \param  errorCode Error detail of previous request.
   //! \return 0 when successful, errno when unsuccessful.

   int sendErrorAckToCommandChannel(std::string dest, uint32_t returnCode, uint32_t errorCode);

   //! \brief  Send an ErrorAck message to the data channel.
   //! \param  returnCode Result of previous request.
   //! \param  errorCode Error detail of previous request.
   //! \return 0 when successful, errno when unsuccessful.

   int sendErrorAckToDataChannel(uint32_t returnCode, uint32_t errorCode);

   //! \brief  Get the number of online cpus on the system.
   //! \return Number of online cpus.

   size_t getNumCpus(void) const { return sysconf(_SC_NPROCESSORS_ONLN); }

   //! \brief  Pin this thread to the specified cpu (and any threads started by this thread).
   //! \param  cpu Number of cpu to be pinned to.
   //! \return 0 when successful, errno when unsuccessful.

   int pinToCpu(size_t cpu) const;

   //! \brief  Pin this thread to the specified cpu range (and any threads started by this thread).
   //! \param  firstCpu Number of first cpu in the range.
   //! \param  lastCpu Number of last cpu in the range.
   //! \return 0 when successful, errno when unsuccessful.

   int pinToCpuRange(size_t firstCpu, size_t lastCpu) const;

   //! \brief  Prevent this thread from running on the specified cpu (and any threads started by this thread).
   //! \param  cpu Number of cpu to stay off.
   //! \return 0 when successful, errno when unsuccessful.

   int stayOffCpu(size_t cpu) const;

   //! \brief  Log the list of cpus this thread has affinity to.
   //! \return Nothing.

   void logAffinity(void) const;

   //! \brief  Get version string.
   //! \param  name Name of daemon.
   //! \param  version Protocol version number.
   //! \return Version string.

   const std::string getVersionString(const std::string name, int version) const;

protected:

   //! \brief  Send a message to a stream socket.
   //! \param  socket Stream socket to send message to.
   //! \param  buffer Pointer to buffer containing message.
   //! \return 0 when successful, errno when unsuccessful.

   int sendMessageToStream(const SocketPtr& socket, void *buffer);

   //! \brief  Receive a message from a stream socket.
   //! \param  socket Stream socket to receive message from.
   //! \param  buffer Pointer to buffer for storing message.
   //! \return 0 when successful, errno when unsuccessful.

   int recvMessageFromStream(const SocketPtr& socket, void *buffer);

   //! \brief  Generate an encryption key from the security key in the personality.
   //! \param  bfkey Pointer to Blowfish encryption key.
   //! \return 0 when successful, errno when unsuccessful.

   int getEncryptionKey(BF_KEY *bfkey);

   //! \brief  Dump a buffer in hexadecimal format to a string.
   //! \param  label Label to identify buffer.
   //! \param  ptr Pointer to buffer to dump.
   //! \param  length Length of buffer.
   //! \return Output string with formatted data.

   std::string dumpHex(std::string label, void *ptr, uint32_t length);

   //! Command channel socket.
   LocalDatagramSocketPtr _cmdChannel;

   //! Listening data channel socket.
   InetStreamSocketPtr _dataListener;

   //! Connected data channel socket.
   InetSocketPtr _dataChannel;

   //! Path to work directory.
   std::string _workDirectory;

   //! Path to iosd command channel socket.
   std::string _iosdCmdChannelPath;

   //! Flag to indicate event monitor is done.
   volatile int _done;

   //! Flag to indicate Terminate message has been received.
   volatile int _terminated;

   //! Storage for inbound message from data or command channels.
   char *_inboundMessage;

   //! Storage for outbound message from data or command channels.
   char *_outboundMessage;

};

} // namespace bgcios

#endif // COMMON_SERVICECONTROLLER_H


