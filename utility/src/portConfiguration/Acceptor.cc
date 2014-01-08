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


#include <utility/include/portConfiguration/Acceptor.h>

#include "Handshaker.h"
#include "pc_util.h"

#include "Log.h"
#include "UserId.h"

#include <boost/archive/text_iarchive.hpp>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/throw_exception.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>


LOG_DECLARE_FILE( "utility" );


using boost::bind;
using boost::lexical_cast;

using boost::asio::ip::tcp;

using std::exception;
using std::ostringstream;
using std::runtime_error;
using std::string;


namespace bgq {
namespace utility {


// Class Acceptor::AcceptArguments


const Acceptor::AcceptArguments Acceptor::AcceptArguments::ResolveError( Status::ResolveError );
const Acceptor::AcceptArguments Acceptor::AcceptArguments::ListenError( Status::ListenError );
const Acceptor::AcceptArguments Acceptor::AcceptArguments::NoAcceptors( Status::NoAcceptors );
const Acceptor::AcceptArguments Acceptor::AcceptArguments::AcceptError( Status::AcceptError );


Acceptor::AcceptArguments::AcceptArguments(
        Status::Value status
    ) :
        status(status),
        user_type(portConfig::UserType::Normal)
{
    // Nothing to do.
}


Acceptor::AcceptArguments::AcceptArguments(
        const Acceptor::AcceptArguments::Endpoints& endpoints
    ) :
        status(Status::NowAccepting),
        socket_ptr(),
        user_id_ptr(),
        user_type(portConfig::UserType::None),
        client_cn(),
        endpoints(endpoints)
{
    // Nothing to do.
}


Acceptor::AcceptArguments::AcceptArguments(
        portConfig::SocketPtr socket_ptr,
        UserId::ConstPtr user_id_ptr,
        portConfig::UserType::Value user_type,
        const std::string& client_cn
    ) :
        status(Status::OK),
        socket_ptr(socket_ptr),
        user_id_ptr(user_id_ptr),
        user_type(user_type),
        client_cn(client_cn)
{
    // Nothing to do.
}


// Class Acceptor

Acceptor::Acceptor(
        boost::asio::io_service& io_service,
        const ServerPortConfiguration& port_configuration,
        portConfig::UserIdHandling::Value user_id_handling
    ) :
    _io_service(io_service),
    _strand( _io_service ),
    _port_config( port_configuration ),
    _user_id_handling(user_id_handling),
    _context_ptr(port_configuration.createSslConfiguration().createContext( _io_service )),
    _resolver( _io_service )
{
    // Nothing to do.
}


void
Acceptor::start(
        AcceptHandler accept_handler
    )
{
    _accept_handler = accept_handler;

    ServerPortConfiguration::Pairs::const_iterator pi( _port_config.getPairs().begin());

    if ( pi == _port_config.getPairs().end() ) {
        // Done accepting...

        if ( _acceptors.empty() ) {
            string msg( "when accepting, no host-port pairs configured" );
            LOG_ERROR_MSG( msg );
            BOOST_THROW_EXCEPTION( std::invalid_argument( msg ) );
            return;
        }
    }

    _startResolve( pi );
}


void
Acceptor::stop()
{
    LOG_DEBUG_MSG( "Requested to stop..." );

    _strand.post( boost::bind( &Acceptor::_stopImpl, this ) );
}


void
Acceptor::_startResolve(
        ServerPortConfiguration::Pairs::const_iterator pi
    )
{
    const string &hostname(pi->first);
    const string &service_name(pi->second);

    boost::scoped_ptr<tcp::resolver::query> query_ptr;

    if ( hostname.empty() ) {
        query_ptr.reset( new tcp::resolver::query( tcp::v6(), service_name ) );
        LOG_DEBUG_MSG( "Looking up " << service_name );
    } else {
        query_ptr.reset( new tcp::resolver::query( hostname, service_name ) );
        LOG_DEBUG_MSG( "Looking up [" << hostname << "]:" << service_name );
    }

    _resolver.async_resolve(
            *query_ptr,
            bind(
                &Acceptor::_handleResolve,
                this,
                pi,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator
                )
        );
}


void
Acceptor::_handleResolve(
        ServerPortConfiguration::Pairs::const_iterator pi,
        const boost::system::error_code& err,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator
    )
{
    const string &hostname(pi->first);
    const string &service_name(pi->second);

    if ( err ) {
        LOG_ERROR_MSG( "Failed to resolve [" << hostname << "]:" << service_name << ". Error is " << err.message() );

        _acceptors.clear(); // wipe out my acceptors.

        _accept_handler( AcceptArguments::ResolveError ); // notify the app.

        return;
    }

    try {
        if ( endpoint_iterator == tcp::resolver::iterator() ) {
            LOG_WARN_MSG( "Configured listen port [" << hostname << "]:" << service_name << " returned no endpoints." );
        }

        for ( ; endpoint_iterator != tcp::resolver::iterator() ; ++endpoint_iterator ) {
            LOG_DEBUG_MSG( "[" << hostname << "]:" << service_name << " -> " << endpoint_iterator->endpoint() );

            try {

                AcceptorPtr acceptor_ptr( new tcp::acceptor( _io_service ) );
                acceptor_ptr->open( endpoint_iterator->endpoint().protocol() );

                fcntl( acceptor_ptr->native(), F_SETFD, FD_CLOEXEC );

                acceptor_ptr->set_option( tcp::acceptor::reuse_address(true) );
                acceptor_ptr->bind( endpoint_iterator->endpoint() );
                acceptor_ptr->listen();

                _acceptors.push_back( acceptor_ptr );

            } catch ( exception& e ) {

                LOG_ERROR_MSG( "Couldn't listen on " << endpoint_iterator->endpoint() << " from [" << hostname << "]:" << service_name
                        << ". Error is '" << e.what() << "'." );

                _acceptors.clear();

                _accept_handler( AcceptArguments::ListenError ); // Notify the app.

                return;
            }
        }

        ++pi;

        if ( pi == _port_config.getPairs().end() ) {
            // Done resolving.

            LOG_DEBUG_MSG( "Accepting connections on all requested ports." );

            if ( _acceptors.empty() ) {
                _accept_handler( AcceptArguments::NoAcceptors );
                return;
            }

            AcceptArguments::Endpoints endpoints;

            // Start accepting on all the acceptors.
            for ( Acceptors::const_iterator i(_acceptors.begin()) ; i != _acceptors.end() ; ++i ) {
                endpoints.push_back((*i)->local_endpoint());
                _startAccept( *i );
            }

            _accept_handler( AcceptArguments( endpoints ) ); // notify the app.

            return;
        }

        // Resolve the next pair.
        _startResolve( pi );
    } catch ( exception& e ) {
        LOG_ERROR_MSG( string() + "Unexpected error resolving ports when listening. Error is " + e.what() );
        _accept_handler( AcceptArguments::ResolveError ); // notify the app.
    }
}


void
Acceptor::_startAccept(
        AcceptorPtr acceptor_ptr
    )
{
    portConfig::SocketPtr socket_ptr( new portConfig::Socket( _io_service, *_context_ptr ) );

    acceptor_ptr->async_accept(
            socket_ptr->lowest_layer(),
            _strand.wrap( bind(
                    &Acceptor::_handleAccept,
                    this,
                    acceptor_ptr,
                    socket_ptr,
                    boost::asio::placeholders::error
                ) )
        );
}


void
Acceptor::_handleAccept(
        AcceptorPtr acceptor_ptr,
        portConfig::SocketPtr socket_ptr,
        const boost::system::error_code& err
    )
{
    if ( err ) {
        // An error occurred on the accept socket.
        // Remove it from my acceptors and notify the caller.
        // If there are no acceptors remaining, notify the caller that no acceptors,
        // otherwise notify that lost an acceptor.

        const Acceptors::iterator i(std::find( _acceptors.begin(), _acceptors.end(), acceptor_ptr ));

        if ( i != _acceptors.end() )  _acceptors.erase( i );

        _accept_handler( _acceptors.empty() ? AcceptArguments::NoAcceptors : AcceptArguments::AcceptError );

        return;
    }

    Handshaker::create( _user_id_handling )->handshake(
            socket_ptr,
            _port_config,
            bind(
                &Acceptor::_handshakeComplete,
                this,
                _1, _2, _3, _4
                )
            );

    // accept another connection
    _startAccept( acceptor_ptr );
}


void
Acceptor::_handshakeComplete(
        portConfig::SocketPtr socket_ptr,
        UserId::ConstPtr user_id_ptr,
        portConfig::UserType::Value user_type,
        const std::string& client_cn
    )
{
    _accept_handler( AcceptArguments( socket_ptr, user_id_ptr, user_type, client_cn ) );
}


void
Acceptor::_stopImpl()
{
    LOG_DEBUG_MSG( "In _stopImpl" );

    for ( Acceptors::const_iterator i(_acceptors.begin()) ; i != _acceptors.end() ; ++i ) {
        (*i)->close();
    }
    _acceptors.clear();
}


} // namespace bgq::utility
} // namespace bgq
