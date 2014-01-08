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

//! \file  LocalSocket.h
//! \brief Declaration for bgcios::LocalSocket and bgcios::LocalDatagramSocket classes.

#ifndef COMMON_LOCALSOCKET_H
#define COMMON_LOCALSOCKET_H

// Includes
#include "Socket.h"
#include <sys/un.h>
#include <stdint.h>
#include <tr1/memory>

namespace bgcios
{

//! AF_LOCAL socket.

class LocalSocket : public Socket
{
public:

   //! \brief  Default constructor.

   LocalSocket()
   {
      memset(&_address, 0x00, sizeof(_address));
      memset(&_peerAddress, 0x00, sizeof(_peerAddress));
   }

   //! \brief  Default destructor.

   ~LocalSocket();

   //! \brief  Connect to the specified destination socket.
   //! \param  name Path to socket.
   //! \return Nothing.
   //! \throws SocketError.

   void connect(std::string name);

   //! \brief  Make socket a peer after accepting a new connection.
   //! \param  sd Socket descriptor of new connection.
   //! \return Nothing.

   void makePeer(int sd);

   //! \brief  Get the name of the socket.
   //! \return String with path to socket.

   std::string getName(void);

   //! \brief  Get the name of the peer's socket.
   //! \return String with path to socket.

   std::string getPeerName(void);

protected:

   //! \brief  Bind a name to the socket.

   void bind(std::string name);

   //! Address of local socket.
   sockaddr_un _address;

   //! Address of connected peer local socket.
   sockaddr_un _peerAddress;

};

//! Smart pointer for LocalSocket object.
typedef std::tr1::shared_ptr<LocalSocket> LocalSocketPtr;

//! AF_LOCAL datagram socket.

class LocalDatagramSocket : public LocalSocket
{
public:

   //! \brief  Default constructor.

   LocalDatagramSocket();

   //! \brief  Constructor to create and bind a name to a local socket.
   //! \param  name Path to local socket.
   //! \throws SocketError.

   LocalDatagramSocket(std::string name);

   //! \brief  Send data to the specified destination socket.
   //! \param  name Path to destination socket.
   //! \param  buffer Pointer to buffer containing data.
   //! \param  length Number of bytes in buffer.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   void sendTo(const std::string& name, void *buffer, size_t length);

   //! \brief  Receive all data and return path to peer socket.
   //! \param  name Path to peer socket.
   //! \param  buffer Pointer to buffer for received data.
   //! \param  length Number of bytes in buffer.
   //! \return Nothing.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   void recvFrom(std::string& name, void *buffer, size_t length);

   //! \brief  Receive available data and return path to peer socket.
   //! \param  name Path to peer socket.
   //! \param  buffer Pointer to buffer for received data.
   //! \param  length Number of bytes in buffer.
   //! \return Number of bytes received into buffer.
   //! \throws SocketError, SocketClosed, SocketTimeout.

   ssize_t recvAvailableFrom(std::string& name, void *buffer, size_t length);

};

//! Smart pointer for LocalDatagramSocket object.
typedef std::tr1::shared_ptr<LocalDatagramSocket> LocalDatagramSocketPtr;

// Forward reference
class LocalStreamSocket;

//! Smart pointer for LocalStreamSocket object.
typedef std::tr1::shared_ptr<LocalStreamSocket> LocalStreamSocketPtr;

//! AF_LOCAL stream socket.

class LocalStreamSocket : public LocalSocket
{
public:

   //! \brief  Default constructor.

   LocalStreamSocket();

   //! \brief  Constructor to create and bind a name to a local socket.
   //! \param  name Path to local socket.
   //! \throws SocketError.

   LocalStreamSocket(std::string name);

   //! \brief  Prepare socket to listen for connections.
   //! \param  backlog Number of pending connections to hold in backlog.
   //! \return Nothing.
   //! \throws SocketError.

   void listen(int backlog);

   //! \brief  Accept a connection from a client.
   //! \param  socket Socket connected to client.
   //! \return Nothing.
   //! \throws SocketError.

   void accept(LocalStreamSocketPtr& socket);

};

} // namespace bgcios

#endif // COMMON_LOCALSOCKET_H

