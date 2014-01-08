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
#include "server/job/Transition.h"

#include "common/message/Result.h"

#include "server/cios/Message.h"

#include "server/job/KillTimer.h"
#include "server/job/Signal.h"
#include "server/job/SubNodePacing.h"

#include "server/mux/Connection.h"

#include "common/logging.h"

#include "server/Job.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Transition::Transition(
        const Job::Ptr& job
        ) :
    _job( job )
{

}

std::size_t
Transition::next(
        const Status::Value current,
        IoNode::Map& io
        ) const
{
    // job leader support means we'll only get a single exit job message from
    // a single compute node
    bool exited = false;
    if ( current == Status::Running ) {
        BOOST_FOREACH( IoNode::Map::value_type& i, io ) {
            IoNode& node = i.second;
            exited = node.exited();
            if ( exited ) break;
        }
    }

    // count the number of I/O nodes with the status we are attempting
    // to transition to
    std::size_t count = 0;

    BOOST_FOREACH( IoNode::Map::value_type& i, io ) {
        IoNode& node = i.second;
        if ( current == Status::Setup ) {
            if ( node.setup() || node.error() ) {
                ++count;
            }
        } else if ( current == Status::Loading ) {
            if ( node.loaded() || node.error() ) {
                ++count;
            }
        } else if ( current == Status::OutputStarting ) {
            if ( node.output_started() || node.error() ) {
                ++count;
            }
        } else if ( current == Status::ControlStarting ) {
            if ( node.running() || node.error() ) {
                ++count;
            }
        } else if ( current == Status::Running ) {
            if ( exited && node.drained() ) {
                ++count;
            }
        } else if ( current == Status::Cleanup ) {
            if ( node.ended() ) {
                ++count;
            }
        } else if ( current == Status::Debug ) {
            // do nothing
        } else if ( current == Status::Invalid ) {
            // do nothing
        } else if ( current == Status::ClientStarting ) {
            // do nothing
        } else if ( current == Status::Terminating ) {
            // do nothing
        } else {
            LOG_FATAL_MSG( "unhandled status: " << Status::toString(current) );
            BOOST_ASSERT( !"unhandled status" );
        }
    }

    LOG_TRACE_MSG( count << " of " << io.size() << " nodes ready to transition" );
    return count;
}

void
Transition::run() const
{
    _job->status().set(
            Status::Running,
            _job
            );

    const mux::Connection::Ptr mux = _job->mux().lock();
    if ( !mux ) return;

    const message::Result::Ptr msg( new message::Result );
    msg->setClientId( _job->client() );
    msg->setJobId( _job->id() );
    mux->write( msg );

    _job->queue().drain();

    if ( _job->killTimer().expires().is_not_a_date_time() ) {
        // timer has not been started yet
    } else {
        Signal::create( _job, SIGKILL );
    }

    // there is a timing window where a job can end prior to all the
    // jobctl daemons on the I/O nodes reporting a StartJobAck.
    const std::size_t count = this->next( _job->status().get(), _job->io() );
    if ( count != _job->io().size() ) return;
    this->end();
}

void
Transition::end() const
{
    _job->status().set(
            Status::Cleanup,
            _job
            );

    const cios::Message::Ptr msg( 
            cios::Message::create( bgcios::jobctl::CleanupJob, _job->id() )
            );

    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        IoNode& node = i.second;
        if ( _job->pacing() ) {
            _job->pacing()->add( msg, _job );
        } else {
            node.writeControl( msg );
        }
    }
}

void
Transition::loaded() const
{
    _job->status().set(
            Status::ClientStarting,
            _job
            );

    if ( _job->queue().isClientDisconnected() ) {
        // end the job since the client will never start it
        _job->setError( 
                "client disconnected",
                error_code::job_failed_to_start
                );

        Transition( _job ).end();

        return;
    }

    const mux::Connection::Ptr mux = _job->mux().lock();
    if ( !mux ) return;

    const message::Result::Ptr msg( new message::Result );
    msg->setClientId( _job->client() );
    msg->setJobId( _job->id() );
    mux->write( msg );
}

} // job
} // server
} // runjob
