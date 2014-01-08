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

//! \file  InetSocket.cc
//! \brief Methods for bgcios::InetSocket and bgcios::InetStreamSocket classes.

// Includes
#include <ramdisk/include/services/common/InetSocket.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <ifaddrs.h>
#include <boost/lexical_cast.hpp>
#include <sstream>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

InetSocket::InetSocket(in_addr_t addr, in_port_t port, bool reuseAddress)
{
   // Build the socket address.
   sockaddr_in address;
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = addr;
   address.sin_port = port;

   // Create the socket.
   create((sockaddr *)&address, sizeof(address), reuseAddress);
}

InetSocket::InetSocket(in6_addr addr, in_port_t port, bool reuseAddress)
{
   // Build the socket address.
   sockaddr_in6 address;
   address.sin6_family = AF_INET6;
   memcpy(&address.sin6_addr, &addr, sizeof(addr));
   address.sin6_port = port;

   // Create the socket.
   create((sockaddr *)&address, sizeof(address), reuseAddress);
}

InetSocket::InetSocket(in_port_t port, bool reuseAddress)
{
   // Build the socket address.
   sockaddr_in6 address;
   memset(&address, '\0', sizeof(address));
   address.sin6_family = AF_INET6;
   memcpy(&address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
   address.sin6_port = port;

   // Create the socket.
   create((sockaddr *)&address, sizeof(address), reuseAddress);
}

void
InetSocket::create(sockaddr *address, socklen_t addrLength, bool reuseAddress)
{
   // Create the TCP socket.
   _sd = ::socket(address->sa_family, SOCK_STREAM | SOCK_CLOEXEC, 0);
   if (_sd == Closed) {
      SocketError e(errno, "socket() failed");
      LOG_ERROR_MSG("error creating inet socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << "[FD " << _sd << "] ";
   _tag = tag.str();

   // If requested, set the reuse address socket option.
   if (reuseAddress) {
      setReuseAddress(true);
   }

   // Bind the address to the socket.
   int rc = ::bind(_sd, address, addrLength);
   if (rc != 0) {
      SocketError e(errno, "bind() failed");
      ::close(_sd);
      _sd = Closed;
      LOG_ERROR_MSG(_tag << "error binding to address '" << addrToString((address)) << "': " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Get the name assigned by the system.
   socklen_t addrlen = sizeof(_myAddress);
   getsockname(_sd, (sockaddr *)&_myAddress, &addrlen);
   LOG_CIOS_TRACE_MSG(_tag << "bound to " << getName());

   return;
}

void
InetSocket::destroy(void)
{
   // Close the socket.
   LOG_CIOS_TRACE_MSG(_tag << "ready to destroy");
   if (_sd != Closed) {
      ::close(_sd);
      _sd = Closed;
      _tag.clear();
   }

   return;
}

void
InetSocket::connect(in_addr_t addr, in_port_t port)
{
   // Create the socket if needed.
   if (_sd == Closed) {
      _sd = ::socket(AF_INET, SOCK_STREAM, 0);
      if (_sd == Closed) {
         SocketError e(errno, "socket() failed");
         LOG_ERROR_MSG("error creating inet socket: " << bgcios::errorString(e.errcode()));
         throw e;
      }

      // Generate tag for trace points.
      std::ostringstream tag;
      tag << "[FD " << _sd << "] ";
      _tag = tag.str();
   }

   // Build the socket address.
   sockaddr_in address;
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = addr;
   address.sin_port = port;

   // Connect to the peer socket.
   int rc = ::connect(_sd, (sockaddr *)&address, sizeof(address));
   if (rc != 0) {
      SocketError e(errno, "connect() failed");
      LOG_ERROR_MSG(_tag << "error connecting to " << addrToString((sockaddr *)&address) << ": " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Get the peer's name.
   socklen_t addrlen = sizeof(_peerAddress);
   getpeername(_sd, (struct sockaddr *)&_peerAddress, &addrlen);
   LOG_CIOS_TRACE_MSG(_tag << "connected to " << getPeerName());

   return;
}

void
InetSocket::connect(std::string addrString, std::string portString)
{
   // Convert address from string to network address.
   in_addr_t addr;
   if (inet_pton(AF_INET, addrString.c_str(), &addr) <= 0) {
      SocketError e(errno, "failed to convert address");
      LOG_ERROR_MSG("error converting address " << addrString << ": " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Convert port from string to number.
   in_port_t port;
   try {
      port = htons(boost::lexical_cast<in_port_t>(portString));
   }
   catch (const std::invalid_argument& e) {
      SocketError se(EINVAL, "port cast is invalid");
      throw se;
   } 
   catch (const boost::bad_lexical_cast& e) {
      SocketError se(EINVAL, e.what());
      throw se;
   }
   return connect(addr, port);
}

void
InetSocket::makePeer(int sd)
{
   // Set the descriptor.
   _sd = sd;

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << "[FD " << _sd << "] ";
   _tag = tag.str();

   // Get the name assigned by the system.
   socklen_t addrlen = sizeof(_myAddress);
   getsockname(_sd, (struct sockaddr *)&_myAddress, &addrlen);

   // Get the peer's name.
   addrlen = sizeof(_peerAddress);
   getpeername(_sd, (struct sockaddr *)&_peerAddress, &addrlen);
   LOG_CIOS_TRACE_MSG(_tag << "made peer to " << getPeerName());

   return;
}

void
InetSocket::setNoDelay(bool value)
{
   int val = value;
   if (setsockopt(_sd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) != 0) {
      SocketError e(errno, "setsockopt(TCP_NODELAY) failed");
      LOG_ERROR_MSG(_tag << "error setting TCP no delay boolean: " << errorString(e.errcode()));
      throw e;
   }
   return;
}

std::string
InetSocket::addrToString(sockaddr *addr)
{
   void *sourceAddr;
   in_port_t port;
   if (addr->sa_family == AF_INET6) {
      sourceAddr = &(((sockaddr_in6 *)addr)->sin6_addr);
      port = ((sockaddr_in6 *)addr)->sin6_port;
   }
   else {
      sourceAddr = &(((sockaddr_in *)addr)->sin_addr);
      port = ((sockaddr_in *)addr)->sin_port;
   }

   char buffer[INET6_ADDRSTRLEN];
   inet_ntop(addr->sa_family, sourceAddr, buffer, INET6_ADDRSTRLEN);
   std::ostringstream name;
   name << buffer << ":" << ntohs(port);
   return name.str();
}

void
InetStreamSocket::listen(int backlog)
{
   // Listen for connections.
   int rc = ::listen(_sd, backlog);
   if (rc != 0) {
      SocketError e(errno, "listen() failed");
      LOG_ERROR_MSG(_tag << "error listening for connections: " << bgcios::errorString(e.errcode()));
      throw e;
   }
   LOG_CIOS_TRACE_MSG(_tag << "listening for connections");

   return;
}

void
InetStreamSocket::accept(InetSocketPtr& socket)
{
   // Accept a new connection.
   struct sockaddr_in addr;
   socklen_t addrlen = sizeof(addr);
   int newSd = ::accept(_sd, (struct sockaddr *)&addr, &addrlen);
   if (newSd < 0) {
      SocketError e(errno, "accept() failed");
      LOG_ERROR_MSG(_tag << "error accepting connection: " << bgcios::errorString(e.errcode()));
      throw e;
   }
   LOG_CIOS_TRACE_MSG(_tag << "accepted connection on descriptor " << newSd);

   // Initialize object for new socket.
   socket->makePeer(newSd);

   return;
}

