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

//! \file  Socket.h
//! \brief Declaration and inline methods for bgcios::Socket, bgcios::SocketError,
//!        bgcios::SocketClosed, bgcios::SocketTimeout classes.

#ifndef COMMON_SOCKET_H
#define COMMON_SOCKET_H

// Includes
#include <stdexcept>
#include <tr1/memory>
#include <sys/socket.h>
#include <string>
#include <errno.h>

namespace bgcios
{

//! Exception for general socket errors.

class SocketError : public std::runtime_error
{
public:

   //! \brief  Default constructor.
   //! \param  err Error code value.
   //! \param  what String describing error.

   SocketError(int err=0, const std::string what="") : std::runtime_error(what), _errcode(err) { }

   int errcode(void) const { return _errcode; }

protected:

   //! Error code (typically errno from socket function).
   int _errcode;
};

//! Exception for when peer unexpectedly closes socket.

class SocketClosed : public std::runtime_error
{
public:

   //! \brief  Default constructor.
   //! \param  what String describing error.

   SocketClosed(const std::string what="") : std::runtime_error(what) { }
};

//! Exception for send or receive timeout error.

class SocketTimeout : public std::runtime_error
{
public:

   //! \brief  Default constructor.
   //! \param  what String describing error.

   SocketTimeout(const std::string what="") : std::runtime_error(what) { }
};

//! Base class for a socket.

class Socket
{

public:

   //! \brief  Default constructor.

   Socket() : _sd(Closed) { }

   //! \brief  Send specified number bytes from specified buffer on a connected socket.
   //! \param  buffer Pointer to buffer containing data.
   //! \param  length Number of bytes in buffer.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   void send(const void *buffer, size_t length);

   //! \brief  Send specified number of bytes from specified buffer to specified destination on unconnected socket.
   //! \param  dest Destination address to send to.
   //! \param  destlen Length of destination address.
   //! \param  buffer Pointer to buffer containing data.
   //! \param  length Number of bytes in buffer.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   void sendToAddr(sockaddr *dest, socklen_t destlen, const void *buffer, size_t length);

   //! \brief  Send specified number of bytes from specified buffer on connected socket.
   //! \param  buffer Pointer to buffer containing data.
   //! \param  length Number of bytes in buffer.
   //! \param  error set to errno encountered else 0.  See man page for send for errno values
   //! \param  optional flags for each of the send operations.  See man page for flag settings
   //! \return return number of bytes sent

inline
unsigned int
sendOnConnectedSocket(char *buffer, unsigned int length, unsigned int& error, int flags=0)
{
   unsigned int bytesLeft = length;
   unsigned int nbytes;
   unsigned int numBytesSent = 0;
   
   while (bytesLeft > 0) {
      nbytes = ::send(_sd, buffer, (size_t)bytesLeft, flags);
      if (nbytes > 0) {
        numBytesSent += nbytes;
        bytesLeft -= nbytes;
        buffer += nbytes;
        continue;
      }
      else if (nbytes < 0) {
         error=errno;
         break;
      }
      else //closed if nbytes==0
      {
        error=ENOTCONN;
        break;
      }      
   }
   return numBytesSent;
}

   //! \brief  Receive specified number of bytes into specified buffer.
   //! \param  buffer Pointer to buffer for received data.
   //! \param  length Number of bytes in buffer.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   void recv(void *buffer, size_t length);

   //! \brief  Receive specified number of bytes into specified buffer and return address data was received from.
   //! \param  from Peer address data was received from.
   //! \param  fromlen Length of peer address.
   //! \param  buffer Pointer to buffer for received data.
   //! \param  length Number of bytes in buffer.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   void recvFromAddr(sockaddr *from, socklen_t *fromlen, void *buffer, size_t length);

   //! \brief  Receive available bytes into specified buffer.
   //! \param  buffer Pointer to buffer for received data.
   //! \param  length Number of bytes in buffer.
   //! \return Number of bytes received into buffer.

   ssize_t recvAvailable(void *buffer, size_t length);

   //! \brief  Receive available bytes into specified buffer and return address data was received from.
   //! \param  from Peer address data was received from.
   //! \param  fromlen Length of peer address.
   //! \param  buffer Pointer to buffer for received data.
   //! \param  length Number of bytes in buffer.
   //! \return Number of bytes received into buffer.

   ssize_t recvAvailableFromAddr(sockaddr *from, socklen_t *fromlen, void *buffer, size_t length);

   //! \brief  Get the receive buffer size socket option.
   //! \return Size of receive buffer.

   int getRecvBufferSize(void) const;

   //! \brief  Set the receive buffer size socket option.
   //! \param  size Size of receive buffer.
   //! \return Nothing.
   //! \throws SocketError.

   void setRecvBufferSize(int size);

   //! \brief  Get the send buffer size socket option.
   //! \return Size of send buffer.

   int getSendBufferSize(void) const;

   //! \brief  Set the send buffer size socket option.
   //! \param  size Size of send buffer.
   //! \return Nothing.
   //! \throws SocketError.

   void setSendBufferSize(int size);

   //! \brief  Get the receive timeout socket option.
   //! \return Number of seconds and microseconds for timeout.

   timeval getRecvTimeout(void) const;

   //! \brief  Set the receive timeout socket option.
   //! \param  nseconds Number of seconds for timeout.
   //! \return Nothing.
   //! \throws SocketError.

   void setRecvTimeout(long int nseconds)
   {
      timeval timeout;
      timeout.tv_sec = nseconds;
      timeout.tv_usec = 0;
      return setRecvTimeout(timeout);
   }

   //! \brief  Set the receive timeout socket option.
   //! \param  timeout Number of seconds and microseconds for timeout.
   //! \return Nothing.
   //! \throws SocketError.

   void setRecvTimeout(timeval& timeout);

   //! \brief  Get the send timeout socket option.
   //! \return Number of seconds and microseconds for timeout.

   timeval getSendTimeout(void) const;

   //! \brief  Set the send timeout socket option.
   //! \param  nseconds Number of seconds for timeout.
   //! \return Nothing.
   //! \throws SocketError.

   void setSendTimeout(long int nseconds)
   {
      timeval timeout;
      timeout.tv_sec = nseconds;
      timeout.tv_usec = 0;
      return setSendTimeout(timeout);
   }

   //! \brief  Set the send timeout socket option.
   //! \param  timeout Number of seconds and microseconds for timeout.
   //! \return Nothing.
   //! \throws SocketError.

   void setSendTimeout(timeval& timeout);

   //! \brief  Set the reuse address socket option.
   //! \param  value True to enable option, false to disable option.
   //! \return Nothing.
   //! \throws SocketError.

   void setReuseAddress(bool value);

   //! \brief  Get the number of bytes available to be read.
   //! \return Number of bytes available.

   int getBytesAvailable(void);

   //! \brief  Get the socket descriptor.
   //! \return Socket descriptor value.

   int getSd(void) { return _sd; }

   //! \brief  Close the socket.
   //! \return Nothing.
   //! \throws SocketError.

   void close(void);

   //! \brief  Get the name of the socket.
   //! \return Name string.

   virtual std::string getName(void) { return ""; }

   //! \brief  Get the name of the peer's socket.
   //! \return Name string.

   virtual std::string getPeerName(void) { return ""; }

   //! Special value to indicate descriptor is closed.
   static const int Closed = -1;

protected:

   void setSd(int sd) { _sd = sd; }

   //! Socket descriptor.
   int _sd;

   //! Tag to identify socket in trace points.
   std::string _tag;

};

//! Smart pointer for Socket object.
typedef std::tr1::shared_ptr<Socket> SocketPtr;

} // namespace bgcios

#endif // COMMON_SOCKET_H
