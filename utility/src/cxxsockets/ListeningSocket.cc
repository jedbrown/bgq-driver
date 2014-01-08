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
#include <utility/include/portConfiguration/SslConfiguration.h>
#include "cxxsockets/SocketTypes.h"

LOG_DECLARE_FILE( "utility.cxxsockets" );

using namespace CxxSockets;

ListeningSocket::ListeningSocket(SockAddr& addr, int backlog) {
    LOGGING_DECLARE_FD_MDC;
    FileLocker locker;
    LockFile(locker);
    _fileDescriptor = socket(addr.family(), SOCK_STREAM, 0);
    if(_fileDescriptor < 0) {
        std::ostringstream errmsg;
        errmsg << "socket() error: " << strerror(errno);
        LOG_ERROR_MSG(errmsg.str());
        throw CxxSockets::SockHardError(errno, errmsg.str());
    }

    CloseOnExec();
    LOG_DEBUG_MSG("constructing listener");
        
    pBind(addr);
    if(listen(_fileDescriptor, backlog) == -1) {
        std::ostringstream msg;
        msg << "listen error: " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(errno, msg.str());
    }
    _nonagle = true;
}

int ListeningSocket::Accept() {
    LOGGING_DECLARE_FD_MDC;
    SockAddr addr;
    socklen_t addrlen = sizeof(sockaddr_storage);
    int newFd = accept(_fileDescriptor, (sockaddr*)(&addr), &addrlen);
    if(newFd < 0) {
        std::ostringstream msg;
        msg << "accept error: " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        if(errno == EINTR || errno == EWOULDBLOCK 
           || errno == EAGAIN || errno == ERESTART)
            throw CxxSockets::SockSoftError(errno, msg.str());
        else
            throw CxxSockets::SockHardError(errno, msg.str());
    }

    if(_nonagle) {
        int flag = 1;
        LOG_DEBUG_MSG("Disabling nagle");
        int ret = setsockopt( newFd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if(ret < 0) {
            throw CxxSockets::SockHardError(errno, "Disabling nagle algorithm (TCP_NODELAY) failed");
        }
    }
    SockAddr my_addr;
    std::string my_host = "";
    std::string my_sport = "";
    std::string peer_addr = "";
    std::string peer_port = "";

    socklen_t sasz = sizeof(sockaddr_storage);

    if(getsockname(newFd, (sockaddr*)(&my_addr), &sasz) < 0)
        LOG_WARN_MSG("cannot get sock name " << strerror(errno));

    try {
        my_host = my_addr.getHostAddr();
        my_sport = boost::lexical_cast<std::string>(my_addr.getServicePort());
        peer_addr = addr.getHostAddr();
        peer_port = boost::lexical_cast<std::string>(addr.getServicePort());
    } catch (CxxSockets::CxxError& e) {
        LOG_DEBUG_MSG(e.what());
    }

    LOG_INFO_MSG("Accepted socket descriptor " << newFd << " " 
                 << my_host << ":" << my_sport 
                 << "::" << peer_addr << ":" << peer_port);

    return newFd;
}

bool ListeningSocket::AcceptNew(TCPSocketPtr& sock) {
    LOGGING_DECLARE_FD_MDC;
    FileLocker locker;
    LockFile(locker);
    int fd = Accept();
    if(sock) {
        sock->replaceFd(fd);
    } else {
        throw CxxSockets::SockHardError(-1, "Invalid socket passed to accept");
    }

    LOG_INFO_MSG("Accepted socket file descriptor " << fd);
    return true;
}

bool ListeningSocket::AcceptNew(SecureTCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config) {
    LOGGING_DECLARE_FD_MDC;
    FileLocker locker;
    LockFile(locker);
    TCPSocketPtr tp;

    LOG_INFO_MSG("Accepting secure socket");

    int fd = Accept();

    sock->replaceFd(fd);
    try {
        bgq::utility::SslConfiguration sslconf = port_config.createSslConfiguration();
        sock->SetupContext(sslconf);
        int rc = SSL_accept(sock->_ssl);
        if(rc != 1) {
            std::string errormsg = "SSL accept failed: " + SecureTCPSocket::printSSLError(sock->_ssl, rc);
            LOG_ERROR_MSG(errormsg);
            throw CxxSockets::SockHardError(SSL_ERROR, errormsg);
        }
        LOG_DEBUG_MSG("ssl accept successful");
        sock->ServerHandshake(port_config);
        LOG_INFO_MSG("Accepted secure socket file descriptor " << fd);
    } catch (std::runtime_error& e) {
        std::string original_error = e.what();
        std::ostringstream msg;
        CxxSockets::SockAddr sa;
        try {
            sock->getPeerName(sa);
            msg << "SSL handshake failed accepting new connection on file descriptor "
                << fd << " from  " << sa.getHostAddr() << ":" << sa.getServicePort() 
                << ". " <<  e.what();
        } catch(...) {
            // getPeerName() failed.  Socket is just plain bad so we can't log info about it.
            msg << "SSL handshake failed accepting new connection on file descriptor "
                << fd << ". " << original_error;
        }            
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(SSL_ERROR, msg.str());
    }

    return true;
}
