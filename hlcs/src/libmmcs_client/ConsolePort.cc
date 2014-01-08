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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "ConsolePort.h"

#include <utility/include/cxxsockets/exception.h>
#include <utility/include/cxxsockets/ListenerSet.h>
#include <utility/include/cxxsockets/SockAddr.h>
#include <utility/include/cxxsockets/SockAddrList.h>
#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/TCPSocket.h>

#include <boost/foreach.hpp>

#include <iostream>
#include <sstream>
#include <errno.h>
#include <assert.h>
#include <poll.h>

#define RCVBUF_SIZE 4096

LOG_DECLARE_FILE("mmcs_client");

namespace mmcs_client {

ConsolePort::ConsolePort(
        const CxxSockets::TCPSocketPtr& sock
        ) :
    _sock(sock)
{
    // Nothing to do
}

void
ConsolePort::sendMessage(
        std::string& m
        ) const
{
    if (m.length() == 0 || m[m.length()-1] != '\n')
        m.append("\n");
    CxxSockets::Message msg;
    msg << m << std::ends;
    write(msg);
}

int
ConsolePort::procMessage(
        std::string& m,
        char* buf,
        bool& done,
        const int rcvsz
        )
{
    // make cr/lf just a newline
    int i, j;
    bool nullTerm = false;

    // This spins through the buffer until we
    // either run through all of it, find a
    // null terminator (\0), or find a new line (\n).
    // If any carriage returns (\r) are found, make
    // them new lines (\n).
    // j is the counter of characters that are not \n
    // and not \r
    for ( i=0, j=0; i < rcvsz; ++i ) {
        // See if it's a magic probe message
        if (buf[i] == '*' && buf[i+1] == '~' && buf[i+2] == '@'
            && buf[i+3] == '~' && buf[i+4] == '*') {
            bzero(buf, sizeof(buf));
            done = false;
            nullTerm = false;
            break;
        }

        if (buf[i] == '\0') {
            done = true;        // end of buffer
            nullTerm = true;    // end of reply
            break;
        }

        if (buf[i] != '\r') {
            // We have to be a char other than \r
            // to get here.  If it =is= a \r, then
            // we don't hit this code and we don't ++j.
            // So the next time through j < i.
            // This means that we overwrite
            // the \r with the current char and continue
            // to compress back through the entire buffer.
            if (j < i) {
                buf[j]=buf[i];
            }
            ++j;
        }

        if (buf[i] == '\n') {
            // We are going to finish iterating the buffer,
            // but this signals the caller that we're not
            // null termed so there's more in the total message.
            done = true;
        }
    }

    if (j > 0) {
        m.append(buf,j);
        bzero(buf, RCVBUF_SIZE);
    }

    return nullTerm;
}

void
ConsolePort::checkConnection() const
{
    static const CxxSockets::Message ping( "*~@~*\n" );
    try {
        if (_sock)
            this->write(ping);
    } catch (const CxxSockets::SoftError& a) {
        return;
    } catch (const CxxSockets::HardError& b) {
        throw ConsolePort::Error(b.errcode, b.what());
    } catch (const CxxSockets::CloseUnexpected& c) {
        throw ConsolePort::Error(c.errcode, c.what());
    }

    return;
}

int
ConsolePort::receiveMessage(
        std::string& m
        )
{
    bool nullTerm = false;
    bool done = false;
    char buf[RCVBUF_SIZE];      // initially allocate a 4K buffer for receiving messages


    while (!done) {
        bzero(buf, RCVBUF_SIZE);
        m.clear();            // clear out the message
        CxxSockets::Message msg;
        try {
            boost::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>(_sock)->Receive(msg);
        } catch (const CxxSockets::SoftError& a) {
            continue;
        } catch (const CxxSockets::HardError& b) {
            throw ConsolePort::Error(b.errcode, b.what());
        } catch (const CxxSockets::CloseUnexpected& c) {
            throw ConsolePort::Error(c.errcode, c.what());
        }

        if (msg.str().length() == 0) {
            return -1;
        }

        if (msg.str().find("*~@~*") != std::string::npos) {
            continue;  // Probe.  We don't care.
        }

        strncpy(buf, msg.str().c_str(), msg.str().length());
        nullTerm = procMessage(m, buf, done, msg.str().length());
    }
    return nullTerm;
}

int
ConsolePort::pollReceiveMessage(
        std::string& m,
        const unsigned timeout
        )
{
    char buf[RCVBUF_SIZE];      // initially allocate a 4K buffer for receiving messages
    bool done = false;
    bool nullTerm = false;

    struct pollfd pfd;
    pfd.fd = _sock->getFileDescriptor();
    pfd.events = 0;
    pfd.events |= POLLIN;
    pfd.events |= POLLPRI;
    m.clear();

    while (!done) {
        bzero(buf, RCVBUF_SIZE);
        try {
            int rc = poll( &pfd, 1, timeout * 1000);
            if ( rc < 0 ) {
                char buf[256];
                LOG_DEBUG_MSG( "poll: " << strerror_r(errno, buf, sizeof(buf)) );
                if ( errno == EINTR ) {
                    throw CxxSockets::SoftError(errno, buf);
                } else {
                    throw CxxSockets::HardError(errno, buf);
                }
            } else if ( rc == 0 ) {
                return 0;
            } else {
                // fall through, descriptor has data
            }

            CxxSockets::Message msg;
            rc = boost::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>(_sock)->Receive(msg);
            if ( rc == 0 )
                return -1;

            strncpy(buf, msg.str().c_str(), RCVBUF_SIZE);
            nullTerm = procMessage(m, buf, done, rc);
            if (msg.str().find("*~@~*") != std::string::npos) {
                return 0; // Probe.  Treat it as an poll timeout.
            }
        } catch ( const CxxSockets::Error& e ) {
            throw ConsolePort::Error(e.errcode, e.what());
        }
    }

    return nullTerm;
}

void
ConsolePort::write(
        const CxxSockets::Message& msg
        ) const
{
    try {
        boost::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>(_sock)->Send(msg);
    } catch (const CxxSockets::SoftError& a) {
        throw ConsolePort::Error(a.errcode, a.what());
    } catch (const CxxSockets::HardError& b) {
        throw ConsolePort::Error(b.errcode, b.what());
    } catch (const CxxSockets::CloseUnexpected& c) {
        throw ConsolePort::Error(c.errcode, c.what());
    }
}

ConsolePort::~ConsolePort()
{

}

ConsolePortClient::ConsolePortClient(
        const bgq::utility::ClientPortConfiguration& port_config
        ) :
        ConsolePort()
{
    bool failed = true;

    BOOST_FOREACH(const bgq::utility::PortConfiguration::Pair& portpair, port_config.getPairs()) {
        const std::string &connected_host = portpair.first;
        const std::string &connected_port = portpair.second;
        unsigned int attempts = 0;

        // Create a socket.
        try {
            const CxxSockets::SockAddrList remote_list(AF_UNSPEC, connected_host, connected_port);
            BOOST_FOREACH(const CxxSockets::SockAddr& remote, remote_list) {
                const CxxSockets::SecureTCPSocketPtr sock( new CxxSockets::SecureTCPSocket(remote.family(), 0) );

                try {
                    ++attempts;
                    sock->Connect(remote, port_config);
                    _sock = sock;
                    failed = false;
                    if (attempts > 1) // Let 'em know the nth try succeeded
                        LOG_INFO_MSG("Connected to mmcs_server successfully.");
                    break;
                } catch (const CxxSockets::Error& e) {
                    if (attempts < (remote_list.size() * port_config.getPairs().size())) {
                        LOG_INFO_MSG("Connect attempt failed to " << connected_host
                                << ":" << connected_port << " " << e.what()
                                << ". Will try other port pairs.");
                    }
                }
            }
        } catch (const CxxSockets::Error& e) {
            throw ConsolePort::Error(e.errcode, e.what());
        }

        if ( ! failed )
            break;
    }

    if (failed) {
        std::ostringstream msg;
        msg << "Failed to connect. " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        throw ConsolePort::Error(errno, msg.str().c_str());
    }
}

ConsolePortServer::ConsolePortServer(
        const bgq::utility::PortConfiguration::Pairs& portpairs
        )
{
    using namespace CxxSockets;
    SockAddrList masterlist; // One big list to rule them all!
    BOOST_FOREACH(const bgq::utility::PortConfiguration::Pair& curr_pair, portpairs) {
        const SockAddrList salist(AF_UNSPEC, curr_pair.first, curr_pair.second);
        BOOST_FOREACH(const SockAddr& curr_sockaddr, salist) { // Now copy every SockAddr in to the master list
            masterlist.push_back(curr_sockaddr);
        }
    }

    try {
        LOG_INFO_MSG("Attempting to bind and listen on " << masterlist.size() << " port pairs.");
        _listener.reset( new ListenerSet(masterlist, SOMAXCONN) );
    } catch (const Error& e) {
        LOG_ERROR_MSG(e.what());
        throw ConsolePort::Error(e.errcode, e.what());
    }
}

ConsolePortClient::ConsolePortClient(
        const CxxSockets::TCPSocketPtr& sock
        ) :
        ConsolePort(sock)
{

}

ConsolePortClient*
ConsolePortServer::accept()
{
    const CxxSockets::TCPSocketPtr newsock(
            new CxxSockets::TCPSocket
            );
    bool accepted = false;
    while (!accepted) {
        try {
            accepted = _listener->AcceptNew(newsock);
        } catch (const CxxSockets::Error& e) {
            int error = 0;
            if (e.errcode == -1)
                // SSL gives us a -1 on a failed validation
                error = ECONNABORTED;
            else
                error = e.errcode;

            throw ConsolePort::Error(error, e.what());
        }
    }

    ConsolePortClient* newConnection = new ConsolePortClient(newsock);
    return newConnection;
}

} // namespace mmcs_client
