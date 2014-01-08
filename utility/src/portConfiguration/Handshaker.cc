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


#include "Handshaker.h"

#include "pc_util.h"

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <boost/asio/ssl.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <stdint.h>


using boost::bind;

using std::ostringstream;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


Handshaker::Ptr Handshaker::create(
        portConfig::UserIdHandling::Value user_id_handling
    )
{
    return Ptr( new Handshaker( user_id_handling ) );
}


Handshaker::Handshaker(
        portConfig::UserIdHandling::Value user_id_handling
    ) :
    _user_id_handling(user_id_handling)
{
    // Nothing to do.
}


void
Handshaker::handshake(
        portConfig::SocketPtr socket_ptr,
        const ClientPortConfiguration& port_config,
        ClientHandler callback
        )
{
    socket_ptr->async_handshake(
            boost::asio::ssl::stream_base::client,
            bind(
                &Handshaker::_handleClientHandshake,
                shared_from_this(),
                port_config,
                socket_ptr,
                callback,
                boost::asio::placeholders::error
                )
            );
}


void
Handshaker::_handleClientHandshake(
        const ClientPortConfiguration& port_config,
        portConfig::SocketPtr socket_ptr,
        ClientHandler callback,
        const boost::system::error_code& error
    )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( error ) {
        const string error_str(
                "handshake with " + 
                boost::lexical_cast<std::string>(socket_ptr->next_layer().remote_endpoint()) + 
                " failed with " +
                error.message()
                );
        socket_ptr.reset();

        LOG_ERROR_MSG( error_str );

        callback( socket_ptr, error_str, string() /*server_cn*/ );
        return;
    }

    // Get the server's CN so can compare it vs the expected administrative certificate CN.

    std::string server_cn;

    try {
        boost::shared_ptr<std::string> server_cn_ptr(pc_util::extractPeerCn( *socket_ptr ));

        if ( ! server_cn_ptr ) {
            BOOST_THROW_EXCEPTION( runtime_error( "the server didn't provide a certificate and a certificate is required" ) );
        }

        server_cn = *server_cn_ptr;
    } catch ( std::exception& e ) {
        socket_ptr.reset();
        string error_str( "handshake failed, couldn't get commonName from subject name from server's certificate" );

        LOG_ERROR_MSG( error_str );

        callback( socket_ptr, error_str, server_cn );
        return;
    }

    LOG_DEBUG_MSG( "Server's CN='" << server_cn << "'" );

    if ( server_cn != port_config.getAdministrativeCn() ) {
        socket_ptr.reset();
        string error_str( string() + "server sent invalid certificate with cn='" + server_cn + "' expected '" + port_config.getAdministrativeCn() + "'" );

        callback( socket_ptr, error_str, server_cn );
        return;
    }

    // Send the user info.
    //  Gather the user info and serialize it to a string.
    //  Sends the length as 4 ascii bytes, then sends the user id

    bgq::utility::UserId user_id;

    string user_id_str( user_id.serialize() );
    const size_t  user_id_str_len( user_id_str.size() );

    ostringstream oss;
    oss.width( 4 );
    oss.fill( '0' );
    oss << user_id_str_len;

    boost::shared_ptr<std::string> buf( new std::string );
    buf->append( oss.str() );
    buf->append( user_id_str );
    LOG_TRACE_MSG( *buf );


    boost::asio::async_write(
            *socket_ptr,
            boost::asio::buffer( *buf ),
            bind(
                &Handshaker::_handleWroteUserInfo,
                shared_from_this(),
                socket_ptr,
                buf,
                callback,
                boost::asio::placeholders::error,
                server_cn
                )
            );
}


void
Handshaker::_handleWroteUserInfo(
        portConfig::SocketPtr socket_ptr,
        boost::shared_ptr<std::string> /*buf_ptr*/,
        ClientHandler callback,
        const boost::system::error_code& error,
        const std::string& server_cn
    )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( error ) {
        socket_ptr.reset();
        string error_str( string() + "writing user info failed with " + error.message() );

        LOG_ERROR_MSG( "writing user info failed with " << error.message() );

        callback( socket_ptr, error_str, server_cn );
    } else {
        callback( socket_ptr, std::string() /* error_str */, server_cn );
    }
}


void
Handshaker::handshake(
        portConfig::SocketPtr socket_ptr,
        const ServerPortConfiguration& port_config,
        ServerHandler callback
        )
{
    socket_ptr->async_handshake(
            boost::asio::ssl::stream_base::server,
            bind(
                &Handshaker::_handleServerHandshake,
                shared_from_this(),
                port_config,
                socket_ptr,
                callback,
                boost::asio::placeholders::error
                )
            );
}


void
Handshaker::_handleServerHandshake(
        const ServerPortConfiguration& port_config,
        portConfig::SocketPtr socket_ptr,
        ServerHandler callback,
        const boost::system::error_code& err
    )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const std::string administrative_cn( port_config.getAdministrativeCn() );
    const std::string command_cn( port_config.getCommandCn() );
    ServerPortConfiguration::ConnectionType::Value connection_type(port_config.getConnectionType());

    if ( err ) {
        string re_name;
        try {
            re_name = boost::lexical_cast<string>(socket_ptr->lowest_layer().remote_endpoint());
        } catch ( std::exception& e ) {
            re_name = "(unknown)";
        }

        LOG_WARN_MSG( "Handshake with client " << re_name << " failed. Discarding the connection. Error is " << err.message() );
        return;
    }

    try {
        // Get the client's CN if present so can compare it vs the expected CNs.

        boost::shared_ptr<std::string> client_cn_ptr = pc_util::extractPeerCn( *socket_ptr );

        if ( ! client_cn_ptr ) {

            if ( connection_type == ServerPortConfiguration::ConnectionType::Optional ) {
                callback( socket_ptr, UserId::ConstPtr(), portConfig::UserType::None, std::string() );
                return;
            }

            // Server requires a certificate and none was presented, so this fails badly.

            BOOST_THROW_EXCEPTION( runtime_error(
                    string() + "client did not send a certificate and a certificate is required."
                ));
        }

        string client_cn(*client_cn_ptr);

        LOG_DEBUG_MSG( "Client's CN='" << client_cn << "'" );

        portConfig::UserType::Value user_type;

        if ( client_cn == administrative_cn ) {
            user_type = portConfig::UserType::Administrator;
        } else if ( client_cn == command_cn ) {
            user_type = portConfig::UserType::Normal;
        } else {
            BOOST_THROW_EXCEPTION(
                    runtime_error(
                        string() + "client sent certificate with invalid CN. The client's CN is '" + client_cn + "'"
                        )
                    );
        }

        if ( (connection_type == ServerPortConfiguration::ConnectionType::AdministrativeOnly) && (user_type != portConfig::UserType::Administrator ) ) {
            BOOST_THROW_EXCEPTION(
                    runtime_error(
                        string() + "only accepting administrative certificate and client using command certificate"
                        )
                    );
        }

        // If the application requested to not read the user ID then don't read the user ID and just return an empty User ID.
        if ( _user_id_handling == portConfig::UserIdHandling::Skip ) {
            callback( socket_ptr, UserId::ConstPtr(), user_type, client_cn );
            return;
        }

        // Read the user info off the socket.

        UserIdBufPtr user_id_buf_ptr( new UserIdBuf );
        user_id_buf_ptr->resize( 4 ); // read 4 bytes off the socket, which is the length in ascii text.

        boost::asio::async_read(
                *socket_ptr,
                boost::asio::buffer( *user_id_buf_ptr ),
                bind(
                    &Handshaker::_handleReadUserLen,
                    shared_from_this(),
                    socket_ptr,
                    user_id_buf_ptr,
                    user_type,
                    client_cn,
                    callback,
                    boost::asio::placeholders::error
                    )
                );
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Error during handshake, " << e.what() << ". Discarding the connection." );
    }
}


void
Handshaker::_handleReadUserLen(
        portConfig::SocketPtr socket_ptr,
        UserIdBufPtr user_id_buf_ptr,
        portConfig::UserType::Value user_type,
        const std::string& client_cn,
        ServerHandler callback,
        const boost::system::error_code& err
    )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( err ) {
        LOG_WARN_MSG( "Handshake with client failed reading user message length. Discarding the connection." );
        return;
    }

    try {
        uint32_t user_id_size( boost::lexical_cast<uint32_t>( string( user_id_buf_ptr->begin(), user_id_buf_ptr->end() ) ));

        if ( user_id_size == 0 ) {
            // No user info provided. Only if using administrative certificate is this ok.

            if ( user_type != portConfig::UserType::Administrator ) {
                BOOST_THROW_EXCEPTION(
                        runtime_error(
                            string() +  "handshake with client failed because user info was not send and the administrative certificate wasn't used"
                            )
                        );
            }

            // only OK because using the administrative certificate.

            LOG_INFO_MSG( "Client didn't send user info but used administrative certificate." );

            callback( socket_ptr,  UserId::ConstPtr( new UserId() ), portConfig::UserType::Administrator, client_cn );
            return;
        }

        user_id_buf_ptr->resize( user_id_size );

        boost::asio::async_read(
                *socket_ptr,
                boost::asio::buffer( *user_id_buf_ptr ),
                bind(
                        &Handshaker::_handleReadUserInfo,
                        shared_from_this(),
                        socket_ptr,
                        user_id_buf_ptr,
                        user_type,
                        client_cn,
                        callback,
                        boost::asio::placeholders::error
                    )
            );
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Error during handshake, " << e.what() << ". Discarding the connection." );
    }
}


void
Handshaker::_handleReadUserInfo(
        portConfig::SocketPtr socket_ptr,
        UserIdBufPtr user_id_buf_ptr,
        portConfig::UserType::Value user_type,
        const std::string& client_cn,
        ServerHandler callback,
        const boost::system::error_code& err
    )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( err ) {
        LOG_WARN_MSG( "Handshake with client failed reading user message. Discarding the connection." );
        return;
    }

    // create UserId from buffer
    try {
        UserId::ConstPtr user_id_ptr( new UserId( *user_id_buf_ptr ) );

        callback( socket_ptr, user_id_ptr, user_type, client_cn );
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Error during handshake, " <<  e.what() << ". Discarding the connection." );
    }
}


namespace portConfig {

std::ostream& operator<<( std::ostream& os, UserType::Value user_type )
{
    os << (
            user_type == UserType::Administrator ? "Administrative" :
            user_type == UserType::Normal ? "Normal/Command" :
            user_type == UserType::None ? "None" :
            "Unexpected"
        );
    return os;
}

} // namespace portConfig


} // namespace bgq::utility
} // namespace bgq
