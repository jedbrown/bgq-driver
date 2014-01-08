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

//! \file  LocalSocket.cc
//! \brief Methods for bgcios::LocalSocket and bgcios::LocalDatagramSocket classes.

// Includes
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <errno.h>
#include <sys/stat.h>
#include <sstream>
#include <sys/socket.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

LocalSocket::~LocalSocket()
{
   // Close the socket.
   if (_sd != Closed) {
      ::close(_sd);
      _sd = Closed;
   }

   // Remove the link to the socket.
   if (SUN_LEN(&_address) != 0) {
      ::unlink(_address.sun_path);
   }
   LOG_CIOS_TRACE_MSG(_tag << "destroyed local socket");
}

void
LocalSocket::connect(std::string name)
{
   // Build the socket address.
   _peerAddress.sun_family = AF_LOCAL;
   strcpy(_peerAddress.sun_path, name.c_str());

   // Connect to the peer socket.
   int rc = ::connect(_sd, (sockaddr *)&_peerAddress, (socklen_t)SUN_LEN(&_peerAddress));
   if (rc != 0) {
      SocketError e(errno, "connect() failed");
      LOG_CIOS_DEBUG_MSG(_tag << "error connecting to '" << _peerAddress.sun_path << "': " << bgcios::errorString(e.errcode()));
      throw e;
   }
   LOG_CIOS_TRACE_MSG(_tag << "connected to " << getPeerName());

   return;
}

void
LocalSocket::makePeer(int sd)
{
   // Set the descriptor.
   _sd = sd;

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << "[FD " << _sd << "] ";
   _tag = tag.str();

   // Get the peer's name.
   socklen_t addrlen = sizeof(_peerAddress);
   getpeername(_sd, (struct sockaddr *)&_peerAddress, &addrlen);
   LOG_CIOS_TRACE_MSG(_tag << "made peer to " << getPeerName());

   return;
}

std::string
LocalSocket::getName(void)
{
   std::string name = _address.sun_path;
   return name;
}

std::string
LocalSocket::getPeerName(void)
{
   std::string name = _peerAddress.sun_path;
   return name;
}

void
LocalSocket::bind(std::string name)
{
   // Make sure the name is not too long.
   if (name.length() > sizeof(_address.sun_path)) {
      SocketError e(ENAMETOOLONG, "local socket name too long");
      LOG_ERROR_MSG("error creating local socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Remove the link to the socket.
   int rc = ::unlink(name.c_str());
   if (rc != 0) {
      SocketError e(errno, "unlink() failed");
      if (e.errcode() != ENOENT) { // Ignore the error if the socket did not exist
         LOG_ERROR_MSG("error unlinking local socket '" << name << "': " << bgcios::errorString(e.errcode()));
         throw e;
      }
   }

   // Build the socket address.
   _address.sun_family = AF_LOCAL;
   strcpy(_address.sun_path, name.c_str());

   // Bind the name to the socket.
   rc = ::bind(_sd, (struct sockaddr *)&_address, (socklen_t)SUN_LEN(&_address));
   if (rc != 0) {
      SocketError e(errno, "bind() failed");
      ::close(_sd);
      _sd = Closed;
      LOG_ERROR_MSG(_tag << "error binding to address '" << _address.sun_path << "': " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Make sure socket is accessible to all users.
   rc = ::fchmod(_sd, 0777);
   if (rc != 0) {
      SocketError e(errno, "fchmod() failed");
      ::close(_sd);
      LOG_ERROR_MSG(_tag << "error setting permissions of socket '" << _address.sun_path << "': " << bgcios::errorString(e.errcode()));
      throw e;
   }

   return;
}

LocalDatagramSocket::LocalDatagramSocket() : LocalSocket()
{
   // Create the socket.
   _sd = ::socket(AF_LOCAL, SOCK_DGRAM | SOCK_CLOEXEC, 0);
   if (_sd == Closed) {
      SocketError e(errno, "socket() failed");
      LOG_ERROR_MSG("error creating unnamed local datagram socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << "[FD " << _sd << "] ";
   _tag = tag.str();

   // Build the socket address.
   _address.sun_family = AF_LOCAL;
   _address.sun_path[0] = 0;
   LOG_CIOS_TRACE_MSG(_tag << "unnamed socket created");
}

LocalDatagramSocket::LocalDatagramSocket(std::string name) : LocalSocket()
{
   // Create the socket.
   _sd = ::socket(AF_LOCAL, SOCK_DGRAM | SOCK_CLOEXEC, 0);
   if (_sd == Closed) {
      SocketError e(errno, "socket() failed");
      LOG_ERROR_MSG("error creating named local datagram socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << name<<":[FD " << _sd << "] ";
   _tag = tag.str();

   setSendTimeout(30); //sendto timeout in seconds
   // Bind a name to the socket.
   try {
      bind(name);
   }
   catch (SocketError e) {
      LOG_ERROR_MSG(_tag << "error binding name to local datagram socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   LOG_CIOS_TRACE_MSG(_tag << "bound to " << getName());
}

void
LocalDatagramSocket::sendTo(const std::string& name, void *buffer, size_t length)
{
   // Build the destination socket address.
   sockaddr_un dest;
   dest.sun_family = AF_LOCAL;
   strcpy(dest.sun_path, name.c_str());

   // Send all of the data to the destination socket.
   sendToAddr((sockaddr *)&dest, (socklen_t)SUN_LEN(&dest), buffer, length);
   LOG_CIOS_TRACE_MSG(_tag << length << " bytes sent to '" << name << "'");

   return;
}

void
LocalDatagramSocket::recvFrom(std::string& name, void *buffer, size_t length)
{
   // Receive all of the data.
   sockaddr_un dest;
   socklen_t fromlen = sizeof(dest);
   recvFromAddr((sockaddr *)&dest, &fromlen, buffer, length);

   // Set peer socket address.
   name = dest.sun_path;
   LOG_CIOS_TRACE_MSG(_tag << length << " bytes received from '" << name << "'");

   return;
}

ssize_t
LocalDatagramSocket::recvAvailableFrom(std::string& name, void *buffer, size_t length)
{
   // Receive available data.
   sockaddr_un dest;
   socklen_t fromlen = sizeof(dest);
   ssize_t nbytes = recvAvailableFromAddr((sockaddr *)&dest, &fromlen, buffer, length);

   // Set peer socket address if there is one.
   if (SUN_LEN(&dest) > sizeof(sockaddr)) {
      name = dest.sun_path;
   }
   LOG_CIOS_TRACE_MSG(_tag << nbytes << " bytes received from '" << name << "'");

   return nbytes;
}

LocalStreamSocket::LocalStreamSocket() : LocalSocket()
{
   // Create the socket.
   _sd = ::socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
   if (_sd == Closed) {
      SocketError e(errno, "socket() failed");
      LOG_ERROR_MSG("error creating unnamed local stream socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << "[FD " << _sd << "] ";
   _tag = tag.str();

   // Build the socket address.
   _address.sun_family = AF_LOCAL;
   _address.sun_path[0] = 0;
   LOG_CIOS_TRACE_MSG(_tag << "unnamed socket created");
}

LocalStreamSocket::LocalStreamSocket(std::string name) : LocalSocket()
{
   // Create the socket.
   _sd = ::socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);
   if (_sd == Closed) {
      SocketError e(errno, "socket() failed");
      LOG_ERROR_MSG("error creating named local stream socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   // Generate tag for trace points.
   std::ostringstream tag;
   tag << "[FD " << _sd << "] ";
   _tag = tag.str();

   // Bind a name to the socket.
   try {
      bind(name);
   }
   catch (SocketError e) {
      LOG_ERROR_MSG(_tag << "error binding name to local stream socket: " << bgcios::errorString(e.errcode()));
      throw e;
   }

   LOG_CIOS_TRACE_MSG(_tag << "bound to " << getName());
}

void
LocalStreamSocket::listen(int backlog)
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
LocalStreamSocket::accept(LocalStreamSocketPtr& socket)
{
   // Accept a new connection.
   struct sockaddr_un addr;
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
