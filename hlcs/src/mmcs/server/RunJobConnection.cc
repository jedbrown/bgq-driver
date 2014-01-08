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

#include "RunJobConnection.h"

#include "common/Properties.h"

#include <utility/include/portConfiguration/Connector.h>
#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

LOG_DECLARE_FILE("mmcs.server");

namespace {

void run_io_service(
        boost::asio::io_service& io_service
        )
{
    while (1) {
        try {
            const size_t handlers = io_service.run();
            LOG_DEBUG_MSG( "Ran " << handlers << " handlers." );
            break;
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "Uncaught exception: " << e.what() );
            sleep(5);
        }
    }
}

} // anonymous namespace

namespace mmcs {
namespace server {

RunJobConnection::RunJobConnection() :
    _socket(),
    _io_service( 1 /* currency hint */ ),
    _pendingSignals( ),
    _incomingHeader( ),
    _incomingMessage( ),
    _outgoingHeader( ),
    _outgoingMessage( ),
    _thread( )
{
    this->connect();

    _thread = boost::thread(
            boost::bind( &run_io_service, boost::ref(_io_service) )
            );
}

void
RunJobConnection::stop()
{
    _io_service.stop();
    _thread.join();
}

void
RunJobConnection::connect()
{
    _socket.reset();

    const std::string servname = "24510";
    bgq::utility::ClientPortConfiguration config(servname);
    config.setProperties( common::Properties::getProperties(), "runjob.server.commands" );
    config.notifyComplete();

    const boost::shared_ptr<bgq::utility::Connector> connector(
            new bgq::utility::Connector(
                _io_service,
                config
                )
            );

    connector->async_connect(
            boost::bind(
                &RunJobConnection::connectHandler,
                this,
                connector,
                _1
                )
            );
}

void
RunJobConnection::connectHandler(
        const boost::shared_ptr<bgq::utility::Connector>& connector,
        const bgq::utility::Connector::ConnectResult& result
        )
{
    if ( result.error ) {
        LOG_DEBUG_MSG( "Could not connect: " << result.error_str );
        const boost::shared_ptr<boost::asio::deadline_timer> timer(
                new boost::asio::deadline_timer( _io_service )
                );
        timer->expires_from_now( boost::posix_time::seconds(5) );
        timer->async_wait(
                boost::bind(
                    &RunJobConnection::waitHandler,
                    this,
                    timer,
                    _1
                    )
                );
        return;
    }

    _socket = result.socket_ptr;
    LOG_DEBUG_MSG( "Connected to " << _socket->lowest_layer().remote_endpoint() );

    // start reading header
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            boost::bind(
                &RunJobConnection::readHeaderHandler,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );

    // send any queued signals
    if ( !_pendingSignals.empty() ) {
        this->write();
    }
}

void
RunJobConnection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "Could not read: " << boost::system::system_error(error).what() );
        this->connect();
        return;
    }

    boost::asio::async_read(
            *_socket,
            _incomingMessage.prepare( _incomingHeader._length ),
            boost::bind(
                &RunJobConnection::readBodyHandler,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
RunJobConnection::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    _incomingMessage.commit( bytesTransferred );

    if ( error == boost::asio::error::operation_aborted ) {
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "Could not read: " << boost::system::system_error(error).what() );
        this->connect();
        return;
    }

    // we only support a kill response
    runjob::commands::response::KillJob response;

    if ( _incomingHeader._type != response.getType() ) {
        LOG_WARN_MSG( "Expected response type " << response.getType() );
    } else if ( _incomingHeader._tag != static_cast<unsigned>(response.getTag()) ) {
        LOG_WARN_MSG( "Expected response tag " << response.getTag() );
    } else {
        std::istream is( &_incomingMessage );
        boost::archive::text_iarchive ar( is );
        ar & response;
        switch ( response.getError() ) {
            case runjob::commands::error::success:
                LOG_DEBUG_MSG("Kill job " << response._job << " successful." ); break;
            case runjob::commands::error::job_already_dying:
                LOG_DEBUG_MSG("Job " << response._job << " already dying." ); break;
            default:
                LOG_ERROR_MSG(
                        "Could not kill job " << response._job << " (" <<
                        runjob::commands::error::toString( response.getError() ) << ")"
                        );
                if ( !response.getMessage().empty() ) {
                    LOG_ERROR_MSG( response.getMessage() );
                }
        }
    }

    // get next header
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            boost::bind(
                &RunJobConnection::readHeaderHandler,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
RunJobConnection::waitHandler(
        const boost::shared_ptr<boost::asio::deadline_timer>& timer,
        const boost::system::error_code& error
        )
{
    if ( error ) {
        LOG_WARN_MSG( "Could not wait: " << boost::system::system_error(error).what() );
        return;
    }

    this->connect();
}

void
RunJobConnection::killRecidImpl(
        const BGQDB::job::Id job,
        const int signal,
        const int recid
        )
{
    PendingSignal e;
    e._job = job;
    e._signal = signal;
    e._recid = recid;
    _pendingSignals.push_back( e );

    // can't signal a job if we're not connected
    if ( !_socket ) return;

    if ( _pendingSignals.size() == 1 ) {
        this->write();
    }
}

void
RunJobConnection::killDetailsImpl(
        const BGQDB::job::Id job,
        const int signal,
        const std::string& details
        )
{
    PendingSignal e;
    e._job = job;
    e._signal = signal;
    e._recid = 0;
    e._details = details;
    _pendingSignals.push_back( e );

    // can't signal a job if we're not connected
    if ( !_socket ) return;

    if ( _pendingSignals.size() == 1 ) {
        this->write();
    }
}

void
RunJobConnection::write()
{
    const PendingSignal& front = _pendingSignals.front();

    runjob::commands::request::KillJob request;
    request._job = front._job;
    request._signal = front._signal;
    request._controlActionRecordId = front._recid;
    request._details = front._details;
    std::ostream os(&_outgoingMessage);
    request.doSerialize( os );

    _outgoingHeader._type = request.getType();
    _outgoingHeader._tag = request.getTag();
    _outgoingHeader._length = _outgoingMessage.size();

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&_outgoingHeader, sizeof(_outgoingHeader) ) );
    buffers.push_back( _outgoingMessage.data() );

    boost::asio::async_write(
            *_socket,
            buffers,
            boost::bind(
                &RunJobConnection::writeHandler,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
RunJobConnection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    // assume this handler is protected by strand

    _outgoingMessage.consume( bytesTransferred );
    _pendingSignals.pop_front();

    if ( error ) {
        LOG_ERROR_MSG( "Could not write: " << boost::system::system_error(error).what() );
        return;
    }

    if ( !_pendingSignals.empty() ) {
        this->write();
    }
}

void
RunJobConnection::kill(
        const BGQDB::job::Id job,
        const int signal,
        const int recid
        )
{
    LOG_WARN_MSG("Killing job " << job << " with signal " << signal << " due to RAS event " << recid );

    _io_service.post(
            boost::bind(
                &RunJobConnection::killRecidImpl,
                this,
                job,
                signal,
                recid
                )
            );
}

void
RunJobConnection::kill(
        const BGQDB::job::Id job,
        const int signal,
        const std::string& details
        )
{
    LOG_WARN_MSG("Killing job " << job << " with signal " << signal << " " << details );

    _io_service.post(
            boost::bind(
                &RunJobConnection::killDetailsImpl,
                this,
                job,
                signal,
                details
                )
            );
}

} } // namespace mmcs::server
