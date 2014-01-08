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
#include "server/handlers/locate_rank/Impl.h"

#include "server/handlers/locate_rank/Exception.h"
#include "server/handlers/locate_rank/Job.h"
#include "server/handlers/locate_rank/SecurityCheck.h"

#include "common/logging.h"

#include "server/CommandConnection.h"
#include "server/Server.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/date_time/c_local_time_adjustor.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

Impl::Impl(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _request(),
    _response( new runjob::commands::response::LocateRank ),
    _connection(),
    _error( runjob::commands::error::unknown_failure ),
    _message()
{

}

Impl::~Impl()
{
    if ( !_connection ) return;

    _response->setError( _error );

    if ( !_message.str().empty() ) {
        _response->setMessage( _message.str() );
        LOG_WARN_MSG( _response->getMessage() );
    }

    _connection->write( _response );
}

void
Impl::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _request = boost::static_pointer_cast<runjob::commands::request::LocateRank>( request );
    _connection = connection;
    
    LOGGING_DECLARE_JOB_MDC( _request->_job );

    try {
        const cxxdb::ConnectionPtr db(
                BGQDB::DBConnectionPool::Instance().getConnection()
                );
        if ( !db ) {
            _error = runjob::commands::error::database_error;
            _message << "could not get database connection";
            return;
        }

        const Job::Ptr job = Job::create( db, _request->_job );
        LOGGING_DECLARE_BLOCK_MDC( job->block() );

        SecurityCheck security( _server, _connection );
        if ( !security(job) ) {
            _message << security.error();
            _error = runjob::commands::error::permission_denied;
            return;
        }

        this->validateMappingFile( job );

        job->find( _request->_rank );

        // getting here is a success
        _response->_block = job->block();
        _response->_location = job->find( _request->_rank );
        _error = runjob::commands::error::success;
    } catch ( const Exception& e ) {
        _error = e.error();
        _message << e.what();
    } catch ( const std::exception& e ) {
        _message << e.what();
    }
}

void
Impl::validateMappingFile(
        const Job::Ptr& job
        )
{
    const Mapping& mapping( job->mapping() );
    if ( mapping.type() != Mapping::Type::File ) return;

    // ensure we can read the file
    if ( access(mapping.value().c_str(), R_OK) == -1 ) {
        const int error = errno;
        char buf[256];
        BOOST_THROW_EXCEPTION(
                Exception(
                    std::string() +
                    "Could not read mapping file (" +
                    strerror_r(error, buf, sizeof(buf)) + "): " +
                    mapping.value(),
                    runjob::commands::error::permission_denied
                    )
                );
    }
    LOG_DEBUG_MSG( "Can read mapping file: " << mapping.value() );

    // get utc time when the mapping file was last modified
    const boost::filesystem::path path( mapping );
    const boost::posix_time::ptime modified_utc(
            boost::posix_time::from_time_t(
                boost::filesystem::last_write_time( path )
                )
            );

    // convert into our local time zone
    typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;
    const boost::posix_time::ptime modified_local = local_adj::utc_to_local( modified_utc );

    if ( modified_utc > job->startTime() ) {
        _message <<
                "mapping file '" << mapping << "' has been modified " <<
                "(" << modified_local << ") " <<
                "after the job started at " << job->startTime() << ". " <<
                "The results may not be accurate."
                ;
    }
}

} // locate_rank
} // handlers
} // server
} // runjob
