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
#include "server/job/JobStatus.h"

#include "common/message/StdIo.h"

#include "common/commands/JobStatus.h"

#include "common/logging.h"

#include "server/mux/Connection.h"
#include "server/CommandConnection.h"
#include "server/Job.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
JobStatus::create(
        const Job::Ptr& job,
        const CommandConnection::Ptr& connection
        )
{
    const JobStatus::Ptr status(
            new JobStatus( job, connection )
            );
    job->strand().post(
            boost::bind(
                &JobStatus::impl,
                status
                )
            );
}

JobStatus::JobStatus(
        const Job::Ptr& job,
        const CommandConnection::Ptr& connection
        ) :
    _job( job ),
    _connection( connection )
{

}

void
JobStatus::impl()
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );
    LOG_TRACE_MSG( "impl" );

    const runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        const auto location = i.first;
        IoNode& node = i.second;

        runjob::commands::response::JobStatus::IoConnection io;
        io._location = location.get();
        io._computes = node.getComputes();
        io._outputStarted = node.output_started();
        io._drained = node.drained();
        io._killed = node.killed();
        io._hardwareFailure = node.hardwareFailure();
        io._ended = node.ended();
        io._error = node.error();
        io._exited = node.exited();
        io._loaded = node.loaded();
        io._running = node.running();
        io._signalInFlight = node.signalInFlight();

        response->_connections.push_back( io );
    }

    response->_killTimeout = _job->killTimer().expires();
    const mux::Connection::Ptr mux( _job->mux().lock() );
    if ( mux ) {
        try {
            response->_mux = boost::lexical_cast<std::string>(mux->getEndpoint());
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_WARN_MSG( e.what() );
        }
    }

    _connection->write( response );
}

} // job
} // server
} // runjob

