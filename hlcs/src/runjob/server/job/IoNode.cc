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
#include "server/job/IoNode.h"

#include "server/block/Compute.h"
#include "server/block/IoNode.h"

#include "server/cios/Connection.h"
#include "server/cios/Message.h"

#include "server/job/ExitStatus.h"
#include "server/job/Heartbeat.h"
#include "server/job/Signal.h"

#include "server/mux/Connection.h"

#include "common/error.h"
#include "common/logging.h"

#include "server/Job.h"
#include "server/Ras.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/MessageUtility.h>

#include <utility/include/ExitStatus.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

/*
<rasevent
  id="00062002" 
  category="Job" 
  component="MMCS"
  severity="INFO"
  message="The prolog program failed with error $(ERROR) on I/O node $(BG_LOC)."
  description="The prolog program is run after all the compute nodes have successfully loaded the executable. A non-zero return code prevents the job from starting."
  service_action="Check the configuration of the prolog program in the [cios.jobctld] section of bg.properties"
 />

<rasevent
  id="00062003" 
  category="Job" 
  component="MMCS"
  severity="INFO"
  message="The epilog program failed with error $(ERROR) on I/O node $(BG_LOC)."
  description="The epilog program is run after all of the compute nodes have terminated."
  service_action="Check the configuration of the epilog program in the [cios.jobctld] section of bg.properties"
 />
*/

IoNode::IoNode(
        const boost::shared_ptr<block::IoNode>& node,
        unsigned computes
        ) :
    _flags(),
    _numComputes( computes ),
    _drained( false ),
    _killed( false ),
    _hardwareFailure( false ),
    _ended( false ),
    _error( false ),
    _exited( false ),
    _loaded( false ),
    _setup( false ),
    _output_started( false ),
    _signalInFlight( false ),
    _node( node )
{

}

void
IoNode::writeControl(
        const cios::Message::Ptr& msg
        )
{
    if ( cios::Connection::Ptr control = _node->getControl() ) {
        control->write( msg );
        // fall through
    } else {
        LOG_WARN_MSG( "lost control connection" );
        return;
    }

    switch ( msg->type() ) {
        case bgcios::jobctl::SetupJob:
            _flags.set( Status::Setup );
            break;
        case bgcios::jobctl::LoadJob:
            _flags.set( Status::Loading );
            break;
        case bgcios::jobctl::StartJob:
            _flags.set( Status::ControlStarting );
            break;
        case bgcios::jobctl::CleanupJob:
            _flags.set( Status::Cleanup );
            break;
        case bgcios::jobctl::SignalJob:
            _signalInFlight = true;
            break;
        default:
            break;
    }
}

void
IoNode::writeData(
        const cios::Message::Ptr& msg
        )
{
    if ( cios::Connection::Ptr data = _node->getData() ) {
        data->write( msg );
    } else {
        LOG_WARN_MSG( "lost data connection" );
    }

    switch ( msg->type() ) {
        case bgcios::stdio::StartJob:
            _flags.set( Status::OutputStarting );
            break;
        default:
            break;
    }
}

void
IoNode::handleControl(
        const bgcios::MessageHeader* header,
        const cios::Message::Ptr& message,
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );

    if ( header->type == bgcios::jobctl::SetupJobAck && _flags.test(Status::Setup) ) {
        if ( header->returnCode != bgcios::Success ) {
            this->setupFailure( header, job );
        } else {
            this->setup( true );
        }
    } else if ( header->type == bgcios::jobctl::LoadJobAck && _flags.test(Status::Loading) ) {
        if ( header->returnCode != bgcios::Success ) {
            this->loadFailure( header, job );
        } else {
            this->loaded( true );
        }
    } else if ( header->type == bgcios::jobctl::StartJobAck && _flags.test(Status::ControlStarting) ) {
        if ( header->returnCode != bgcios::Success ) {
            this->startFailure( header, job ); 
        } else {
            _flags.set( Status::Running );
        }
    } else if ( header->type == bgcios::jobctl::ExitJob && _flags.test(Status::Running) ) {
        const boost::shared_ptr<bgcios::jobctl::ExitJobMessage> exit_job(
                message->as<bgcios::jobctl::ExitJobMessage>()
                );
        job->exitStatus().set( exit_job->status, exit_job->header.rank );
        this->exited( true );
    } else if ( header->type == bgcios::jobctl::CleanupJobAck && _flags.test(Status::Cleanup) ) {
        // handle epilog program failures
        const boost::shared_ptr<bgcios::jobctl::CleanupJobAckMessage> ack(
                message->as<bgcios::jobctl::CleanupJobAckMessage>()
                );
        if ( ack->epilogReturnCode != bgcios::Success ) {
            this->epilogFailure( message, job );
        }

        if ( header->returnCode != bgcios::Success ) {
            this->cleanupFailure( header, job );
        } else {
            // job should still end even if a cleanup failure happens
        }

        this->ended( true );
    } else if ( header->type == bgcios::jobctl::SignalJobAck ) {
        _signalInFlight = false;
    } else if ( header->type == bgcios::jobctl::StartToolAck ) {
        const boost::shared_ptr<bgcios::jobctl::StartToolAckMessage> ack(
                message->as<bgcios::jobctl::StartToolAckMessage>()
                );

        if ( header->returnCode != bgcios::Success ) {
            job->tools().failure( ack, _node->getLocation() );
            return;
        }

        job->tools().started( ack->toolId, _node->getLocation() );
    } else if ( header->type == bgcios::jobctl::EndToolAck ) {
        // nothing to do
    } else if ( header->type == bgcios::jobctl::ExitTool ) {
        const boost::shared_ptr<bgcios::jobctl::ExitToolMessage> etm(
                message->as<bgcios::jobctl::ExitToolMessage>()
                );
        job->tools().exited( etm, _node->getLocation() );

        const cios::Message::Ptr msg(
                cios::Message::create( bgcios::jobctl::ExitToolAck, job->id() )
                );
        msg->as<bgcios::jobctl::ExitToolAckMessage>()->toolId = etm->toolId;
        this->writeControl( msg );
    } else if ( header->type == bgcios::jobctl::ExitProcess ) {
        const boost::shared_ptr<bgcios::jobctl::ExitProcessMessage> exitProcess(
                message->as<bgcios::jobctl::ExitProcessMessage>()
                );
        job->exitStatus().set( exitProcess->status, header->rank );

        const bgq::utility::ExitStatus exit_status( exitProcess->status );
	
	const JobInfo::EnvironmentVector& envs = job->info().getEnvs();
	bool exitimm = false;
	BOOST_FOREACH( const Environment& i, envs ) 
	{
	   if(i.getKey() == "BG_EXITIMMEDIATELYONRC")
	   {
	       exitimm = atoi(i.getValue().c_str());
	       break;
	   }
	}
	
        if ( exit_status.exited() && ((exit_status.getExitStatus() == 1) || (exitimm && (exit_status.getExitStatus() != 0)))) {
            // deliver SIGTERM to remaining nodes in job
            Signal::create( job, SIGTERM );
        } else if ( exit_status.signaled() ) {
            switch ( exit_status.getSignal() ) {
                case bgcios::jobctl::SIGHARDWAREFAILURE:
                    // skip signal rebroadcast
                    break;
                default:
                    // deliver SIGKILL to remaining nodes in job
                    Signal::create( job, SIGKILL );
            }
        }
    } else if ( header->type == bgcios::jobctl::CheckToolStatusAck ) {
        const boost::shared_ptr<bgcios::jobctl::CheckToolStatusAckMessage> ack(
                message->as<bgcios::jobctl::CheckToolStatusAckMessage>()
                );
        job->heartbeat().pulse( job, _node->getLocation(), ack->activeTools );
    } else {
        LOG_WARN_MSG( 
                "unsupported message type " << bgcios::jobctl::toString( header->type ) << " " <<
                "for status " << _flags << " for rank " << header->rank
                );
    }
}

void
IoNode::handleData(
        const bgcios::MessageHeader* header,
        const cios::Message::Ptr& message,
        const Job::Ptr& job,
        const Callback& callback
        )
{
    if ( header->type == bgcios::stdio::StartJobAck && _flags.test(Status::OutputStarting) ) {
        if ( header->returnCode != bgcios::Success ) {
            this->startFailure( header, job ); 
        } else {
            this->output_started( true );
        }
        callback();
    } else if ( header->type == bgcios::stdio::CloseStdio ) {
        this->drained( true );
        callback();
    } else if ( header->type == bgcios::stdio::ReadStdin ) {
        LOG_TRACE_MSG( "stdin request from rank " << header->rank );

        const message::StdIo::Ptr msg( new message::StdIo() );
        msg->setType( runjob::Message::StdIn );
        msg->setClientId( job->client() );
        msg->setJobId( job->id() );
        msg->setRank( header->rank );
        msg->setLocation( _node->getLocation() );

        // number of bytes to read
        const boost::shared_ptr<bgcios::stdio::ReadStdinMessage> input(
                message->as<bgcios::stdio::ReadStdinMessage>()
                );
        msg->setLength( input->length );
        LOG_TRACE_MSG( "for " << input->length << " bytes" );

        job->queue().add( msg, callback );
    } else {
        LOG_WARN_MSG( 
                "unsupported message type " << bgcios::stdio::toString( header->type ) << " " <<
                "for rank " << header->rank
                );
        callback();
    }
}

bool
IoNode::drained(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _drained = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _drained;
}

bool
IoNode::setup(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _setup = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _setup;
}

bool
IoNode::loaded(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _loaded = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _loaded;
}

bool
IoNode::output_started(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _output_started = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _output_started;
}

bool
IoNode::exited(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _exited = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _exited;
}

bool
IoNode::error(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _error = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _error;
}

bool
IoNode::ended(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _ended = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _ended;
}

bool
IoNode::killed(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _killed = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _killed;
}

bool
IoNode::hardwareFailure(
        const boost::tribool& value
        )
{
    if ( !boost::logic::indeterminate(value) ) {
        _hardwareFailure = value;
        LOGGING_DECLARE_LOCATION_MDC( _node->getLocation() );
        LOG_TRACE_MSG( __FUNCTION__ );
    }

    return _hardwareFailure;
}

void
IoNode::setupFailure(
        const bgcios::MessageHeader* header,
        const Job::Ptr& job
        )
{
    LOG_WARN_MSG(
            "Setup failed with rc " << header->returnCode << ": " <<
            bgcios::returnCodeToString( header->returnCode )
            );

    this->error( true );

    boost::system::system_error error(
            boost::system::error_code(
                header->errorCode,
                boost::system::get_system_category()
                )
            );
    
    job->exitStatus().set(
            std::string("Setup failed on ") + boost::lexical_cast<std::string>(_node->getLocation()) + ": " +
            bgcios::returnCodeToString( header->returnCode ) + ", errno " + 
            boost::lexical_cast<std::string>( error.code().value() ) + " " +
            error.what(),
            runjob::error_code::job_failed_to_start
            );
}

void
IoNode::loadFailure(
        const bgcios::MessageHeader* header,
        const Job::Ptr& job
        )
{
    LOG_WARN_MSG(
            "Load failed with rc " << header->returnCode << ": " <<
            bgcios::returnCodeToString( header->returnCode )
            );

    this->error( true );

    boost::system::system_error error(
            boost::system::error_code(
                header->errorCode,
                boost::system::get_system_category()
                )
            );

    job->exitStatus().set(
            std::string("Load failed on ") + boost::lexical_cast<std::string>(_node->getLocation()) + ": " +
            bgcios::returnCodeToString( header->returnCode ) + ", errno " + 
            boost::lexical_cast<std::string>( error.code().value() ) + " " +
            error.what(),
            runjob::error_code::job_failed_to_start
            );
}

void
IoNode::startFailure(
        const bgcios::MessageHeader* header,
        const Job::Ptr& job
        )
{
    LOG_WARN_MSG( 
            "Start failed with rc " << header->returnCode << ": " <<
            bgcios::returnCodeToString( header->returnCode )
            );

    this->error( true );

    boost::system::system_error error(
            boost::system::error_code(
                header->errorCode,
                boost::system::get_system_category()
                )
            );

    if (
            header->returnCode == bgcios::PrologPgmError ||
            header->returnCode == bgcios::PrologPgmStartError
       )
    {
        this->prologFailure( header, job );
    }
   
    job->exitStatus().set(
            std::string("Start failed on ") + boost::lexical_cast<std::string>(_node->getLocation()) + ": " +
            bgcios::returnCodeToString( header->returnCode ) + ", errno " + 
            boost::lexical_cast<std::string>( error.code().value() ) + " " +
            error.what(),
            runjob::error_code::job_failed_to_start
            );

    // send nodes that have started a kill
    const cios::Message::Ptr msg(
            cios::Message::create( bgcios::jobctl::SignalJob, job->id() )
            );
    msg->as<bgcios::jobctl::SignalJobMessage>()->signo = SIGKILL;

    BOOST_FOREACH( IoNode::Map::value_type& i, job->io() ) {
        IoNode& node = i.second;
        if ( !node.running() ) continue;
        if ( node.killed() ) continue;
        node.killed( true );
        node.writeControl( msg );
    }
}

void
IoNode::cleanupFailure(
        const bgcios::MessageHeader* header,
        const Job::Ptr& job
        )
{
    LOG_WARN_MSG( 
            "Cleanup failed with rc " << header->returnCode << ": " <<
            bgcios::returnCodeToString( header->returnCode )
            );

    this->error( true );

    boost::system::system_error error(
            boost::system::error_code(
                header->errorCode,
                boost::system::get_system_category()
                )
            );

    job->exitStatus().set(
            std::string("Cleanup failed on ") + boost::lexical_cast<std::string>(_node->getLocation()) + ": " +
            bgcios::returnCodeToString( header->returnCode ) + ", errno " + 
            boost::lexical_cast<std::string>( error.code().value() ) + " " +
            error.what(),
            runjob::error_code::job_failed_to_cleanup
            );
}

void
IoNode::prologFailure(
        const bgcios::MessageHeader* header,
        const Job::Ptr& job
        ) 
{
    if ( job->exitStatus().getError() ) return;

    LOG_WARN_MSG(
            bgcios::returnCodeToString( header->returnCode )
            );

    // emit RAS event if this is the first I/O node to report a prolog failure
    Ras::create( Ras::PrologFailure ).
            detail( RasEvent::LOCATION, boost::lexical_cast<std::string>(_node->getLocation()) ).
            detail( "ERROR", boost::lexical_cast<std::string>(header->errorCode) ).
            block( job->block()->name() ).
            job( job->id() )
            ;
}

void
IoNode::epilogFailure(
        const cios::Message::Ptr& message,
        const Job::Ptr& job
        ) 
{
    if ( job->exitStatus().getError() ) return;

    const boost::shared_ptr<bgcios::jobctl::CleanupJobAckMessage> ack(
            message->as<bgcios::jobctl::CleanupJobAckMessage>()
            );
    
    LOG_WARN_MSG(
            bgcios::returnCodeToString( ack->epilogReturnCode )
            );

    // emit RAS event if this is the first I/O node to report a epilog failure
    Ras::create( Ras::EpilogFailure ).
        detail( RasEvent::LOCATION, boost::lexical_cast<std::string>(_node->getLocation()) ).
        detail( "ERROR", boost::lexical_cast<std::string>(ack->epilogErrorCode) ).
        block( job->block()->name() ).
        job( job->id() )
        ;

    job->exitStatus().set(
            std::string("epilog program failed on ") + boost::lexical_cast<std::string>(_node->getLocation()) + 
            " with error " +
            boost::lexical_cast<std::string>( ack->epilogErrorCode ),
            runjob::error_code::job_failed_to_cleanup
            );
}

} // job
} // server
} // runjob
