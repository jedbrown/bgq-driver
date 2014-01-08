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

#include <bgsched/runjob/kill.h>
#include <bgsched/runjob/ConnectionException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>

#include <runjob/commands/KillJob.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include <utility/include/portConfiguration/Connector.h>
#include <utility/include/portConfiguration/types.h>
#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/archive/text_iarchive.hpp>

#include <boost/asio.hpp>

#include "bgsched/utility.h"

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace runjob {

bgq::utility::portConfig::SocketPtr
connect(
        boost::asio::io_service& io_service
        )
{
    const std::string default_service_name( "24510" );
    using namespace bgq::utility;
    ClientPortConfiguration config( default_service_name, ClientPortConfiguration::ConnectionType::Administrative );

    const std::string section_name( "runjob.server.commands" );
    config.setProperties( getProperties(), section_name );
    config.notifyComplete();
    Connector connector( io_service, config );

    return connector.connect();
}

::runjob::commands::response::KillJob::Ptr
send(
        const bgq::utility::portConfig::SocketPtr& connection,
        const ::runjob::commands::request::KillJob::Ptr& request
    )
{
    // serialize message
    boost::asio::streambuf buf;
    std::ostream os( &buf );
    request->doSerialize( os );

    // create header
    ::runjob::commands::Header header;
    bzero( &header, sizeof(header) );
    header._type = request->getType();
    header._tag = request->getTag();
    header._length = static_cast<uint32_t>(buf.size());

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&header, sizeof(header) ) );
    buffers.push_back( buf.data() );
    std::size_t length = boost::asio::write( *connection, buffers );
    buf.consume( length );

    // read header and body
    length = boost::asio::read(
            *connection,
            boost::asio::buffer( &header, sizeof(header) )
            );
    LOG_TRACE_MSG( "read header " << length << " bytes" );
    length = boost::asio::read(
            *connection,
            buf.prepare(header._length)
            );
    LOG_TRACE_MSG( "read body " << length << " bytes" );

    // create response message
    buf.commit( length );
    std::istream is( &buf );
    boost::archive::text_iarchive ar( is );
    const ::runjob::commands::response::KillJob::Ptr response( 
            new ::runjob::commands::response::KillJob
            );
    ar & *response;

    return response;
}

std::string
gatherDetails()
{
    std::ostringstream response;
    try {
        const bgq::utility::UserId uid;
        response << "Delivered by scheduler API user " << uid.getUser() << " with pid " << getpid();
        response << " on host " << boost::asio::ip::host_name();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return response.str();
}

int
kill(
        const Job::Id job,
        const int signal
    )
{
    const ::runjob::commands::request::KillJob::Ptr request(
            new ::runjob::commands::request::KillJob
            );
    request->_job = job;
    request->_signal = signal;
    request->_details = gatherDetails();
    boost::asio::io_service io_service;
    bgq::utility::portConfig::SocketPtr connection;

    try {
        connection = connect( io_service );
    } catch ( const bgq::utility::Connector::ResolveError& e ) {
        LOG_ERROR_MSG( e.what() );
        THROW_EXCEPTION(
                bgsched::runjob::ConnectionException,
                ConnectionErrors::CannotResolve,
                "Could not resolve the runjob server host or service name" 
                );
    } catch ( const bgq::utility::Connector::ConnectError& e ) {
        LOG_ERROR_MSG( e.what() );
        THROW_EXCEPTION(
                bgsched::runjob::ConnectionException,
                ConnectionErrors::CannotConnect,
                "Failed to connect to the runjob server"
                );
    }

    if ( !connection ) {
        THROW_EXCEPTION(
                InternalException,
                bgsched::InternalErrors::UnexpectedError,
                "Unexpected error connecting to the runjob server."
                );
    }

    LOG_DEBUG_MSG( "Connected to " << connection->lowest_layer().remote_endpoint() );

    ::runjob::commands::response::KillJob::Ptr response;
    try {
        response = send(
                connection,
                request
                );
    } catch ( const std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
        THROW_EXCEPTION(
                bgsched::runjob::ConnectionException,
                ConnectionErrors::LostConnection,
                "Lost connection to the runjob server"
                );
    }

    if ( !response ) {
        THROW_EXCEPTION(
                InternalException,
                bgsched::InternalErrors::UnexpectedError,
                "Unexpected error receiving response from the runjob server."
                );
    }

    switch ( response->getError() ) {
        case ::runjob::commands::error::success:
            LOG_DEBUG_MSG( "delivered signal " << signal << " to job " << job );
            return 0;
        case ::runjob::commands::error::job_not_found:
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::JobNotFound,
                    "job not found: " + boost::lexical_cast<std::string>( job )
                    );
        default:
            LOG_ERROR_MSG( ::runjob::commands::error::toString( response->getError() ) << ": " << response->getMessage() );
            return -1;
    }
}

} // runjob
} // bgsched
