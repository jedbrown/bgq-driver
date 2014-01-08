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
#include "server/job/Signal.h"

#include "server/cios/Message.h"

#include "server/job/IoNode.h"
#include "server/job/Status.h"
#include "server/job/SubNodePacing.h"
#include "server/job/Transition.h"

#include "common/logging.h"

#include "server/Job.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Signal::create(
        const Job::Ptr& job,
        const int number,
        const size_t timeout,
        const Callback& callback
        )
{
    const boost::shared_ptr<Signal> result( new Signal(job, callback) );

    // dispatch used instead of post since the handler can be invoked immediately
    // should we already be inside of the strand
    job->strand().dispatch(
            boost::bind(
                &Signal::impl,
                result,
                number,
                timeout
                )
            );
}

Signal::Signal(
        const Job::Ptr& job,
        const Callback& callback
        ) :
    _job( job ),
    _callback( callback ),
    _error( runjob::commands::error::unknown_failure ),
    _message()
{

}

Signal::~Signal()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( _callback.empty() ) return;

    _callback( _error, _message.str() );
}

void
Signal::impl(
        const int number,
        const size_t timeout
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );

    // special case for client starting since the job has not started
    if ( _job->status().get() == Status::ClientStarting ) {
        _message << "Client disconnected";
        _job->setError( 
                _message.str(),
                error_code::job_failed_to_start
                );

        Transition( _job ).end();

        return;
    }

    // special case for DEBUG since the job has not yet started
    if ( _job->status().get() == Status::Debug ) {
        _error = runjob::commands::error::success;
        _message << "Tool setup aborted by signal " << number;
        _job->setError( 
                _message.str(),
                error_code::job_failed_to_start
                );

        Transition( _job ).end();

        return;
    }

    // job has to be running to signal it
    if ( _job->status().get() != Status::Running ) {
        _error = runjob::commands::error::job_status_invalid;
        switch( number ) {
            case SIGKILL:
            case bgcios::jobctl::SIGHARDWAREFAILURE:
                _message << "Waiting for job to begin " << Status::toString(Status::Running) << " before delivering signal " << number << ". ";
                this->startTimer( number, timeout );
                break;
            default:
                _message << "Cannot deliver signal " << number << " when status is " << std::string(_job->status()) << ". ";
                break;
        }

        LOG_WARN_MSG( _message.str() );
        return;
    }

    // another signal cannot be outstanding
    if ( _job->_outstandingSignal ) {
        _error = runjob::commands::error::job_already_dying;
        switch( number ) {
            case SIGKILL:
            case bgcios::jobctl::SIGHARDWAREFAILURE:
                _message <<
                    "Waiting for signal " << _job->_outstandingSignal << 
                    " to be delivered before delivering signal " << number << ". ";
                this->startTimer( number, timeout );
                break;
            default:
                _message <<
                    "Cannot deliver signal " << number << " when signal " <<
                    _job->_outstandingSignal << " is in flight. ";
                break;
        }

        LOG_WARN_MSG( _message.str() );
        return;
    }

    const cios::Message::Ptr msg(
            cios::Message::create( bgcios::jobctl::SignalJob, _job->id() )
            );
    msg->as<bgcios::jobctl::SignalJobMessage>()->signo = number;

    unsigned count = 0;

    BOOST_FOREACH( IoNode::Map::value_type& i, _job->io() ) {
        IoNode& node = i.second;
        if ( number == SIGKILL ) {
            if ( node.killed() ) continue;
            node.killed( true );
        }

        if ( number == bgcios::jobctl::SIGHARDWAREFAILURE ) {
            if ( node.hardwareFailure() ) continue;
            node.hardwareFailure( true );
        }
    
        count++;

        if ( _job->pacing() ) {
            _job->pacing()->add( msg, _job );
        } else {
            node.writeControl( msg );
        }
    }
    
    if ( count ) {
        LOG_INFO_MSG( "sent signal " << number );
        _job->_outstandingSignal = number;
    }

    if ( this->startTimer(number, timeout) ) {
        _error = runjob::commands::error::success;
        _message << "delivered signal " << number;
    }
}

bool
Signal::startTimer(
        const int number,
        const size_t timeout
        )
{
    try {
        if ( number == SIGKILL ) {
            _job->killTimer().start( timeout, _job );
        } else if ( number == bgcios::jobctl::SIGHARDWAREFAILURE ) {
            _job->heartbeat().start( _job );
        }

        return true;
    } catch ( const std::logic_error& e ) {
        LOG_WARN_MSG( e.what() );
        _error = runjob::commands::error::job_already_dying;
        _message << e.what();

        return false;
    }
}

} // job
} // server
} // runjob
