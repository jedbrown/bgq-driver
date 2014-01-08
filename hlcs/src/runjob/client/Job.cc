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
#include "client/Job.h"

#include "client/options/Parser.h"

#include "client/Debugger.h"
#include "client/ProctablePrompt.h"
#include "client/Input.h"
#include "client/MuxConnection.h"
#include "client/Output.h"
#include "client/Timeout.h"

#include "common/message/convert.h"

#include "common/Exception.h"
#include "common/JobInfo.h"
#include "common/logging.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

Job::Job(
        boost::asio::io_service& io_service,
        const boost::shared_ptr<const options::Parser>& options,
        bgq::utility::ExitStatus& exitStatus
        ) :
    _status( Invalid ),
    _options( options ),
    _info( _options->getJobInfo() ),
    _io_service( io_service ),
    _mux(),
    _timeout(),
    _stdin(),
    _stdout(),
    _stderr(),
    _exitStatus( exitStatus ),
    _id( getpid() ), // id is our PID until job starts
    _debugger( Debugger::create(io_service) )
{
    // validate job info
    this->validate();
}

Job::~Job()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
    LOG_DEBUG_MSG( "stopping I/O service" );
    _io_service.stop();
}

void
Job::validate()
{
    LOG_DEBUG_MSG( "block: " << _info.getBlock() );

    // ensure we have exe
    if ( _info.getExe().empty() ) {
        LOG_RUNJOB_EXCEPTION( error_code::job_failed_to_start, "missing executable" );
    }
    LOG_DEBUG_MSG( "exe: " << _info.getExe() );
    
    LOG_DEBUG_MSG( _info.getArgs().size() << " args" );
    BOOST_FOREACH( const std::string& arg, _info.getArgs() ) {
        LOG_DEBUG_MSG( "arg " << arg );
    }
    LOG_DEBUG_MSG( _info.getEnvs().size() << " envs" );
    BOOST_FOREACH( const runjob::Environment& env, _info.getEnvs() ) {
        LOG_DEBUG_MSG( env );
    }
}

void
Job::start(
        const int input,
        const int output,
        const int error
        )
{
    _status = Connecting;

    try {
        LOG_DEBUG_MSG( "creating mux connection" );
        const boost::shared_ptr<MuxConnection> mux(
                new MuxConnection(
                    _io_service,
                    shared_from_this(),
                    _options
                    )
                );
        _mux = mux;
        mux->start(
                boost::bind(
                    &Job::add,
                    shared_from_this(),
                    _1
                    )
                );
    } catch ( const boost::system::system_error& e ) {
        LOG_FATAL_MSG( "could not create connection to runjob_mux: " << e.what() );
        throw;
    }

    _stdin = Input::create( _io_service, _mux, input );
    _stdout = Output::create( _options, _io_service, output, "stdout" );
    _stderr = Output::create( _options, _io_service, error, "stderr" );
}

void
Job::add(
        const boost::system::error_code& error
        )
{
    if ( error ) {
        LOG_ERROR_MSG( "could not connect: " << error.message() );
        return;
    }

    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    const message::InsertJob::Ptr msg( new message::InsertJob() );
    msg->setJobInfo(_info);
    msg->setDuration( _options->getDuration() );

    mux->write( msg );

    _status = Inserting;
    LOG_DEBUG_MSG( "set status to " << this->toString(_status) );

    _timeout.reset(
            new Timeout( _io_service, shared_from_this() )
            );
    _timeout->start( _options->getTimeout() );
}

bool
Job::handle(
        const Message::Ptr& msg
        )
{
    LOG_TRACE_MSG( "status " << this->toString(_status) );
    LOG_TRACE_MSG( "handling " << Message::toString( msg->getType() ) << " message" );

    if ( _status == Inserting ) {
        this->inserting( msg );
        return true;
    }

    if ( _status == Starting ) {
        this->starting( msg );
        return true;
    }

    if ( _status == Debug ) {
        this->debug( msg );
        return true;
    }

    if ( _status == Running ) {
        this->running( msg );
        return true;
    }

    LOG_WARN_MSG(
            "unsupported status " << this->toString(_status) << " to handle message type " << 
            Message::toString(msg->getType())
            );
    return false;
}

void
Job::inserting(
        const Message::Ptr& msg
        )
{
    if ( msg->getType() != Message::Result ) {
        LOG_WARN_MSG(
                "unsupported message type " << Message::toString( msg->getType() ) <<
                " for status " << this->toString(_status)
                );

        return;
    }

    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    const message::Result::ConstPtr result = Message::get<message::Result>( msg );
    const error_code::rc error = result->getError();
    const std::string& message = result->getMessage();
    if ( error ) {
        LOG_FATAL_MSG( "could not start job: " << error_code::toString(error) );
        if ( !message.empty() ) {
            LOG_FATAL_MSG( message );
        }

        _status = Terminated;

        return;
    }

    // remember job ID
    _id = msg->getJobId();

    if ( _debugger->attached() ) {
        // start tool
        _status = Debug;
        const message::StartTool::Ptr start( new message::StartTool() );
        start->_description = _debugger->tool();
        mux->write( start );
    } else if ( _options->getTool().getExecutable().empty() ) {
        // start job
        _status = Starting;
        const message::StartJob::Ptr start( new message::StartJob() );
        mux->write( start );
    } else {
        // start tool
        _status = Debug;
        const message::StartTool::Ptr start( new message::StartTool() );
        start->_description = _options->getTool();
        mux->write( start );
    }
}

void
Job::starting(
        const Message::Ptr& msg
        )
{
    if ( msg->getType() != Message::Result ) {
        LOG_WARN_MSG(
                "unsupported message type " << Message::toString( msg->getType() ) <<
                " for status " << this->toString(_status)
                );

        return;
    }

    const message::Result::ConstPtr result = Message::get<message::Result>( msg );
    const error_code::rc error = result->getError();
    const std::string& message = result->getMessage();
    if ( error ) {
        LOG_FATAL_MSG( "could not start job: " << error_code::toString(error) );
        if ( !message.empty() ) {
            LOG_FATAL_MSG( message );
        }
        _status = Terminated;

        return;
    }

    LOG_INFO_MSG( "job " << _id << " started" );
    _status = Running;

    // start monitoring for a debugger to attach
    _debugger->start( _mux );
}

void
Job::debug(
        const Message::Ptr& msg
        )
{
    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    if ( msg->getType() == Message::Result ) {
        const message::Result::ConstPtr result = Message::get<message::Result>( msg );
        const error_code::rc error = result->getError();
        const std::string& message = result->getMessage();

        LOG_FATAL_MSG( "could not start tool: " << error_code::toString(error) );
        if ( !message.empty() ) {
            LOG_FATAL_MSG( message );
        }
        _status = Terminated;

        return;
    } else if ( msg->getType() == Message::Proctable ) {
        // fall through
    } else {
        LOG_WARN_MSG(
                "unsupported message type " << Message::toString( msg->getType() ) <<
                " for status " << this->toString(_status)
                );

        return;
    }

    // remember job ID
    _id = msg->getJobId();

    if ( _debugger->attached() ) {
        _debugger->fillProctable( 
                Message::get<message::Proctable>( msg ),
                _info
                );

        _status = Job::Starting;
        const message::StartJob::Ptr start( new message::StartJob() );
        start->setJobId( _id );
        mux->write( start );
    } else {
        // wait for user to start job
        const ProctablePrompt::Ptr prompt(
                ProctablePrompt::create( _io_service, _mux, _id, _status )
                );
        if ( prompt ) {
            prompt->start(
                    Message::get<message::Proctable>( msg )
                    );
        } else {
            LOG_DEBUG_MSG( "did not create proctable prompt" );
        }
    }
}

void
Job::running(
        const Message::Ptr& msg
        )
{
    if ( msg->getType() == Message::StdOut ) {
        _stdout->write( Message::get<message::StdIo>(msg) );
    } else if ( msg->getType() == Message::StdError ) {
        _stderr->write( Message::get<message::StdIo>(msg) );
    } else if ( msg->getType() == Message::StdIn ) {
        const message::StdIo::Ptr stdio = boost::static_pointer_cast<message::StdIo>(msg);
        _stdin->read( 
                stdio->getRank(),
                stdio->getLength(),
                stdio->getLocation()
                );
    } else if ( msg->getType() == Message::Proctable ) {
        _debugger->fillProctable( 
                Message::get<message::Proctable>( msg ),
                _info
                );
    } else if ( msg->getType() == Message::Result ) {
        // tool fail to start
        _debugger->handle( 
                Message::get<message::Result>( msg )
                );
    } else if ( msg->getType() == Message::ExitJob ) {
        const message::ExitJob::ConstPtr exit_job_msg = Message::get<message::ExitJob>( msg );
        const error_code::rc error = exit_job_msg->_error;
        const std::string& message = exit_job_msg->_message;
        _status = Terminated;

        if ( !error ) {
            _exitStatus = bgq::utility::ExitStatus( exit_job_msg->_status );
            if ( _exitStatus.exited() ) {
                LOG_INFO_MSG( "exited with status " << _exitStatus.getExitStatus() );
                if ( _exitStatus.getExitStatus() && !message.empty() ) {
                    LOG_WARN_MSG( message );
                }
            } else if ( _exitStatus.signaled() ) {
                LOG_WARN_MSG( "terminated by signal " << _exitStatus.getSignal() );
                if ( !message.empty() ) {
                    LOG_WARN_MSG( message );
                }
            } else {
                LOG_WARN_MSG( "terminated with status " << _exitStatus.get() );
            }
            if ( !exit_job_msg->_rasCount.empty() ) {
                for (
                        std::map<std::string,unsigned>::const_iterator i = exit_job_msg->_rasCount.begin();
                        i != exit_job_msg->_rasCount.end();
                        ++i
                    )
                {
                    LOG_WARN_MSG( i->second << " " << i->first <<" RAS event" << (i->second == 1 ? "" : "s") );
                }
            }
            if ( !exit_job_msg->_rasMessage.empty() ) {
                LOG_WARN_MSG( "most recent RAS event text: " << exit_job_msg->_rasMessage );
                LOG_WARN_MSG( "with severity " << exit_job_msg->_rasSeverity );
            }
        } else {
            LOG_FATAL_MSG( "terminated due to: " << error_code::toString(error) );
            if ( !message.empty() ) {
                LOG_FATAL_MSG( message );
            }
            if ( !exit_job_msg->_rasCount.empty() ) {
                for (
                        std::map<std::string,unsigned>::const_iterator i = exit_job_msg->_rasCount.begin();
                        i != exit_job_msg->_rasCount.end();
                        ++i
                    )
                {
                    LOG_FATAL_MSG( i->second << " " << i->first <<" RAS event" << (i->second == 1 ? "" : "s") );
                }
            }
            if ( !exit_job_msg->_rasMessage.empty() ) {
                LOG_FATAL_MSG( "most recent RAS event text: " << exit_job_msg->_rasMessage );
                LOG_FATAL_MSG( "with severity " << exit_job_msg->_rasSeverity );
            }
        }

        // log nodes that experienced a software failure event and are no
        // longer available for future jobs
        if ( !exit_job_msg->_nodes.empty() ) {
            LOG_WARN_MSG( exit_job_msg->_nodes.size() << " nodes unavailable" );
            BOOST_FOREACH( const auto& i, exit_job_msg->_nodes ) {
                LOG_WARN_MSG( i._location << " at " << i._coordinates );
            }
        }
    } else {
        LOG_WARN_MSG(
                "unsupported message type " << Message::toString( msg->getType() ) << 
                " for status " << this->toString(_status)
                );
    }
}

void
Job::kill(
        const int signal
        )
{
    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    LOG_DEBUG_MSG( "sending signal " << signal );
    const message::KillJob::Ptr msg( new message::KillJob() );
    msg->_signal = signal;
    mux->write(msg);
}

} // client
} // runjob
