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
 * \file SocketTypes.h
 */

// Include this file only.  It gets you everything.

#ifndef SOCKETTYPES_H
#define SOCKETTYPES_H

#include <tr1/tuple>
#include <tr1/memory>
#include <sstream>
#include <fcntl.h>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <bgq_util/include/pthreadmutex.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>
#include <log4cxx/mdc.h>

#define LOGGING_DECLARE_FD_MDC \
    log4cxx::MDC _descriptor_mdc( "FD", std::string("{") + boost::lexical_cast<std::string>(_fileDescriptor) + "} " );

#define LOGGING_DECLARE_FT_MDC \
    log4cxx::MDC _four_tuple_mdc( "FT", std::string("<") + _connection_data + "> " );


const int AF_INET6_ONLY = 21;

namespace CxxSockets {
//! Forward declares
// File and socket types
class File;
class Socket;
class SocketReceiveSide;
class SocketSendSide;
class TCPSocket;
class SecureTCPSocket;
class ListeningSocket;

// Sets
class FileSet;
class ListenerSet;
class TCPSocketSet;
class PollingTCPSocket;
class PollingListenerSet;
class PollingSocketSet;
class PollingTCPSocketSet;
class PollingSecureTCPSocketSet;
class PollingFileSet;

// Socket addrs
class SockAddr;

//! Smart pointers
typedef std::tr1::shared_ptr<File> FilePtr;
typedef std::tr1::shared_ptr<Socket> SocketPtr;
typedef std::tr1::shared_ptr<SocketReceiveSide> SocketReceiveSidePtr;
typedef std::tr1::shared_ptr<SocketSendSide> SocketSendSidePtr;
typedef std::tr1::shared_ptr<TCPSocket> TCPSocketPtr;
typedef std::tr1::shared_ptr<SecureTCPSocket> SecureTCPSocketPtr;
typedef std::tr1::shared_ptr<PollingTCPSocket> PollingTCPSocketPtr;
typedef std::tr1::shared_ptr<ListeningSocket> ListeningSocketPtr;
typedef std::tr1::shared_ptr<FileSet> FileSetPtr;
typedef std::tr1::shared_ptr<ListenerSet> ListenerSetPtr;
typedef std::tr1::shared_ptr<TCPSocketSet> TCPSocketSetPtr;
typedef std::tr1::shared_ptr<PollingSocketSet> PollingSocketSetPtr;
typedef std::tr1::shared_ptr<PollingFileSet> PollingFileSetPtr;
typedef std::tr1::shared_ptr<PollingTCPSocketSet> PollingTCPSocketSetPtr;
typedef std::tr1::shared_ptr<PollingSecureTCPSocketSet> PollingSecureTCPSocketSetPtr;
typedef std::tr1::shared_ptr<PollingListenerSet> PollingListenerSetPtr;
typedef std::stringstream Message;
typedef std::map<TCPSocketPtr, std::string>  MsgMap;

//! \brief Which side to poll, or poll for errors
enum PollType { RECV, SEND, ERROR };

//! \brief Type of user for a secure socket.
enum UserType { Administrator, Normal };

//! \brief Default poll timeout
static const int POLL_TIME = 50;

enum SockErrors { DEFAULT_ERROR = -3, SOCKET_ERROR = -2, SSL_ERROR = -1 };

//! Base CxxSockets exception class inheriting from runtime_error
class CxxError : public std::runtime_error {
public:
    int errcode;
    CxxError(int err=0, const std::string what="") : std::runtime_error(what), errcode(err) {}
};


//! \brief Socket error exception class for networking/socket api errors.
//!
//! Socket will no longer be usable when we throw this.
class SockHardError : public CxxError { 
public:
    SockHardError(int err=0, const std::string what="") : CxxError(err, what) {}
};


//! \brief Soft error exception
//!
//! Socket error exception class for networking/socket api errors that are
//! non-fatal.  EAGAIN, EWOULDBLOCK, EINTR.
class SockSoftError : public CxxError { 
public:
    SockSoftError(int err=0, const std::string what="") : CxxError(err, what) {}
};

//! \brief Socket unexpected closure
//!
//! Socket closed unexpectedly during atomic send/receive operation
class SockCloseUnexpected : public CxxError { 
public:
    SockCloseUnexpected(int err=0, const std::string what="") : CxxError(err, what) {}
};

//! \brief Exception for problems indicating bugs internal to CxxSockets
class CxxSocketInternalError : public CxxError {
public:
    CxxSocketInternalError(int err=0, const std::string what="") : CxxError(err,what) {}
};


//! \brief User error exception
//!
//! Exception for problems indicating CxxSockets usage errors like bad or
//! mismatched parameters or concurrency issues. 
class CxxSocketUserError : public CxxError {
public:
    CxxSocketUserError(int err=0, const std::string what="") : CxxError(err,what) {}
};

}

#include "SocketAddrs.h"
#include "CxxSocket.h"
#include "FileSet.h"
#include "PollFileSet.h"
#include "PollSocketSet.h"
#include "ListenerSets.h"
#include "Host.h"

#endif
