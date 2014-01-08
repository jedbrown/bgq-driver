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
#include "server/job/Container.h"

#include "common/Exception.h"
#include "common/logging.h"

#include "server/cios/Connection.h"

#include "server/block/Compute.h"

#include "server/database/Init.h"
#include "server/database/Insert.h"

#include "server/job/EndOfFile.h"
#include "server/job/CheckIoLinks.h"
#include "server/job/Signal.h"

#include "server/mux/ClientContainer.h"
#include "server/mux/Connection.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Container::Container(
        const Server::Ptr& server
        ) :
    _options( server->getOptions() ),
    _io_service( server->getIoService() ),
    _strand( _io_service ),
    _jobs(),
    _database( server->getDatabase() ),
    _counters( server->getPerformanceCounters()->getJobs() )
{

}

Container::~Container()
{

}

void
Container::add(
        const runjob::server::Job::Ptr& job,
        const Create::Callback& callback,
        const size_t nodesUsed,
        const performance::Counters::JobContainer::Timer::Ptr& arbitration,
        const performance::Counters::JobContainer::Timer::Ptr& security
        )
{
    _strand.post(
            boost::bind(
                &Container::addImpl,
                this,
                job,
                callback,
                nodesUsed,
                arbitration,
                security
                )
            );
}

void
Container::addImpl(
        const runjob::server::Job::Ptr& job,
        const Create::Callback& callback,
        const size_t nodesUsed,
        const performance::Counters::JobContainer::Timer::Ptr& arbitration,
        const performance::Counters::JobContainer::Timer::Ptr& security
        )
{
    LOGGING_DECLARE_BLOCK_MDC( job->info().getBlock() );
    LOGGING_DECLARE_JOB_MDC( job->client() );

    // stop arbitration timer 
    arbitration->stop();

    if ( job->id() ) {
        // ID already assigned means we are reconnecting and don't need to 
        // insert this job into the bgqjob table since it is already there
        BOOST_ASSERT(
                _jobs.insert(
                    std::make_pair( job->id(), job )
                    ).second
                );
        if ( callback ) callback( error_code::success, job );

        return;
    }

    // start database insertion timer
    const performance::Counters::JobContainer::Timer::Ptr timer(
            _counters->create()
                ->function( "server")
                ->subFunction( "database insertion")
                ->mode( bgq::utility::performance::Mode::Value::Basic )
            );
    timer->dismiss();

    try {
        _database->getInsert().execute(
                job,
                nodesUsed
                );
        LOG_INFO_MSG( "assigned id " << job->id() );
    } catch ( const Exception& e ) {
        job->setError( e.what(), e.getError() );
        return;
    }

    // set ID and un-dismiss the outstanding performance counters
    // that didn't have job IDs assigned to them yet
    arbitration->id( job->id() )->dismiss( false );
    security->id( job->id() )->dismiss( false );
    timer->id( job->id() )->dismiss( false );;

    BOOST_ASSERT(
            _jobs.insert(
                std::make_pair( job->id(), job )
                ).second
            );

    CheckIoLinks::create( job );
}

void
Container::remove(
        const BGQDB::job::Id id
        )
{
    _strand.post(
            boost::bind(
                &Container::removeImpl,
                this,
                id
                )
            );
}

void
Container::removeImpl(
        const BGQDB::job::Id id
        )
{
    const Jobs::iterator i = _jobs.find(id);
    if ( i != _jobs.end() ) {
        _jobs.erase(i);
        LOG_TRACE_MSG( "removed job " << id );
    } else {
        LOG_WARN_MSG( "could not find job " << id << " to remove" );
    }
}

void
Container::find(
        const BGQDB::job::Id id,
        const FindCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::findImpl,
                this,
                id,
                callback
                )
            );
}

void
Container::get(
        const GetCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::getImpl,
                this,
                callback
                )
            );
}

void
Container::getImpl(
        const GetCallback& callback
        )
{
    callback( _jobs );
}

void
Container::eof(
        const Uci& location
        )
{
    _strand.post(
            boost::bind(
                &Container::ioEofImpl,
                this,
                location
                )
            );
}

void
Container::eof(
        const block::Compute::Ptr& block
        )
{
    _strand.post(
            boost::bind(
                &Container::blockEofImpl,
                this,
                block
                )
            );
}

void
Container::findImpl(
        const BGQDB::job::Id id,
        const FindCallback& callback
        )
{
    Job::Ptr job;

    const Jobs::iterator i = _jobs.find(id);
    if ( i != _jobs.end() ) {
        job = i->second;
    }

    callback( job );
}

void
Container::ioEofImpl(
        const Uci& location
        )
{
    LOGGING_DECLARE_LOCATION_MDC( location );

    BOOST_FOREACH( const Jobs::value_type& i, _jobs ) {
        const Job::Ptr& job = i.second;
        EndOfFile::create( job, location );
    }
}

void
Container::blockEofImpl(
        const block::Compute::Ptr& block
        )
{
    LOGGING_DECLARE_BLOCK_MDC( block->name() );

    unsigned count = 0;

    BOOST_FOREACH( const Jobs::value_type& i, _jobs ) {
        const Job::Ptr& job = i.second;
        if ( job->block() == block ) {
            // kill job
            Signal::create( job, SIGKILL );

            // leave an informative message
            job->setError(
                    "Aborted due to block " + block->name() + " removal",
                    error_code::block_invalid
                    );
            ++count;
        }
    }
    if ( count ) {
        LOG_INFO_MSG( 
                "killed " << count <<
                " job" <<
                (count == 1 ? "" : "s")
                );
    }
}

} // job
} // server
} // runjob
