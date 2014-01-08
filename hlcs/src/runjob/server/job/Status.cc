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
#include "server/job/Status.h"

#include "server/database/Init.h"
#include "server/database/Update.h"

#include "server/job/Container.h"

#include "common/logging.h"

#include "server/Job.h"
#include "server/Server.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Status::Status(
        const Server::Ptr& server
        ) :
    _status( Invalid ),
    _database( server->getDatabase() ),
    _counters( server->getPerformanceCounters()->getJobs() ),
    _counter(),
    _started( false )
{

}

void
Status::set(
        const Value s,
        const Job::Ptr& job
        )
{
    // set and log status
    _status = s;
    LOG_INFO_MSG( this->toString(s) );

    // remember if job started running
    if ( !_started ) _started = ( _status == Running );

    // reset performance counter measuring this transition
    switch ( _status ) {
        case Running:
        case Terminating:
            // don't time this transition, just reset the counter
            _counter.reset();
            break;
        default:
            // time how long this transition will take
            // we don't care about Running since the job can run as
            // long as it wants to
            _counter = _counters->create()
                ->function( "server" )
                ->subFunction( boost::to_lower_copy(this->toString(s)) )
                ->id( job->id() )
                ->otherData( boost::lexical_cast<std::string>(job->io().size())  + "i" )
                ->mode( bgq::utility::performance::Mode::Value::Basic )
                ;

            if ( _status == Cleanup ) {
                // cleanup is not included as part of the job submission performance counters
                _counter->mode( bgq::utility::performance::Mode::Value::Extended );
            }
    }

    switch ( _status ) {
        case Cleanup:
        case Debug:
        case Loading:
        case Running:
        case Setup:
        case OutputStarting:
            // fall through
            break;
        default:
            // these values are not kept in the database
            return;
    }

    // update status in database
    const performance::Counters::JobContainer::Timer::Ptr counter(
            _counters->create()
            ->function( "server" )
            ->subFunction( "database update " + boost::to_lower_copy(this->toString(s)) )
            ->id( job->id() )
            ->mode( bgq::utility::performance::Mode::Value::Extended )
            );

    _database->getUpdate().execute(
            job->id(),
            _status
            );
}

} // job
} // server
} // runjob
