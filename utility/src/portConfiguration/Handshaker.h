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


#ifndef BGQ_UTILITY_HANDSHAKER_H_
#define BGQ_UTILITY_HANDSHAKER_H_


/*! \file
 *
 *  \brief Handshaker class.
 */


#include "portConfiguration/ClientPortConfiguration.h"
#include "portConfiguration/ServerPortConfiguration.h"
#include "portConfiguration/types.h"

#include <utility/include/UserId.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <string>
#include <vector>


namespace bgq {
namespace utility {


/*! \brief Handshaker.
 *
 * This class encapsulates the handshaking that occurs
 * between a server and the client.
 *
 */
class Handshaker : public boost::enable_shared_from_this<Handshaker>
{
public:

    typedef boost::shared_ptr<Handshaker> Ptr;


    /*! \brief Function type for callbacks.
     */
    typedef boost::function<
        void (
                portConfig::SocketPtr socket_ptr,
                UserId::ConstPtr user_id_ptr,
                portConfig::UserType::Value user_type,
                const std::string& client_cn
             )
        > ServerHandler;

    /*! \brief Function type for callbacks.
     */
    typedef boost::function<
        void (
                portConfig::SocketPtr socket_ptr,
                const std::string& message,
                const std::string& server_cn
             )
        > ClientHandler;

public:

    /*! \brief Handshaker must be created as a shared pointer.
     */
    static Ptr create(
            portConfig::UserIdHandling::Value user_id_handling = portConfig::UserIdHandling::Process
        );


    /*! \brief Perform the client handshake asynchronously.
     */
    void handshake(
            portConfig::SocketPtr socket_ptr, //!< [copy]
            const ClientPortConfiguration& port_config, //!< [ref]
            ClientHandler callback //!< [copy]
            );

    /*! \brief Perform the server handshake asynchronously.
     *
     *  callback invoked on success
     */
    void handshake(
            portConfig::SocketPtr socket_ptr, //!< [copy]
            const ServerPortConfiguration& port_config, //!< [ref]
            ServerHandler callback //!< [copy]
            );


private:
    typedef std::vector<char> UserIdBuf;
    typedef boost::shared_ptr<UserIdBuf> UserIdBufPtr;

private:

    bool _user_id_handling;


    Handshaker(
            portConfig::UserIdHandling::Value user_id_handling
        );


    void _handleClientHandshake(
            const ClientPortConfiguration& port_config,
            portConfig::SocketPtr socket_ptr,
            ClientHandler callback,
            const boost::system::error_code& error
            );

    void _handleServerHandshake(
            const ServerPortConfiguration& port_config,
            portConfig::SocketPtr socket_ptr,
            ServerHandler callback,
            const boost::system::error_code& error
            );

    void _handleWroteUserInfo(
            portConfig::SocketPtr socket_ptr,
            boost::shared_ptr<std::string>,
            ClientHandler callback,
            const boost::system::error_code& error,
            const std::string& server_cn
            );

    void _handleReadUserLen(
            portConfig::SocketPtr socket_ptr,
            UserIdBufPtr user_id_buf_ptr,
            portConfig::UserType::Value user_type,
            const std::string& client_cn,
            ServerHandler callback,
            const boost::system::error_code& err
            );

    void _handleReadUserInfo(
            portConfig::SocketPtr socket_ptr,
            UserIdBufPtr user_id_buf_ptr,
            portConfig::UserType::Value user_type,
            const std::string& client_cn,
            ServerHandler callback,
            const boost::system::error_code& err
            );
};


} // namespace bgq::utility
} // namespace bgq


#endif
