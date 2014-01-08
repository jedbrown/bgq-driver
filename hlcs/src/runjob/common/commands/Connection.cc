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
#include "common/commands/Connection.h"

#include "common/commands/convert.h"
#include "common/commands/Options.h"

#include "common/logging.h"

#include <utility/include/portConfiguration/Connector.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {
namespace commands {

Connection::Connection(
        const Options& options
        ) :
    _io_service(),
    _options(options),
    _connector( _io_service, _options.getPort() ),
    _socket()
{
    while ( !_socket ) {
        try {
            // connect
            _socket = _connector.connect();
        } catch ( const bgq::utility::Connector::ResolveError& e ) {
            LOG_FATAL_MSG( e.what() );
            throw;
        } catch ( const bgq::utility::Connector::HandshakeError& e  ) {
            LOG_FATAL_MSG( e.what() );
            throw;
        } catch ( const bgq::utility::Connector::ConnectError& e  ) {
            if ( _options.getTimeout() ) {
                LOG_WARN_MSG( e.what() );

                // wait a bit before retrying
                const size_t timeout = _options.getTimeout();
                LOG_DEBUG_MSG( "retrying in " << timeout << " second" << ( timeout > 1 ? "s" : "") );
                boost::asio::deadline_timer timer( _io_service );
                timer.expires_from_now(
                        boost::posix_time::seconds(timeout)
                        );
                timer.wait();
            } else {
                LOG_FATAL_MSG( e.what() );
                throw;
            }
        }
    }

    LOG_DEBUG_MSG( "Connected to " << _socket->lowest_layer().remote_endpoint() );
}

runjob::commands::Response::Ptr
Connection::send(
        const runjob::commands::Request::Ptr& msg
        )
{
    runjob::commands::Response::Ptr result;

    // serialize message
    boost::asio::streambuf buf;
    std::ostream os(&buf);
    msg->serialize(os);

    // create header
    runjob::commands::Header header;
    bzero( &header, sizeof(header) );
    header._type = msg->getType();
    header._tag = msg->getTag();
    header._length = static_cast<uint32_t>(buf.size());
    LOG_TRACE_MSG( header );

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&header, sizeof(header) ) );
    buffers.push_back( buf.data() );
    std::size_t length = boost::asio::write( *_socket, buffers);
    buf.consume( length );

    // wait for response
    length =  boost::asio::read(
            *_socket,
            boost::asio::buffer( &header, sizeof(header) )
            );
    LOG_TRACE_MSG( "read header " << length << " bytes" );

    length = boost::asio::read(
                *_socket,
                buf.prepare(header._length)
            );
    LOG_TRACE_MSG( "read body " << length << " bytes" );

    // commit buffer
    buf.commit( length );

    // create message
    result = boost::dynamic_pointer_cast<runjob::commands::Response>(
            runjob::commands::convert(header, buf)
            );
    BOOST_ASSERT(result);

    return result;
}

} // commands
} // runjob
