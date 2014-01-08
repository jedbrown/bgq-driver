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
#include "server/job/KillTimer.h"

#include "server/block/Compute.h"
#include "server/block/IoNode.h"
#include "server/cios/Message.h"
#include "server/job/Status.h"
#include "server/job/SubNodePacing.h"

#include "common/JobInfo.h"
#include "common/SubBlock.h"
#include "common/logging.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Ras.h"
#include "server/Server.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>

LOG_DECLARE_FILE( runjob::server::log );

/*
<rasevent
  id="00062000" 
  category="Job" 
  component="MMCS"
  severity="INFO"
  message="killing job $(JOB) timed out after $(TIMEOUT) seconds. $(NODE_COUNT) nodes are now unavailable."
  description="delivering a SIGKILL to a job timed out"
  service_action="Reboot the block."
 />
*/

namespace runjob {
namespace server {
namespace job {

KillTimer::KillTimer(
        const Server::Ptr& server,
        const JobInfo& info
        ) :
    _timer( server->getIoService() ),
    _force( false )
{
    if ( server->getOptions().getSim() ) {
        // simulation enables a way to force a kill timeout
        const JobInfo::EnvironmentVector& envs = info.getEnvs();
        BOOST_FOREACH( const Environment& i, envs ) {
            _force = ( i.getKey() == "RUNJOB_KILL_TIMEOUT" );
            if ( _force ) {
                LOG_WARN_MSG( "enabling forced kill timeout" );
                break;
            }
        }
    }
}

void
KillTimer::start(
        const size_t seconds,
        const Job::Ptr& job
        )
{
    const boost::posix_time::time_duration expires = _timer.expires_from_now();
    if ( !expires.is_not_a_date_time() ) {
        if ( expires.total_seconds() > static_cast<int32_t>(seconds) ) {
            LOG_INFO_MSG( "reducing timeout to " << seconds << " seconds" );
            // allow reducing kill timeout, fall through
        } else {
            BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "Kill timeout expires in " + boost::lexical_cast<std::string>( expires.total_seconds() ) + " seconds." )
                );
        }
    }

    LOG_DEBUG_MSG( "expires in " << seconds << " seconds" );
    _timer.expires_from_now( boost::posix_time::seconds(seconds) );
    _timer.async_wait(
            job->strand().wrap(
                boost::bind(
                    &KillTimer::handler,
                    this,
                    _1,
                    job,
                    seconds
                    )
                )
            );
}

void
KillTimer::stop()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    boost::system::error_code error;
    _timer.cancel( error );
    
    if ( error ) {
        LOG_WARN_MSG( "could not stop: " << boost::system::system_error( error ).what() );
    }
}

void
KillTimer::handler(
        const boost::system::error_code& error,
        const Job::Ptr& job,
        const size_t seconds
        )
{
    LOGGING_DECLARE_JOB_MDC( job->id() );

    if ( _force ) {
        // fall through
    } else if ( error == boost::asio::error::operation_aborted ) {
        LOG_TRACE_MSG( "aborted" );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "could not wait: " << boost::system::system_error(error).what() );
        return;
    }

    LOG_WARN_MSG( "timed out after " << seconds << " seconds" );

    // mark the compute nodes in use by this job as unavailable for future jobs
    job->block()->unavailable( job );

    // end the job forcefully
    job->setError( 
            "Delivering SIGKILL with status " +
            Status::toString( job->status().get() ) +
            " timed out after " + 
            boost::lexical_cast<std::string>(seconds) + 
            " seconds",
            error_code::kill_timeout 
            );
    job->remove();

    this->insertRas( job, seconds );

    this->breadcrumbs( job );

    this->cleanup( job );
}

void
KillTimer::insertRas(
        const Job::Ptr& job,
        const size_t timeout
        )
{
    const size_t nodes = this->getNodeCount( job );

    Ras::create( Ras::KillTimeout ).
        detail( "JOB", job->id() ).
        detail( "TIMEOUT", timeout ).
        detail( "NODE_COUNT", nodes ).
        block( job->block()->name() ).
        job( job->id() )
        ;
}

size_t
KillTimer::getNodeCount(
        const Job::Ptr& job
        ) const
{
    size_t result = 0;

    const JobInfo& info = job->info();
    const SubBlock& sub_block = info.getSubBlock();
    if ( sub_block.isValid() ) {
        const Shape& shape = sub_block.shape();
        result = 
            shape.a() *
            shape.b() *
            shape.c() *
            shape.d() *
            shape.e()
            ;
    } else {
        const block::Compute::Ptr& block = job->block();
        result = block->size();
    }

    return result;
}

void
KillTimer::breadcrumbs(
        const Job::Ptr& job
        ) const
{
    // log some useful information why killing this job may have timed out

    const Status::Value status( job->status().get() );
    switch ( status ) {
        case Status::Running:
        case Status::Cleanup:
            // fall through
            break;
        default:
            return;
    }

    BOOST_FOREACH( IoNode::Map::value_type& i, job->io() ) {
        LOGGING_DECLARE_LOCATION_MDC( i.first );
        IoNode& node = i.second;
        if ( node.exited() ) {
            LOG_INFO_MSG( "exited" );
        }
        if ( status == Status::Running ) {
            if ( node.drained() ) continue;
            LOG_INFO_MSG( "has not closed stdio for " << node.getComputes() << " compute nodes" );
        } else if ( status == Status::Cleanup ) {
            if ( node.ended() ) continue;
            LOG_INFO_MSG( "has not cleaned up for " << node.getComputes() << " compute nodes" );
        } else {
            BOOST_ASSERT( !"shouldn't get here" );
        }
    }
}

void
KillTimer::cleanup(
        const Job::Ptr& job
        ) const
{
    const cios::Message::Ptr msg( 
            cios::Message::create( bgcios::jobctl::CleanupJob, job->id() )
            );
    msg->as<bgcios::jobctl::CleanupJobMessage>()->killTimeout = true;

    BOOST_FOREACH( IoNode::Map::value_type& i, job->io() ) {
        IoNode& node = i.second;
        if ( node.cleanup() ) continue;

        if ( job->pacing() ) {
            job->pacing()->add( msg, job );
        } else {
            node.writeControl( msg );
        }
    }
}

} // job
} // server
} // runjob
