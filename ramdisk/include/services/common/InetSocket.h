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

//! \file  InetSocket.h
//! \brief Declaration and inline methods for bgcios::InetSocket and bgcios::InetStreamSocket classes.

#ifndef COMMON_INETSOCKET_H
#define COMMON_INETSOCKET_H

// Includes
#include "Socket.h"
#include <netinet/in.h>
#include <string>
#include <tr1/memory>

namespace bgcios
{

//! AF_INET socket.

class InetSocket : public Socket
{
public:

   //! \brief  Default constructor.

   InetSocket() : Socket() { }

   //! \brief  Constructor to create socket and bind to a IPv4 address.
   //! \param  addr IPv4 address.
   //! \param  port Port number.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \throws SocketError.

   InetSocket(in_addr_t addr, in_port_t port, bool reuseAddress);

   //! \brief  Constructor to create socket and bind to a IPv6 address.
   //! \param  addr IPv6 address.
   //! \param  port Port number.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \throws SocketError.

   InetSocket(in6_addr addr, in_port_t port, bool reuseAddress);

   //! \brief  Constructor to create a socket and bind to any IPv4 or IPv6 address.
   //! \param  port Port number.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \throws SocketError.

   InetSocket(in_port_t port, bool reuseAddress);

   //! \brief  Default destructor.

   ~InetSocket() { destroy(); }

   //! \brief  Destroy socket.
   //! \return Nothing.

   void destroy(void);

   //! \brief  Connect to the specified destination socket.
   //! \param  addr IPv4 address of destination.
   //! \param  port Port number.
   //! \return Nothing.
   //! \throws SocketError.

   void connect(in_addr_t addr, in_port_t port);

   //! \brief  Connect to the specified destination socket.
   //! \param  addr IPv4 address string of destination in dotted decimal format.
   //! \param  port Port number string.
   //! \return Nothing.
   //! \throws SocketError.

   void connect(std::string addr, std::string port);

   //! \brief  Make socket a peer after accepting a new connection.
   //! \param  sd Socket descriptor of new connection.
   //! \return Nothing.

   void makePeer(int sd);

   //! \brief  Set TCP no delay socket option (i.e. send all data immediately).
   //! \param  value True to enable option, false to disable option.
   //! \return Nothing.
   //! \throws SocketError.

   void setNoDelay(bool value);

   //! \brief  Get the port number of the socket.
   //! \return Port number.

   in_port_t getPort(void) const
   { 
      sockaddr *addr = (sockaddr *)&_myAddress;
      if (addr->sa_family == AF_INET6) {
         return ((sockaddr_in6 *)&_myAddress)->sin6_port;
      }
      return ((sockaddr_in *)&_myAddress)->sin_port;
   }

   //! \brief  Get the name of the socket.
   //! \return String with IPv4 address in dotted decimal format and port number.

   std::string getName(void) { return addrToString(((sockaddr *)&_myAddress)); }

   //! \brief  Get the name of the peer's socket.
   //! \return String with IPv4 address in dotted decimal format and port number.

   std::string getPeerName(void) { return addrToString((sockaddr *)&_peerAddress); }

protected:

   //! \brief  Create a socket and bind to an address.
   //! \param  address Address of socket.
   //! \param  addrLength Length of socket address.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \return Nothing.
   //! \throws SocketError.

   void create(sockaddr *address, socklen_t addrLength, bool reuseAddress);

   //! \brief  Convert inet address to string.
   //! \param  addr Address of inet socket.
   //! \return String with address in dotted decimal format and port number.

   std::string addrToString(sockaddr *addr);

   //! Address of inet socket.
   sockaddr_storage _myAddress;

   //! Address of connected peer inet socket.
   sockaddr_storage _peerAddress;
};

//! Smart pointer for InetSocket object.
typedef std::tr1::shared_ptr<InetSocket> InetSocketPtr;

//! AF_INET stream socket.

class InetStreamSocket : public InetSocket
{
public:

   //! \brief  Default constructor.

   InetStreamSocket() : InetSocket() { }

   //! \brief  Constructor to create socket and bind to a IPv4 address.
   //! \param  addr IPv4 address.
   //! \param  port Port number.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \throws SocketError.

   InetStreamSocket(in_addr_t addr, in_port_t port, bool reuseAddress) : InetSocket(addr, port, reuseAddress) { }

   //! \brief  Constructor to create socket and bind to a IPv6 address.
   //! \param  addr IPv6 address.
   //! \param  port Port number.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \throws SocketError.

   InetStreamSocket(in6_addr addr, in_port_t port, bool reuseAddress) : InetSocket(addr, port, reuseAddress) { }

   //! \brief  Constructor to create a socket and bind to any IPv4 or IPv6 address.
   //! \param  port Port number.
   //! \param  reuseAddress True to set reuse address socket option.
   //! \throws SocketError.

   InetStreamSocket(in_port_t port, bool reuseAddress) : InetSocket(port, reuseAddress) { }

   //! \brief  Default destructor.

   ~InetStreamSocket() { destroy(); }

   //! \brief  Prepare socket to listen for connections.
   //! \param  backlog Number of pending connections to hold in backlog.
   //! \return Nothing.
   //! \throws SocketError.

   void listen(int backlog);

   //! \brief  Accept a connection from a client.
   //! \param  socket Socket connected to client.
   //! \return Nothing.
   //! \throws SocketError.

   void accept(InetSocketPtr& socket);

};

//! Smart pointer for InetStreamSocket object.
typedef std::tr1::shared_ptr<InetStreamSocket> InetStreamSocketPtr;

} // namespace bgcios

#endif // COMMON_INETSOCKET_H

