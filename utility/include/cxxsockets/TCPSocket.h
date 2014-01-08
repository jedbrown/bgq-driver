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
 * \file utility/include/cxxsockets/TCPSocket.h
 */

#ifndef CXXSOCKET_TCP_SOCKET_H
#define CXXSOCKET_TCP_SOCKET_H

#include <utility/include/cxxsockets/exception.h>
#include <utility/include/cxxsockets/Socket.h>
#include <utility/include/cxxsockets/types.h>
#include <utility/include/cxxsockets/logging.h>

#include <string>

#include <arpa/inet.h>

namespace CxxSockets {

//! \brief Socket on top of TCP.
//!
//! Address family is determined by sockaddr or param
//! depending upon ctor used.
class TCPSocket : public Socket
{
    friend class ListeningSocket;
    friend class SecureTCPSocket;
protected:
    std::string _connection_data;
    bool _nonagle;

    //! \brief Actually perform the tcp connect
    void performConnect(const SockAddr& remote);

    //! \brief Internal non-locked, insecure method
    void mConnect(const SockAddr& remote_sa);

    void replaceFd(int fd);

    void releaseFd();

    template<class sendfunctor>
    inline int InternalSend(const Message& msg, int flags, sendfunctor& f)
    {
        CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
        const log4cxx::LoggerPtr inlines_logger(log4cxx::Logger::getLogger( "ibm.utility.cxxsockets.CxxSocketInlines" ));
        // Dead simple protocol:  Send a four byte size followed by data.
        const int32_t size = msg.str().length();
        const int32_t maxsize = 150000000;
        if(size > maxsize) {
            std::ostringstream errmsg;
            errmsg << "size send error, message too big "
                << "(MsgSize=" << msg.str().length() << ", maxsize=" << maxsize << ")"
                << ": " << strerror(EFBIG);
            std::string msgstr = errmsg.str();
            LOG4CXX_ERROR(inlines_logger, msgstr);
            throw SoftError(errno, errmsg.str());
        }

        if(size == 0) {
            std::ostringstream errmsg;
            errmsg << "zero length send attempted";
            LOG4CXX_INFO(inlines_logger, errmsg.str());
            return 0;
        }

        const uint32_t sizeNbo = htonl(msg.str().length());  // size in network byte order format.

        if(f(_fileDescriptor, &sizeNbo, sizeof(uint32_t), flags) < 0) {
            std::ostringstream errmsg;
            errmsg << "size send error: " << strerror(errno);
            LOG4CXX_WARN(inlines_logger, errmsg.str());
            throw HardError(errno, errmsg.str());
        }

        LOG4CXX_DEBUG(inlines_logger, "Sending size of " << sizeNbo << " bytes.");

        // Send the data
        int32_t bytes_sent = 0;
        uint32_t bytes_to_send = size;
        int32_t total_bytes_sent = 0;
        int cursor = 0;

        while(total_bytes_sent < size) {
            bytes_sent = f(_fileDescriptor, &(msg.str()[cursor]), bytes_to_send, flags);

            if(bytes_sent < 0) {
                std::ostringstream errmsg;
                errmsg << "data send error: " << strerror(errno);
                LOG4CXX_WARN(inlines_logger, errmsg.str());
                throw HardError(errno, errmsg.str());
            }

            if(bytes_sent < size) {
                LOG4CXX_TRACE(inlines_logger, "Sent " << bytes_sent << " of " << size << " total bytes to send.");
            }

            total_bytes_sent += bytes_sent;
            bytes_to_send = size - total_bytes_sent;
            cursor += bytes_sent;
        }
        LOG4CXX_DEBUG(inlines_logger, "Sent " << bytes_sent << " bytes.");
        return size;
    }

    template<class sendfunctor>
    inline int InternalSendUnmanaged(const Message& msg, int flags, sendfunctor& f)
    {
        const log4cxx::LoggerPtr inlines_logger(log4cxx::Logger::getLogger( "ibm.utility.cxxsockets.CxxSocketInlines" ));
        CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;

        const int bytes_sent = f(_fileDescriptor, msg.str().c_str(), msg.str().length(), flags);

        LOG4CXX_DEBUG(inlines_logger, "Sent " << bytes_sent << " bytes.");

        if(bytes_sent < 0) {
            std::ostringstream errmsg;
            errmsg << "size send error: " << strerror(errno);
            LOG4CXX_ERROR(inlines_logger, errmsg.str());
            throw SoftError(errno, errmsg.str());
        }
        return bytes_sent;
    }

    template<class receivefunctor>
    inline int InternalReceive(Message& msg, int flags, receivefunctor& f)
    {
        const log4cxx::LoggerPtr inlines_logger(log4cxx::Logger::getLogger( "ibm.utility.cxxsockets.CxxSocketInlines" ));
        CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
        uint32_t size = 0;
        int bytes = 0;

        LOG4CXX_TRACE(inlines_logger, "Calling Functor");
        bytes = f(_fileDescriptor, &size, sizeof(uint32_t), MSG_WAITALL);
        LOG4CXX_TRACE(inlines_logger, "Calling ntohl");
        size = ntohl(size);

        LOG4CXX_DEBUG(inlines_logger, "Receiving message of size " << size << " bytes.");

        LOG4CXX_TRACE(inlines_logger, "size bytes received=" << bytes);
        if(bytes < 0) {
            std::ostringstream errmsg;
            errmsg << "size Receive error: " << strerror(errno);
            LOG4CXX_INFO(inlines_logger, errmsg.str());
            if(errno != EINTR && errno != EAGAIN)
                throw HardError(errno, errmsg.str());
            else
                throw SoftError(errno, errmsg.str());
        }

        else if(bytes == 0) {
            std::ostringstream errmsg;
            errmsg << "Receive error: Connection closed: " << strerror(errno);
            LOG4CXX_INFO(inlines_logger, errmsg.str());
            throw CloseUnexpected(errno, errmsg.str());
        }

        else if(bytes < 4) {
            std::ostringstream errmsg;
            errmsg << "Receive error: Invalid data size " << strerror(errno);
            LOG4CXX_INFO(inlines_logger, errmsg.str());
            throw SoftError(errno, errmsg.str());
        }

        if(size == 0) {
            std::ostringstream errmsg;
            errmsg << "zero length size received";
            LOG4CXX_INFO(inlines_logger, errmsg.str());
            return 0;
        }


        std::vector<char> buf;
        buf.reserve( size );
        bytes = 0;
        int gotten = 0;
        unsigned int bytes_left = size;

        while(bytes_left) {
            bytes = f(_fileDescriptor, &buf[0] + gotten, bytes_left, flags);

            if(bytes < 0) {
                std::ostringstream msg;
                msg << "data Receive error: " << strerror(errno);
                LOG4CXX_INFO(inlines_logger, msg.str());
                throw HardError(errno, msg.str());
            }

            else if(bytes == 0) {
                std::ostringstream msg;
                msg << "Receive error: Connection closed: " << strerror(errno);
                LOG4CXX_INFO(inlines_logger, msg.str());
                throw CloseUnexpected(errno, msg.str());
            }

            gotten += bytes;
            bytes_left = size - gotten;
            LOG4CXX_DEBUG(inlines_logger, "Received: " << "size=" <<  size << " bytes=" << bytes << " gotten=" << gotten << " bytes_left=" << bytes_left);
        }
        msg.write(&buf[0], size);

        LOG4CXX_DEBUG(inlines_logger, "Received complete message of size " << size << " bytes.");
        return size;
    }

    template<class receivefunctor>
    inline int InternalReceiveUnManaged(Message& msg, unsigned int bytes, int flags, receivefunctor& f)
    {
        const log4cxx::LoggerPtr inlines_logger(log4cxx::Logger::getLogger( "ibm.utility.cxxsockets.CxxSocketInlines" ));
        CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
        char buffer[bytes + 1];
        bzero(&buffer, bytes + 1);
        int bytes_received = 0;

        bytes_received = f(_fileDescriptor, buffer, bytes, flags);

        if(bytes_received < 0) {
            std::ostringstream msg;
            msg << "Receive error: " << strerror(errno);
            LOG4CXX_INFO(inlines_logger, msg.str());
            if(errno != EINTR)
                throw HardError(errno, msg.str());
            else
                throw SoftError(errno, msg.str());
        }

        msg.write(buffer, bytes_received);
        return bytes_received;
    }

public:
    //! \brief Default ctor.  This one does NOT acquire an FD.  It gets a v4 socket by default
    TCPSocket();

    //! \brief Build a socket based on either a family or pre-acquired file
    //! descriptor.  Socket sets will generally use this.
    TCPSocket(int family, int fd);

    //! \brief Connect and just specify a remote address
    //!
    //! \param remote_sa SockAddr object specifying the remote address to connect
    void Connect(const SockAddr& remote_sa);

    //! \brief set up "keepalive" to probe for dead peers
    //! \param onoff Turn it on or off.
    //! \param firstprobe How long to wait to send the first probe after last data packet.
    //! \param probeint How often to send a probe.
    //! \param probecount Number of unanswered probes to consider the connection dead.
    void setProbe(bool onoff, int firstprobe = 0, int probeint = 0, int probecount = 0);

    //! \brief switch the nagle algorithm on or off.  It's off by default.
    //! \returns whether it is on or off.
    bool toggleNoDelay();

    //! \brief Managed send.  Takes care of byte counting for you.
    //!
    //! \param msg  Message object to send.
    //! \param flags
    //!
    //! This method will send the size of the data before sending the
    //! data.  It's meant to be used with a CxxSocket on the other
    //! side.  This allows the user to send a Message object
    //! atomically.
    int Send(Message& msg, int flags = 0);

    //! \brief Unmanaged send.  User must manage data.
    //!
    //! \param msg Message object to send.
    //! \param flags
    //!
    //! Just send the message.  Use when the other side may not
    //! be using CxxSockets.  Can be blocking or not.
    int SendUnManaged(Message& msg, int flags = 0);

    //! \brief Managed receive.
    //! \param msg Message object to receive.
    //! \param flags
    //!
    //! This method recvs the size of the message first and then
    //! continues to receive data until that amount has arrived.
    //! The assumption is that it is communicating with a CxxSocket.
    int Receive(Message& msg, int flags = 0);

    //! \brief Unmanaged receive
    //! \param msg Message object to receive.
    //! \param bytes
    //! \param flags
    //!
    //! This method just receives whatever is available and returns.
    int ReceiveUnManaged(Message& msg, unsigned int bytes, int flags = 0);
};

class TCPSendFunctor
{
    int error;
public:
    int operator() (int _fileDescriptor, const void* msg, int length, int flags);
};

class TCPReceiveFunctor
{
    int error;
public:
    int operator() (int _fileDescriptor, const void* msg, int length, int flags);
};

}

#endif
