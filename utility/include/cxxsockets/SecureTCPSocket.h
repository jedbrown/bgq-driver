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
 * \file utility/include/cxxsockets/SecureTCPSocket.h
 */

#ifndef CXXSOCKET_SECURE_TCP_SOCKET_H
#define CXXSOCKET_SECURE_TCP_SOCKET_H

#include <utility/include/cxxsockets/TCPSocket.h>
#include <utility/include/cxxsockets/types.h>
#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>
#include <utility/include/UserId.h>

#include <openssl/ssl.h>

namespace CxxSockets {

//! \brief Type of user for a secure socket.
enum UserType { Administrator, Normal };

class SecureTCPSocket : public TCPSocket
{
public:
    /*!
     * \brief Secure a TCPSocket with SSL_accept
     *
     * \note ownership of the file descriptor managed by TCPSocket is released
     */
    SecureTCPSocket(
            const TCPSocketPtr& socket,                                 //!< [in]
            const bgq::utility::ServerPortConfiguration& port_config    //!< [in]
            );

    /*!
     * \brief ctor
     */
    SecureTCPSocket(
            int family,     //!< [in]
            int fd          //!< [in]
            );

    /*!
     * \brief dtor
     */
    ~SecureTCPSocket();

    //! \brief Connect and just specify a remote address
    void Connect(
            const SockAddr& remote_sa,                                  //!< [in] SockAddr object specifying the remote address to connect
            const bgq::utility::ClientPortConfiguration& port_config    //!< [in] port_config
            );

    //! \brief Make this connection a secure one! You'll only want to call this
    //! if you passed a previously connected file descriptor to the constructor.
    //! Otherwise, you get this for free.
    void MakeSecure(
            const bgq::utility::ClientPortConfiguration& port_config    //!< [in]
            );

    void MakeSecure(
            const bgq::utility::ServerPortConfiguration& port_config    //!< [in]
            );

    //! \brief Managed send. Takes care of byte counting for you.
    //!
    //! This method will send the size of the data before sending the
    //! data. It's meant to be used with a CxxSocket on the other
    //! side. This allows the user to send a Message object atomically.
    int Send(
            const Message& msg  //!< [in] msg  Message object to send.
            );

    //! \brief Managed receive.
    //!
    //! This method recvs the size of the message first and then
    //! continues to receive data until that amount has arrived.
    //! The assumption is that it is communicating with a CxxSocket.
    int Receive(
            Message& msg  //!< [in] msg Message object to receive.
            );

    const bgq::utility::UserId& getUserId() const { return *_uid_ptr; }

    UserType getUserType() const { return _utype; }

private:
    void ClientHandshake(const bgq::utility::ClientPortConfiguration& port_config);
    void ServerHandshake(const bgq::utility::ServerPortConfiguration& port_config);
    void SetupCredentials(const bgq::utility::SslConfiguration& sslconfig);
    void SetupContext(const bgq::utility::SslConfiguration& sslconfig);
    bgq::utility::UserId::ConstPtr   _uid_ptr;
    SSL*                             _ssl;
    SSL_CTX*                         _ctx;
    UserType                         _utype;
};

class SecureTCPSendFunctor
{
public:
    SecureTCPSendFunctor(SSL* ssl) : _ssl(ssl) {}
    int operator()(int _fileDescriptor, const void* msg, size_t length);

private:
    SSL* const _ssl;
};

class SecureTCPReceiveFunctor
{
public:
    SecureTCPReceiveFunctor(SSL* ssl) : _ssl(ssl) {}
    int operator()(int _fileDescriptor, const void* msg, size_t length);

private:
    SSL* const _ssl;
};

}

#endif
