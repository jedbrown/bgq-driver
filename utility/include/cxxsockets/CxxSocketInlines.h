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
#include "SocketTypes.h"
#include <log4cxx/logger.h>

static log4cxx::LoggerPtr inlines_logger(log4cxx::Logger::getLogger( "ibm.utility.cxxsockets.CxxSocketInlines" ));

template<class sendfunctor>
inline int CxxSockets::TCPSocket::InternalSend(CxxSockets::Message& msg, int flags, sendfunctor& f) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    // Dead simple protocol:  Send a four byte size followed by data.
    uint32_t size = msg.str().length();
    const uint32_t maxsize = 150000000;
    if(size > maxsize) {
        std::ostringstream errmsg;
        errmsg << "size send error, message too big " 
               << "(MsgSize=" << msg.str().length() << ", maxsize=" << maxsize << ")" 
               << ": " << strerror(EFBIG);
        std::string msgstr = errmsg.str();
        LOG4CXX_ERROR(inlines_logger, msgstr);
        throw CxxSockets::SockSoftError(errno, errmsg.str());
    }

    if(size == 0) {
        std::ostringstream errmsg;
        errmsg << "zero length send attempted";
        LOG4CXX_INFO(inlines_logger, errmsg.str());
        return 0; 
    }

    uint32_t sizeNbo = htonl(msg.str().length());  // size in network byte order format.

    if(f(_fileDescriptor, &sizeNbo, sizeof(uint32_t), flags) < 0) {
        std::ostringstream errmsg;
        errmsg << "size send error: " << strerror(errno);
        LOG4CXX_WARN(inlines_logger, errmsg.str());
        throw CxxSockets::SockHardError(errno, errmsg.str());
    }
    
    LOG4CXX_DEBUG(inlines_logger, "Sending size of " << sizeNbo << " bytes.");

    // Send the data
    uint32_t bytes_sent = 0;
    uint32_t bytes_to_send = size;
    uint32_t total_bytes_sent = 0;
    int cursor = 0;

    while(total_bytes_sent < size) {
        bytes_sent = f(_fileDescriptor, &(msg.str()[cursor]), bytes_to_send, flags);
        
        if(bytes_sent < 0) {
            std::ostringstream errmsg;
            errmsg << "data send error: " << strerror(errno);
            LOG4CXX_WARN(inlines_logger, errmsg.str());
            throw CxxSockets::SockHardError(errno, errmsg.str());
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
inline int CxxSockets::TCPSocket::InternalSendUnmanaged(CxxSockets::Message& msg, int flags, sendfunctor& f) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    int bytes_sent;

    bytes_sent = f(_fileDescriptor, msg.str().c_str(), msg.str().length(), flags);

    LOG4CXX_DEBUG(inlines_logger, "Sent " << bytes_sent << " bytes.");

    if(bytes_sent < 0) {
        std::ostringstream errmsg;
        errmsg << "size send error: " << strerror(errno);
        LOG4CXX_ERROR(inlines_logger, errmsg.str());
        throw CxxSockets::SockSoftError(errno, errmsg.str());
    }
    return bytes_sent;
}

template<class receivefunctor>
inline int CxxSockets::TCPSocket::InternalReceive(CxxSockets::Message& msg, int flags, receivefunctor& f) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
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
            throw CxxSockets::SockHardError(errno, errmsg.str());
        else
            throw CxxSockets::SockSoftError(errno, errmsg.str());
    }

    else if(bytes == 0) {
        std::ostringstream errmsg;
        errmsg << "Receive error: Connection closed: " << strerror(errno);
        LOG4CXX_INFO(inlines_logger, errmsg.str());
        throw CxxSockets::SockCloseUnexpected(errno, errmsg.str());
    }

    else if(bytes < 4) {
        std::ostringstream errmsg;
        errmsg << "Receive error: Invalid data size " << strerror(errno);
        LOG4CXX_INFO(inlines_logger, errmsg.str());
        throw CxxSockets::SockSoftError(errno, errmsg.str());
    }

    if(size == 0) {
        std::ostringstream errmsg;
        errmsg << "zero length size received";
        LOG4CXX_INFO(inlines_logger, errmsg.str());
        return 0;
    }


    const unsigned int BUFFSIZE = 100000;
    char databuff[BUFFSIZE];
    bytes = 0;
    int gotten = 0;
    unsigned int bytes_left = size;

    while(msg.str().length() < size) {
        bzero(&databuff, BUFFSIZE);

        int bytes_to_get = 0;
        if(BUFFSIZE > bytes_left) {
            bytes_to_get = bytes_left;
        } else {
            bytes_to_get = BUFFSIZE - 1;
        }

        bytes = f(_fileDescriptor, databuff, bytes_to_get, flags);

        if(bytes < 0) {
            std::ostringstream msg;
            msg << "data Receive error: " << strerror(errno);
            LOG4CXX_INFO(inlines_logger, msg.str());
            throw CxxSockets::SockHardError(errno, msg.str());
        }

        else if(bytes == 0) {
            std::ostringstream msg;
            msg << "Receive error: Connection closed: " << strerror(errno);
            LOG4CXX_INFO(inlines_logger, msg.str());
            throw CxxSockets::SockCloseUnexpected(errno, msg.str());
        }

        gotten += bytes;
        bytes_left = size - gotten;
        msg.write(databuff, bytes);
        LOG4CXX_DEBUG(inlines_logger, "Received: " << "size=" <<  size << " message size=" << msg.str().length() << " bytes=" << bytes << " gotten=" << gotten << " bytes_left=" << bytes_left);
    }

    LOG4CXX_DEBUG(inlines_logger, "Received complete message of size " << size << " bytes.");
    return size;
}

template<class receivefunctor>
inline int CxxSockets::TCPSocket::InternalReceiveUnManaged(Message& msg, unsigned int bytes, int flags, receivefunctor& f) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    char buffer[bytes + 1];
    bzero(&buffer, bytes + 1);
    int bytes_received = 0;

    bytes_received = f(_fileDescriptor, buffer, bytes, flags);

    if(bytes_received < 0) {
        std::ostringstream msg;
        msg << "Receive error: " << strerror(errno);
        LOG4CXX_INFO(inlines_logger, msg.str());
        if(errno != EINTR)
            throw CxxSockets::SockHardError(errno, msg.str());
        else
            throw CxxSockets::SockSoftError(errno, msg.str());
    }
    
    msg.write(buffer, bytes_received);
    return bytes_received;
}

