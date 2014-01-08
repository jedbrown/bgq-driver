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

// Include this file only.  It gets you everything.

#ifndef _SOCKETTYPES_H
#define _SOCKETTYPES_H

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
    class UDPSocket;
    class ListeningSocket;
    class LocalDatagramSocket;

    // Sets
    class FileSet;
    class SocketSet;
    class ListenerSet;
    class TCPSocketSet;
    class PollingTCPSocket;
    class PollingListenerSet;
    class EpollingListenerSet;
    class PollingSocketSet;
    class PollingTCPSocketSet;
    class PollingSecureTCPSocketSet;
    class PollingFileSet;
    class EpollingFileSet;
    class EpollingSocketSet;
    class EpollingTCPSocketSet;

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
    typedef std::tr1::shared_ptr<UDPSocket> UDPSocketPtr;
    typedef std::tr1::shared_ptr<PollingSocketSet> PollingSocketSetPtr;
    typedef std::tr1::shared_ptr<PollingFileSet> PollingFileSetPtr;
    typedef std::tr1::shared_ptr<EpollingFileSet> EpollingFileSetPtr;
    typedef std::tr1::shared_ptr<PollingTCPSocketSet> PollingTCPSocketSetPtr;
    typedef std::tr1::shared_ptr<PollingSecureTCPSocketSet> PollingSecureTCPSocketSetPtr;
    typedef std::tr1::shared_ptr<EpollingSocketSet> EpollingSocketSetPtr;
    typedef std::tr1::shared_ptr<EpollingTCPSocketSet> EpollingTCPSocketSetPtr;
    typedef std::tr1::shared_ptr<PollingListenerSet> PollingListenerSetPtr;
    typedef std::tr1::shared_ptr<EpollingListenerSet> EpollingListenerSetPtr;
    typedef std::tr1::shared_ptr<LocalDatagramSocket> LocalDatagramSocketPtr;
    typedef boost::shared_ptr<bgq::utility::UserId> UserIdPtr;
    typedef std::stringstream Message;
    typedef std::map<TCPSocketPtr, std::string>  MsgMap;
    //    typedef bgq::utility::security::AbstractSocket PolicyBase;

    //! local and remote indeces for the tuple
    enum { LOCAL = 0, REMOTE };

    enum Policy { NONE = 0, SECURE, INSECURE };

    enum Level { CLEAR = 0, CERTIFICATE, CRYPTUID, CRYPTALL };

    //! \brief Which side to poll, or poll for errors
    enum PollType { RECV, SEND, ERROR };
    
    //! \brief Type of user for a secure socket.
    enum UserType { Administrator, Normal };

    //! \brief Default poll timeout
    static const int POLL_TIME = 50;

    //    static void setProperties(bgq::utility::Properties::Ptr p);

    //! Address/Port Tuples...
    typedef std::tr1::tuple<SockAddr, SockAddr> FourTuple; // local and remote tuples

    //! \brief Global address family determination.
    //!
    //! Will take a string representing a host name or an IP address
    //! and determine if it is valid for IPv4 (AF_INET), IPv6 (AF_INET6) 
    //! or both (AF_UNSPEC).  If it is invalid, it returns -1.
    static int findFamily(const char* host);
    static int findFamily(std::string& host);

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

int CxxSockets::findFamily(std::string& host) {
    return findFamily(host.c_str());
};

int CxxSockets::findFamily(const char* host)
{
    const char* mh = host;
    if(host == NULL) 
        return -1;
    char buff[sizeof(struct in_addr)];
    bool v4 = false;
    bool v6 = false;
    if(inet_pton(AF_INET, host, buff) > 0)
        v4 = true;
    else if(inet_pton(AF_INET6, host, buff) > 0)
        v6 = true;
     
    if(v4 && v6)
        return AF_UNSPEC;
    else if(v4)
        return AF_INET;
    else if(v6)
        return AF_INET6;
    else { // It might be a name
        struct hostent* he = gethostbyname(mh);
        if(he != NULL)
            return he->h_addrtype;
    }
    return -1;
};

#include "SocketAddrs.h"
#include "CxxSocket.h"
#include "FileSet.h"
#include "SocketSet.h"
#include "EpollingFileSet.h"
#include "PollFileSet.h"
#include "EpollingSocketSet.h"
#include "PollSocketSet.h"
#include "ListenerSets.h"

namespace CxxSockets {
static inline void setProperties(bgq::utility::Properties::Ptr p) {
    File::setProperties(p);
}
};

#endif
