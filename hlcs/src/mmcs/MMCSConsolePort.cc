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

#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>        /* socket(), bind(), etc. */
#include <sys/fcntl.h>
#include <netinet/in.h>        /* sockaddr_in, INADDR_ANY, etc. */
#include <arpa/inet.h>        /* inet_addr() */
#include <netdb.h>        /* gethostbyname() */
#include <sys/poll.h>        /* poll() */
#include <openssl/err.h>
#include <boost/foreach.hpp>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>
#include "MMCSConsolePort.h"
#include "MMCSProperties.h"

#define RCVBUF_SIZE 4096

LOG_DECLARE_FILE("mmcs");

MMCSConsolePort::MMCSConsolePort(CxxSockets::SocketPtr& sock) :
    _sock(sock)
{

}

void
MMCSConsolePort::sendMessage(std::string& m)
{
    if (m.length() == 0 || m[m.length()-1] != '\n')
        m.append("\n");
    CxxSockets::Message msg;
    msg << m << std::ends;
    Write(msg, m.length());
    //    Write(m.c_str(), m.length());
}

int MMCSConsolePort::procMessage(std::string& m, char* buf, bool& done, bool nonblock, int rcvsz, unsigned timeout) {

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
    for ( i=0, j=0; i < rcvsz; ++i )
        {
            // See if it's a magic probe message
            if (buf[i] == '*' && buf[i+1] == '~' && buf[i+2] == '@'
                && buf[i+3] == '~' && buf[i+4] == '*') {
                bzero(buf, sizeof(buf));
                done = false;
                nullTerm = false;
                break;
            }
            if (buf[i] == '\0')
                {
                    done = true;        // end of buffer
                    nullTerm = true;    // end of reply
                    break;
                }
            if (buf[i] != '\r')
                {
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

int
MMCSConsolePort::checkConnection() {
    CxxSockets::Message msg;
    msg << "*~@~*";
    try {
        std::string m = msg.str();
        if(_sock)
            sendMessage(m);
    } catch (CxxSockets::SockSoftError& a) {
        return 0;
    } catch (CxxSockets::SockHardError& b) {
        throw MMCSConsolePort::Error(b.errcode, b.what());
    } catch (CxxSockets::SockCloseUnexpected& c) {
        throw MMCSConsolePort::Error(c.errcode, c.what());
    }
    return 0;
}

int
MMCSConsolePort::receiveMessage(std::string& m, unsigned timeout) {
    bool nullTerm = false;
    bool done = false;
    char buf[RCVBUF_SIZE];      // initially allocate a 4K buffer for receiving messages


    while(!done) {
        bzero(buf, RCVBUF_SIZE);
        m.clear();            // clear out the message
        CxxSockets::Message msg;
        try {
            if(MMCSProperties::getProperty(SECURE_CONSOLE) == "true") {
                std::tr1::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>(_sock)->Receive(msg);
            } else {
                std::tr1::dynamic_pointer_cast<CxxSockets::TCPSocket>(_sock)->Receive(msg);
            }
        } catch (CxxSockets::SockSoftError& a) {
            continue;
            throw MMCSConsolePort::Error(a.errcode, a.what());
        } catch (CxxSockets::SockHardError& b) {
            throw MMCSConsolePort::Error(b.errcode, b.what());
        } catch (CxxSockets::SockCloseUnexpected& c) {
            throw MMCSConsolePort::Error(c.errcode, c.what());
        }

        if(msg.str().length() == 0) {
            return -1;
        }

        if(msg.str().find("*~@~*") != std::string::npos) {
            //LOG_TRACE_MSG("Probe found np");
            msg.str().clear();
            continue;  // Probe.  We don't care.
        }

        strncpy(buf, msg.str().c_str(), msg.str().length());
        nullTerm = procMessage(m, buf, done, false, msg.str().length(), 0);
    }
    return nullTerm;
}

int
MMCSConsolePort::pollReceiveMessage(std::string& m, unsigned timeout)
{
    char buf[RCVBUF_SIZE];      // initially allocate a 4K buffer for receiving messages
    bool done = false;
    bool nullTerm = false;
    bool nonblock = (timeout != 0); // use nonblocking I/O when a timeout is specified

    m = "";            // clear out the message
    while (!done)
        {
            bzero(buf, RCVBUF_SIZE);
            CxxSockets::MsgMap msgmap;

            if(MMCSProperties::getProperty(SECURE_CONSOLE) == "true") {
                CxxSockets::PollingSecureTCPSocketSetPtr pollset(new CxxSockets::PollingSecureTCPSocketSet(timeout * 1000));
                pollset->AddSock(_sock);
                try {
                    if(pollset->PolledReceive(msgmap) == 0) return 0;
                } catch (CxxSockets::SockSoftError& a) {
                    throw MMCSConsolePort::Error(a.errcode, a.what());
                } catch (CxxSockets::SockHardError& b) {
                    throw MMCSConsolePort::Error(b.errcode, b.what());
                } catch (CxxSockets::SockCloseUnexpected& c) {
                    throw MMCSConsolePort::Error(c.errcode, c.what());
                }

            } else {
                CxxSockets::PollingTCPSocketSetPtr pollset(new CxxSockets::PollingTCPSocketSet(timeout * 1000));
                pollset->AddSock(_sock);
                try {
                    pollset->PolledReceive(msgmap);
                } catch (CxxSockets::SockSoftError& a) {
                    throw MMCSConsolePort::Error(a.errcode, a.what());
                } catch (CxxSockets::SockHardError& b) {
                    throw MMCSConsolePort::Error(b.errcode, b.what());
                } catch (CxxSockets::SockCloseUnexpected& c) {
                    throw MMCSConsolePort::Error(c.errcode, c.what());
                }

            }

        if(msgmap.size() == 0) {
            return -1;
        }
        CxxSockets::Message msg;

        msg << msgmap.begin()->second;
        int rcvsz = msg.str().length();
        strncpy(buf, msg.str().c_str(), RCVBUF_SIZE);
        nullTerm = procMessage(m, buf, done, nonblock, rcvsz, timeout);
        if(msg.str().find("*~@~*") != std::string::npos) {
            //LOG_TRACE_MSG("Probe found");
            msg.str().clear();
            return 0; // Probe.  Treat it as an poll timeout.
        }
    }

    return nullTerm;
}

void
MMCSConsolePort::Write(CxxSockets::Message& msg, unsigned len)
{
    try {
           if(MMCSProperties::getProperty(SECURE_CONSOLE) == "true") {
               std::tr1::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>(_sock)->Send(msg);
           } else {
               std::tr1::dynamic_pointer_cast<CxxSockets::TCPSocket>(_sock)->Send(msg);
           }
    } catch (CxxSockets::SockSoftError& a) {
        throw MMCSConsolePort::Error(a.errcode, a.what());
    } catch (CxxSockets::SockHardError& b) {
        throw MMCSConsolePort::Error(b.errcode, b.what());
    } catch (CxxSockets::SockCloseUnexpected& c) {
        throw MMCSConsolePort::Error(c.errcode, c.what());
    }
}

MMCSConsolePortClient::MMCSConsolePortClient(bgq::utility::PortConfiguration::Pairs& portpairs) :
    MMCSConsolePort()
{
    std::string connected_host = "";
    std::string connected_port = "";
    bool failed = true;
    std::ostringstream portstrings;

    BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, portpairs) {
        connected_host = portpair.first;
        connected_port = portpair.second;
        unsigned int attempts = 0;

        // Create a socket.
        if(MMCSProperties::getProperty(SECURE_CONSOLE) == "true") {
            try {
                CxxSockets::SockAddrList remote_list(AF_UNSPEC, connected_host, connected_port);
                BOOST_FOREACH(CxxSockets::SockAddr& remote, remote_list) {
                    CxxSockets::SecureTCPSocketPtr
                        sock(new CxxSockets::SecureTCPSocket(remote.family(), 0));

                    try {
                        ++attempts;
                        bgq::utility::ClientPortConfiguration port_config(0, bgq::utility::ClientPortConfiguration::ConnectionType::Command);
                        port_config.setProperties(MMCSProperties::getProperties(), "");
                        port_config.notifyComplete();
                        sock->Connect(remote, port_config);
                        _sock = sock;
                        failed = false;
                        if(attempts > 1) // Let 'em know the nth try succeeded
                            LOG_INFO_MSG("Connected to mmcs_server successfully.");
                        break;
                    } catch(CxxSockets::CxxError& e) {
                        if(e.errcode == CxxSockets::SSL_ERROR) {
                            throw MMCSConsolePort::Error(e.errcode, e.what());
                        }
                        if(attempts < (remote_list.size() * portpairs.size())) {
                            LOG_INFO_MSG("Connect attempt failed to " << connected_host
                                          << ":" << connected_port << " " << e.what()
                                          << ".  Will try other port pairs.");
                        }
                        continue;
                    }
                }
            } catch (CxxSockets::CxxError& e) {
                throw MMCSConsolePort::Error(e.errcode, e.what());
            }
        } else {
            LOG_WARN_MSG("Using insecure socket for communication.");
            CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket);

            try {
                CxxSockets::SockAddr remote(AF_UNSPEC, connected_host, connected_port);
                sock->Connect(remote);
                //                sock->setProbe(true,1,1,1);
            } catch (CxxSockets::CxxError& e) {
                throw MMCSConsolePort::Error(e.errcode, e.what());
            }
            _sock = sock;
            failed = false;
        }
    }

    if(failed) {
        std::ostringstream msg;
        msg << "Failed to connect. " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        throw MMCSConsolePort::Error(errno, msg.str().c_str());
    }
}

MMCSConsolePortClient::~MMCSConsolePortClient()
{
    // Socket should close automatically when this is destructed.
}

MMCSConsolePortServer::MMCSConsolePortServer(bgq::utility::PortConfiguration::Pairs& portpairs) {
    using namespace CxxSockets;
    SockAddrList masterlist; // One big list to rule them all!
    BOOST_FOREACH(bgq::utility::PortConfiguration::Pair& curr_pair, portpairs) {
        SockAddrList salist(AF_UNSPEC, curr_pair.first, curr_pair.second);
        BOOST_FOREACH(SockAddr& curr_sockaddr, salist) { // Now copy every SockAddr in to the master list
            masterlist.push_back(curr_sockaddr);
        }
    }

    try {
        LOG_INFO_MSG("Attempting to bind and listen on " << masterlist.size() << " port pairs.");
        PollingListenerSetPtr ln(new PollingListenerSet(masterlist, SOMAXCONN));
        _listener = ln;
    } catch (CxxError& e) {
        LOG_ERROR_MSG(e.what());
        throw MMCSConsolePort::Error(e.errcode, e.what());
        return;
    }
}

MMCSConsolePortClient*
MMCSConsolePortServer::accept()
{
    CxxSockets::SocketPtr newsockptr;
    if(MMCSProperties::getProperty(SECURE_CONSOLE) == "true") {
        CxxSockets::SecureTCPSocketPtr
            newsock(new CxxSockets::SecureTCPSocket);
        bool accepted = false;
        while(!accepted) {
            try {
                bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand);
                port_config.setProperties(MMCSProperties::getProperties(), "");
                port_config.notifyComplete();
                accepted = _listener->AcceptNew(newsock, port_config);
            } catch (CxxSockets::CxxError& e) {
                int error = 0;
                if(e.errcode == -1)
                    // SSL gives us a -1 on a failed validation
                    error = ECONNABORTED;
                else error = e.errcode;

                throw MMCSConsolePort::Error(error, e.what());
            }
        }
        newsockptr = newsock;
    } else {
        CxxSockets::TCPSocketPtr
            newsock(new CxxSockets::TCPSocket);
        bool accepted = false;
        while(!accepted) {
            try {
                accepted = _listener->AcceptNew(newsock);
            } catch (CxxSockets::CxxError& e) {
                throw MMCSConsolePort::Error(e.errcode, e.what());
            }
        }
        newsockptr = newsock;
    }

    MMCSConsolePortClient* newConnection = new MMCSConsolePortClient(newsockptr);
    return newConnection;
}

MMCSConsolePortServer::~MMCSConsolePortServer()
{
}
