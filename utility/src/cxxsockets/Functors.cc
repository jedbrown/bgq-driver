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
#include "cxxsockets/SocketTypes.h"

LOG_DECLARE_FILE("utility.cxxsockets");

int CxxSockets::TCPSendFunctor::operator() (
                                int _fileDescriptor,
                                const void* msg, 
                                int length, 
                                int flags)
{
    LOG_DEBUG_MSG("TCP Sending unencrypted data");
    int bytes = send(_fileDescriptor, msg, length, flags); 
    if(bytes < 0) 
        error = errno;
    return bytes;
}

int CxxSockets::TCPReceiveFunctor::operator() (
                                   int _fileDescriptor,
                                   const void* msg,
                                   int length,
                                   int flags)
{
    LOG_DEBUG_MSG("TCP Receiving unencrypted data");
    int bytes_received = recv(_fileDescriptor, (void*)msg, length, flags);
    LOG_TRACE_MSG("Data received");
    if(bytes_received < 0)
        error = errno;
    return bytes_received;
}


int CxxSockets::SecureTCPSendFunctor::operator() (int, const void* msg, int length, int) {
    LOG_DEBUG_MSG("TCP Sending encrypted data");
    int rc = SSL_write(_ssl, msg, length);
    if(rc <= 0) {
        SecureTCPSocket::printSSLError(_ssl, rc);
    }
    return rc;
}

int CxxSockets::SecureTCPReceiveFunctor::operator() (int, const void* msg, int length, int) {
    LOG_DEBUG_MSG("TCP Receiving encrypted data");
    int rc = SSL_read(_ssl, (void*)msg, length);
    if(rc < 0) {
        SecureTCPSocket::printSSLError(_ssl, rc);
    }
    return rc;
}
