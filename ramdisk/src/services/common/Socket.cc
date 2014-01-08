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

//! \file  Socket.cc
//! \brief Methods for bgcios::Socket class.

// Includes
#include <ramdisk/include/services/common/Socket.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/ioctl.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

void
Socket::send(const void *buffer, size_t length)
{
   ssize_t bytesLeft = (ssize_t)length;
   char *bufp = (char *)buffer;
   while (bytesLeft > 0) {
      ssize_t nbytes = ::send(_sd, bufp, (size_t)bytesLeft, 0);
      if (nbytes < 0) {
         int err = errno;
         if (err == EAGAIN) {
            SocketTimeout e("send() timed out");
            LOG_CIOS_DEBUG_MSG(_tag << "time out sending data");
            throw e;
         }
         else {
            SocketError e(err, "send() failed");
            LOG_CIOS_DEBUG_MSG(_tag << "error sending data: " << errorString(e.errcode()));
            throw e;
         }
      }
      if (nbytes == 0) {
         SocketClosed e("send() detected close");
         LOG_CIOS_DEBUG_MSG(_tag << "close detected when sending data");
         throw e;
      }
      bytesLeft -= nbytes;
      bufp += nbytes;
   }

   LOG_CIOS_TRACE_MSG(_tag << "sent " << length << " bytes");
   return;
}

void
Socket::recv(void *buffer, size_t length)
{
   // Receive all of the data from socket.
   ssize_t bytesLeft = (ssize_t)length;
   char *bufp = (char *)buffer;
   while (bytesLeft > 0) {
      ssize_t nbytes = ::read(_sd, bufp, (size_t)bytesLeft);
      if (nbytes < 0) {
         int err = errno;
         if (err == EAGAIN) {
            SocketTimeout e("recv() timed out");
            LOG_CIOS_DEBUG_MSG(_tag << "time out receiving data");
            throw e;
         }
         else {
            SocketError e(err, "recv() failed");
            LOG_CIOS_DEBUG_MSG(_tag << "error receiving data: " << errorString(e.errcode()));
            throw e;
         }
      }
      if (nbytes == 0) {
         SocketClosed e("recv() detected close");
         LOG_CIOS_DEBUG_MSG(_tag << "close detected when receiving data");
         throw e;
      }
      bytesLeft -= nbytes;
      bufp += nbytes;
   }

   LOG_CIOS_TRACE_MSG(_tag << "received " << length << " bytes");
   return;
}

void
Socket::sendToAddr(sockaddr *addr, socklen_t addrlen, const void *buffer, size_t length)
{
   ssize_t bytesLeft = (ssize_t)length;
   char *bufp = (char *)buffer;
   while (bytesLeft > 0) {
      ssize_t nbytes = ::sendto(_sd, bufp, (size_t)bytesLeft, 0, addr, addrlen);
      if (nbytes < 0) {
         int err = errno;
         if (err == EAGAIN) {
            SocketTimeout e("sendto() timed out");
            
            LOG_ERROR_MSG(_tag << "sendto() EAGAIN mypid="<<getpid() );
            LOG_ERROR_MSG( printHeader( *(MessageHeader *) buffer) );
            sockaddr_un *destptr = (sockaddr_un *)addr;
            //sockaddr_un *destptr = reinterpret_cast<sockaddr_un *>(addr);
            LOG_ERROR_MSG( "sending to "<< ( destptr->sun_path) );
            logStackBackTrace(100);
            setSendTimeout(0); //sendto timeout in seconds--0 is no timeout
            continue; //wait until operation completes, errno, or interrupted
            //throw e;
         }
         else {
            SocketError e(err, "sendto() failed");
            LOG_CIOS_DEBUG_MSG(_tag << "error sending data: " << errorString(e.errcode()));
            throw e;
         }
      }
      if (nbytes == 0) {
         SocketClosed e("sendto() detected close");
         LOG_CIOS_DEBUG_MSG(_tag << "close detected when sending data");
         throw e;
      }
      bytesLeft -= nbytes;
      bufp += nbytes;
   }

   LOG_CIOS_TRACE_MSG(_tag << length << " bytes sent successfully");
   return;
}


void
Socket::recvFromAddr(sockaddr *addr, socklen_t *addrlen, void *buffer, size_t length)
{
   // Receive all of the data from socket.
   ssize_t bytesLeft = (ssize_t)length;
   char *bufp = (char *)buffer;
   while (bytesLeft > 0) {
      ssize_t nbytes = ::recvfrom(_sd, bufp, (size_t)bytesLeft, 0, addr, addrlen);
      if (nbytes < 0) {
         int err = errno;
         if (err == EAGAIN) {
            SocketTimeout e("recvfrom() timed out");
            LOG_CIOS_DEBUG_MSG(_tag << "time out receiving data");
            throw e;
         }
         else {
            SocketError e(err, "recvfrom() failed");
            LOG_CIOS_DEBUG_MSG(_tag << "error receiving data: " << errorString(e.errcode()));
            throw e;
         }
      }
      if (nbytes == 0) {
         SocketClosed e("recvfrom() detected close");
         LOG_CIOS_DEBUG_MSG(_tag << "close detected when receiving data");
         throw e;
      }
      bytesLeft -= nbytes;
      bufp += nbytes;
   }

   LOG_CIOS_TRACE_MSG(_tag << "received " << length << " bytes");
   return;
}

ssize_t
Socket::recvAvailable(void *buffer, size_t length)
{
   // Receive the available data from socket.
   ssize_t nbytes = ::recv(_sd, buffer, (size_t)length, 0);
   if (nbytes < 0) {
      int err = errno;
      if (err == EAGAIN) {
         SocketTimeout e("recv() timed out");
         LOG_CIOS_DEBUG_MSG(_tag << "time out receiving data");
         throw e;
      }
      else {
         SocketError e(err, "recv() failed");
         LOG_CIOS_DEBUG_MSG(_tag << "error receiving data: " << errorString(e.errcode()));
         throw e;
      }
   }
   if (nbytes == 0) {
      SocketClosed e("recv() detected close");
      LOG_CIOS_DEBUG_MSG(_tag << "close detected when receiving data");
      throw e;
   }

   LOG_CIOS_TRACE_MSG(_tag << "received " << nbytes << " bytes");
   return nbytes;
}

ssize_t
Socket::recvAvailableFromAddr(sockaddr *addr, socklen_t *addrlen, void *buffer, size_t length)
{
   // Receive the available data from socket.
   ssize_t nbytes = ::recvfrom(_sd, buffer, (size_t)length, 0, addr, addrlen);
   if (nbytes < 0) {
      int err = errno;
      if (err == EAGAIN) {
         SocketTimeout e("recvfrom() timed out");
         LOG_CIOS_DEBUG_MSG(_tag << "time out receiving data");
         throw e;
      }
      else {
         SocketError e(err, "recvfrom() failed");
         LOG_CIOS_DEBUG_MSG(_tag << "error receiving data: " << errorString(e.errcode()));
         throw e;
      }
   }
   if (nbytes == 0) {
      SocketClosed e("recvfrom() detected close");
      LOG_CIOS_DEBUG_MSG(_tag << "close detected when receiving data");
      throw e;
   }

   LOG_CIOS_TRACE_MSG(_tag << "received " << nbytes << " bytes");
   return nbytes;
}

int
Socket::getBytesAvailable(void)
{
   int nread = 0;
   if (ioctl(_sd, FIONREAD, &nread) != 0) {
      SocketError e(errno, "ioctl(FIONREAD) failed");
      LOG_ERROR_MSG(_tag << "error getting bytes available: " << errorString(e.errcode()));
      throw e;
   }
   return nread;
}

int
Socket::getRecvBufferSize(void) const
{
   int size;
   socklen_t optlen = sizeof(size);
   if (getsockopt(_sd, SOL_SOCKET, SO_RCVBUF, &size, &optlen) != 0) {
      SocketError e(errno, "getsockopt(SO_RCVBUF) failed");
      LOG_ERROR_MSG(_tag << "error getting receive buffer size: " << errorString(e.errcode()));
      throw e;
   }
   return size;
}

void
Socket::setRecvBufferSize(int size)
{
   if (setsockopt(_sd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != 0) {
      SocketError e(errno, "setsockopt(SO_RCVBUF) failed");
      LOG_ERROR_MSG(_tag << "error setting receive buffer size: " << errorString(e.errcode()));
      throw e;
   }
   return;
}

int
Socket::getSendBufferSize(void) const
{
   int size;
   socklen_t optlen = sizeof(size);
   if (getsockopt(_sd, SOL_SOCKET, SO_SNDBUF, &size, &optlen) != 0) {
      SocketError e(errno, "getsockopt(SO_SNDBUF) failed");
      LOG_ERROR_MSG(_tag << "error getting send buffer size: " << errorString(e.errcode()));
      throw e;
   }
   return size;
}

void
Socket::setSendBufferSize(int size)
{
   if (setsockopt(_sd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) != 0) {
      SocketError e(errno, "setsockopt(SO_SNDBUF) failed");
      LOG_ERROR_MSG(_tag << "error setting send buffer size: " << errorString(e.errcode()));
      throw e;
   }
   return;
}

timeval
Socket::getRecvTimeout(void) const
{
   timeval timeout;
   socklen_t optlen = sizeof(timeout);
   if (getsockopt(_sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &optlen) != 0) {
      SocketError e(errno, "getsockopt(SO_RCVTIMEO) failed");
      LOG_ERROR_MSG(_tag << "error getting receive time out: " << errorString(e.errcode()));
      throw e;
   }
   return timeout;
}

void
Socket::setRecvTimeout(timeval& timeout)
{
   if (setsockopt(_sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeval)) != 0) {
      SocketError e(errno, "setsockopt(SO_RCVTIMEO) failed");
      LOG_ERROR_MSG(_tag << "error setting receive time out: " << errorString(e.errcode()));
      throw e;
   }
   return;
}

timeval
Socket::getSendTimeout(void) const
{
   timeval timeout;
   socklen_t optlen = sizeof(timeout);
   if (getsockopt(_sd, SOL_SOCKET, SO_SNDTIMEO, &timeout, &optlen) != 0) {
      SocketError e(errno, "getsockopt(SO_SNDTIMEO) failed");
      LOG_ERROR_MSG(_tag << "error getting send time out: " << errorString(e.errcode()));
      throw e;
   }
   return timeout;
}

void
Socket::setSendTimeout(timeval& timeout)
{
   if (setsockopt(_sd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeval)) != 0) {
      SocketError e(errno, "setsockopt(SO_SNDTIMEO) failed");
      LOG_ERROR_MSG(_tag << "error setting send time out: " << errorString(e.errcode()));
      throw e;
   }
   return;
}

void
Socket::setReuseAddress(bool value)
{
   int val = value;
   if (setsockopt(_sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
      SocketError e(errno, "setsockopt(SO_REUSEADDR) failed");
      LOG_ERROR_MSG(_tag << "error setting reuse address boolean: " << errorString(e.errcode()));
      throw e;
   }
   return;
}

void
Socket::close(void)
{
   if (::close(_sd) != 0) {
      SocketError e(errno, "close() failed");
      LOG_ERROR_MSG(_tag << "error closing descriptor: " << errorString(e.errcode()));
      throw e;
   }
   _sd = Closed;
   return;
}

