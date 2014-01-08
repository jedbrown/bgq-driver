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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

/*!
 * \file utility/include/cxxsockets/types.h
 */

#ifndef CXXSOCKET_TYPES_H
#define CXXSOCKET_TYPES_H

#include <boost/shared_ptr.hpp>

#include <sstream>

namespace CxxSockets {

class File;
class FileLocker;
class FileSet;
class ListenerSet;
class ListeningSocket;
class SecureTCPSocket;
class SockAddr;
class SockAddrList;
class Socket;
class TCPSocket;

typedef boost::shared_ptr<File> FilePtr;
typedef boost::shared_ptr<FileSet> FileSetPtr;
typedef boost::shared_ptr<ListenerSet> ListenerSetPtr;
typedef boost::shared_ptr<ListeningSocket> ListeningSocketPtr;
typedef boost::shared_ptr<SecureTCPSocket> SecureTCPSocketPtr;
typedef boost::shared_ptr<Socket> SocketPtr;
typedef boost::shared_ptr<TCPSocket> TCPSocketPtr;

typedef std::stringstream Message;

}

#endif
