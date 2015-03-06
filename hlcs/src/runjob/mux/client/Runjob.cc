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
#include "mux/client/Runjob.h"

#include "common/message/InsertJob.h"
#include "common/message/ExitJob.h"
#include "common/message/KillJob.h"
#include "common/message/Proctable.h"
#include "common/message/Result.h"
#include "common/message/StartTool.h"

#include "common/defaults.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/properties.h"

#include "mux/client/Connection.h"
#include "mux/client/Container.h"
#include "mux/client/Credentials.h"

#include "mux/performance/Counters.h"

#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include <utility/include/ScopeGuard.h>

#include <boost/bind/protect.hpp>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace client {

Runjob::Runjob(
        const Multiplexer::Ptr& mux,
        const Id& id
        ) :
    _timer( mux->getIoService() ),
    _strand( mux->getIoService() ),
    _status(),
    _id( id ),
    _connection( new Connection(mux, _id) ),
    _server( mux->getServer(), _id ),
    _timers( _id, mux->getCounters()->getContainer() ),
    _plugin(),
    _credentials(),
    _jobId( 0 ),
    _mux( mux ),
    _startTool( false )
{

}

Runjob::~Runjob()
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOGGING_DECLARE_USER_MDC( _credentials ? _credentials->getUid()->getUser() : "nobody" );
    if ( _status == Status::Initializing ) {
        LOG_DEBUG_MSG( __FUNCTION__ );
    } else {
        LOG_INFO_MSG( __FUNCTION__ );
    }
    if ( Multiplexer::Ptr mux = _mux.lock() ) {
        mux->getClientContainer()->remove( _id );
    }
    _connection->stop();
}

void
Runjob::start()
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_DEBUG_MSG( __FUNCTION__ );

    // start initialization timer
    _timers.start_init();

    // obtain credentials
    _credentials.reset(
            new Credentials(
                _id,
                _connection->getSocket().native()
                )
            );

    // get plugin
    const Multiplexer::Ptr mux( _mux.lock() );
    if ( mux ) {
       mux->getPlugin()->get(
            boost::bind(
                &Runjob::handlePlugin,
                shared_from_this(),
                _1
                )
            );
    }
}

void
Runjob::handlePlugin(
        const runjob::mux::Plugin::WeakPtr& plugin
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOGGING_DECLARE_USER_MDC( _credentials->getUid()->getUser() );
    LOG_TRACE_MSG( __FUNCTION__ );

    _plugin.reset(
            new Plugin( _id, _timers, plugin )
            );

    const Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    // add self to container
    mux->getClientContainer()->add(
            shared_from_this(),
            // start handling client requests after add is complete
            boost::bind(
                &Connection::start,
                _connection,
                // protect used here to prevent outer binder
                // from evaluating inner bind
                boost::protect(
                    boost::bind(
                        &Runjob::handleRequest,
                        shared_from_this(),
                        _1,
                        _2
                        )
                    )
                )
            );

    // now we wait
    size_t timeout( defaults::MuxClientTimeout );
    const std::string key( "client_timeout" );
    std::string value;
    try {
        value = mux->getOptions().getProperties()->getValue(
                PropertiesSection,
                key
                );
        timeout = boost::lexical_cast<size_t>( value );
        if ( timeout == 0 ) {
            timeout = defaults::MuxClientTimeout;
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG(
                "garbage " << key << " value (" << value << ") from [" << PropertiesSection << "], " <<
                "using default value: " << timeout
                );
    } catch ( const std::exception& e ) {
        LOG_TRACE_MSG(
                "missing " << key << " from [" << PropertiesSection << "] section, " <<
                "using default value: " << timeout
                );
    }

    LOG_TRACE_MSG( "timing out after " << timeout << " seconds" );
    _timer.expires_from_now( boost::posix_time::seconds(timeout) );
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &Runjob::timeout,
                    shared_from_this(),
                    _1
                    )
                )
            );
}

void
Runjob::handleRequest(
        const runjob::Message::Ptr& message,
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOGGING_DECLARE_USER_MDC( _credentials->getUid()->getUser() );
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !message ) {
        if ( error ) {
            LOG_DEBUG_MSG( "Could not read: " << error.message() << "(" << Status::toString(_status.get()) << ")" );
        }
        if ( _status == Status::Terminated || _status == Status::Error ) {
            // this is ok
        } else if ( _startTool && _status == Status::Debug ) {
            // also ok
        } else if ( _status == Status::Initializing ) {
            // this is also ok but we should stop the initialization timer since
            // it will just timeout eventually otherwise
            boost::system::error_code ec;
            _timer.cancel( ec );
            if ( ec ) {
                LOG_WARN_MSG( __FUNCTION__ << "(" << __LINE__ << ") could not cancel: " << boost::system::system_error(ec).what() );
            }
        } else {
            // assume client forcefully aborted
            LOG_WARN_MSG( "eof " << Status::toString(_status.get()) );

            const message::KillJob::Ptr msg( new message::KillJob() );
            msg->_clientDisconnected = true;
            _server.send( msg, _jobId );

            _timer.expires_from_now(
                    boost::posix_time::seconds(60)
                    );
            _timer.async_wait(
                    _strand.wrap(
                        boost::bind(
                            &client::Runjob::clientDisconnected,
                            shared_from_this(),
                            _1
                            )
                        )
                    );
        }

        return;
    }

    if ( _status == Status::Initializing ) {
        boost::system::error_code ec;
        _timer.cancel( ec );
        if ( ec ) {
            LOG_WARN_MSG( __FUNCTION__ << "(" << __LINE__ << ") could not cancel: " << boost::system::system_error(ec).what() );
        }

        runjob::Message::Ptr out;
        if ( message->getType() == runjob::Message::InsertJob ) {
            const message::InsertJob::Ptr ijm(
                    boost::static_pointer_cast<message::InsertJob>( message )
                    );

            _status.set( Status::Inserting );

            // stop initialization timer
            _timers.stop_init();

            // set credentials in message
            ijm->getJobInfo().setPid( _credentials->getPid() );
            ijm->getJobInfo().setUserId( _credentials->getUid() );

            // remember time duration to parse arguments
            // invoke plugin
            const message::Result::Ptr result( new message::Result );
            _plugin->verify(
                    _credentials->getPid(),
                    ijm->getJobInfo(),
                    result
                    );
            if ( result->getError() != runjob::error_code::success ) {
                this->handleImpl( result );
                return;
            }

            _timers.set_argument_parsing(
                    ijm->getDuration()
                    );
            out = ijm;
            // fall through
        } else if ( message->getType() == runjob::Message::StartTool ) {
            const message::StartTool::Ptr stm(
                    boost::static_pointer_cast<message::StartTool>( message )
                    );
            stm->_uid = _credentials->getUid();
            _jobId = stm->getJobId();

            _status.set( Status::Debug );
            _startTool = true;

            out = stm;

            // fall through
        } else {
            LOG_ERROR_MSG(
                    "invalid message type (" <<
                    runjob::Message::toString( static_cast<runjob::Message::Type>(message->getType()) ) <<
                    ") for status (" << _status << ")"
                    );
            return;
        }

        _server.send(
                out,
                _jobId,
                boost::bind(
                    &Runjob::serverCallback,
                    shared_from_this(),
                    _1
                    )
                );

        return;
    }

    if ( _status == Status::Running ) {
        if ( message->getType() == runjob::Message::StdIn ) {
            // fall through
        } else if ( message->getType() == runjob::Message::KillJob ) {
            // fall through
        } else if ( message->getType() == runjob::Message::StartTool ) {
            const message::StartTool::Ptr stm(
                    boost::static_pointer_cast<message::StartTool>( message )
                    );
            stm->_uid = _credentials->getUid();
            // fall through
        } else {
            LOG_ERROR_MSG(
                    "invalid message type (" <<
                    runjob::Message::toString( static_cast<runjob::Message::Type>(message->getType()) ) <<
                    ") for status (" << _status << ")"
                    );

            return;
        }
    } else if ( _status == Status::Inserting ) {
        if ( message->getType() == runjob::Message::StartJob ) {
            _status.set( Status::Starting );
            // fall through
        } else if ( message->getType() == runjob::Message::StartTool ) {
            const message::StartTool::Ptr stm(
                    boost::static_pointer_cast<message::StartTool>( message )
                    );
            stm->_uid = _credentials->getUid();
            _status.set( Status::Debug );
            // fall through
        } else if ( message->getType() == runjob::Message::KillJob ) {
            // fall through
        } else {
            LOG_ERROR_MSG(
                    "invalid message type (" <<
                    runjob::Message::toString( static_cast<runjob::Message::Type>(message->getType()) ) <<
                    ") for status (" << _status << ")"
                    );

            return;
        }
    } else if ( _status == Status::Debug ) {
        if ( message->getType() == runjob::Message::StartJob ) {
            _status.set( Status::Starting );
            // fall through
        } else if ( message->getType() == runjob::Message::KillJob ) {
            // fall through
        } else {
            LOG_ERROR_MSG(
                    "invalid message type (" <<
                    runjob::Message::toString( static_cast<runjob::Message::Type>(message->getType()) ) <<
                    ") for status (" << _status << ")"
                    );

            return;
        }
    } else if ( _status == Status::Starting ) {
        if ( message->getType() == runjob::Message::KillJob ) {
            // fall through
        } else {
            LOG_ERROR_MSG(
                    "invalid message type (" <<
                    runjob::Message::toString( static_cast<runjob::Message::Type>(message->getType()) ) <<
                    ") for status (" << _status << ")"
                    );

            return;
        }
    } else {
        LOG_ERROR_MSG(
                "invalid message type (" <<
                runjob::Message::toString( static_cast<runjob::Message::Type>(message->getType()) ) <<
                ") for status (" << _status << ")"
                );

        return;
    }

    // forward message to server
    // the handler will start reading the next message, which will be forwarded
    // to this object's Runjob::handleRequest() method
    _server.send(
            message,
            _jobId,
            boost::bind(
                &Connection::start,
                _connection,
                // protect used here to prevent outer binder
                // from evaluating innner bind
                boost::protect(
                    boost::bind(
                        &Runjob::handleRequest,
                        shared_from_this(),
                        _1,
                        _2
                        )
                    )
                )
            );
}

void
Runjob::handle(
        const runjob::Message::Ptr& msg
        )
{
    _strand.post(
            boost::bind(
                &Runjob::handleImpl,
                shared_from_this(),
                msg
                )
            );
}

void
Runjob::handleImpl(
        const runjob::Message::Ptr& msg
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOGGING_DECLARE_USER_MDC( _credentials->getUid()->getUser() );
    LOG_TRACE_MSG( "handling " << runjob::Message::toString( msg->getType() ) << " message" );
    switch ( _status.get() ) {
        case Status::Debug:     this->handleDebug( msg );       break;
        case Status::Inserting: this->handleInserting( msg );   break;
        case Status::Starting:  this->handleStarting( msg );    break;
        case Status::Running:   this->handleRunning( msg );     break;
        case Status::Initializing:
        case Status::Error:
        case Status::Terminated:
            LOG_WARN_MSG(
                    "unsupported message type " <<
                    runjob::Message::toString( msg->getType() ) <<
                    " for status " << _status
                    );
            break;
        default:
            LOG_FATAL_MSG( "unhandled status value: " << _status );
            BOOST_ASSERT( !"unhandled status value" );
    }
}

void
Runjob::handleDebug(
        const runjob::Message::Ptr& msg
        )
{
    if ( msg->getType() == runjob::Message::Proctable ) {
        const message::Proctable::ConstPtr proctable = runjob::Message::get<message::Proctable>( msg );

        // remember job ID
        _jobId = msg->getJobId();
        LOG_INFO_MSG( "job " << _jobId << " tool started" );

        // update job ID of init timer, but not for start_tool clients
        if ( !_startTool ) _timers.update( _jobId );

        // for start_tool clients, we are done
        if ( _startTool ) _status.set( Status::Terminated );

        _connection->write( msg, _status );
    } else if ( msg->getType() == runjob::Message::Result ) {
        const message::Result::ConstPtr result = runjob::Message::get<message::Result>( msg );
        _status.set( Status::Error );
        _plugin->startError(
                _credentials->getPid(),
                result->getError(),
                result->getMessage()
                );

        _connection->write( msg, _status );

        boost::system::error_code ec;
        _timer.cancel( ec );
        if ( ec ) {
            LOG_WARN_MSG( __FUNCTION__ << "(" << __LINE__ << ") could not cancel: " << boost::system::system_error(ec).what() );
        }
    } else {
        LOG_WARN_MSG(
                "unsupported message type " <<
                runjob::Message::toString( msg->getType() ) <<
                " for status " << _status
                );

        return;
    }
}

void
Runjob::handleInserting(
        const runjob::Message::Ptr& msg
        )
{
    if ( msg->getType() != runjob::Message::Result ) {
        LOG_WARN_MSG(
                "unsupported message type " <<
                runjob::Message::toString( msg->getType() ) <<
                " for status " << _status
                );

        return;
    }

    const message::Result::ConstPtr result = runjob::Message::get<message::Result>( msg );
    const error_code::rc error = result->getError();

    if ( error ) {
        _status.set( Status::Error );
        _plugin->startError(
                _credentials->getPid(),
                error,
                result->getMessage()
                );

        _connection->write( msg, _status );

        return;
    }

    // remember job ID
    _jobId = msg->getJobId();
    LOG_INFO_MSG( "job " << _jobId << " inserted" );
    _connection->write( msg, _status );
}

void
Runjob::handleStarting(
        const runjob::Message::Ptr& msg
        )
{
    if ( msg->getType() != runjob::Message::Result ) {
        LOG_WARN_MSG(
                "unsupported message type " <<
                runjob::Message::toString( msg->getType() ) <<
                " for status " << _status
                );

        return;
    }

    const message::Result::ConstPtr result = runjob::Message::get<message::Result>( msg );
    const error_code::rc error = result->getError();

    if ( error ) {
        _status.set( Status::Error );
        _plugin->startError(
                _credentials->getPid(),
                error,
                result->getMessage()
                );

        _connection->write( msg, _status );

        boost::system::error_code ec;
        _timer.cancel( ec );
        if ( ec ) {
            LOG_WARN_MSG( __FUNCTION__ << "(" << __LINE__ << ") could not cancel: " << boost::system::system_error(ec).what() );
        }

        return;
    }

    // update status
    _status.set( Status::Running );

    LOG_INFO_MSG( "job " << _jobId << " started" );

    // notify scheduler that job started if there is no error
    _plugin->started(
            _credentials->getPid(),
            _jobId
            );

    // update job ID of init timer
    _timers.update( _jobId );

    _connection->write( msg, _status );
}

void
Runjob::handleRunning(
        const runjob::Message::Ptr& msg
        )
{
    if (
            msg->getType() == runjob::Message::StdIn ||
            msg->getType() == runjob::Message::StdOut ||
            msg->getType() == runjob::Message::StdError ||
            msg->getType() == runjob::Message::Proctable ||
            msg->getType() == runjob::Message::Result
       )
    {
        _connection->write( msg, _status );
        return;
    }

    if ( msg->getType() == runjob::Message::ExitJob ) {
        const message::ExitJob::ConstPtr exit_job_msg = runjob::Message::get<message::ExitJob>( msg );
        _status.set( Status::Terminated );

        _plugin->terminated(
                _credentials->getPid(),
                _jobId,
                exit_job_msg
                );

        _connection->write( msg, _status );

        boost::system::error_code ec;
        _timer.cancel( ec );
        if ( ec ) {
            LOG_WARN_MSG( __FUNCTION__ << "(" << __LINE__ << ") could not cancel: " << boost::system::system_error(ec).what() );
        }

        return;
    }

    LOG_WARN_MSG(
            "unsupported message type " <<
            runjob::Message::toString( msg->getType() ) <<
            " for status " << _status
            );
}

void
Runjob::serverCallback(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOGGING_DECLARE_USER_MDC( _credentials->getUid()->getUser() );
    LOG_TRACE_MSG( "server callback" );

    if ( error ) {
        LOG_WARN_MSG( "runjob_server is unavailable" );

        const message::Result::Ptr result( new message::Result );
        result->setError( error_code::runjob_server_unavailable );

        // include server hostname in message
        const Multiplexer::Ptr mux( _mux.lock() );
        if ( mux ) {
            const bgq::utility::Properties::ConstPtr properties = mux->getOptions().getProperties();
            try {
                result->setMessage(
                        properties->getValue(
                           runjob::mux::PropertiesSection,
                           bgq::utility::ClientPortConfiguration::PropertyName
                           )
                        );
            } catch ( const std::exception& e ) {
                LOG_WARN_MSG( e.what() );
            }
        }

        this->handleImpl( result );
        return;
    }

    // get next header
    _connection->start(
            _strand.wrap(
                boost::bind(
                    &Runjob::handleRequest,
                    shared_from_this(),
                    _1,
                    _2
                    )
                )
            );
}

void
Runjob::status(
        const runjob::commands::response::MuxStatus::Ptr& response,
        const StatusCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Runjob::statusImpl,
                shared_from_this(),
                response,
                callback
                )
            );
}

void
Runjob::statusImpl(
        const runjob::commands::response::MuxStatus::Ptr& response,
        const StatusCallback& callback
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOGGING_DECLARE_USER_MDC( _credentials->getUid()->getUser() );
    LOG_TRACE_MSG( "adding status" );

    runjob::commands::response::MuxStatus::Client status;
    status._pid = _credentials->getPid();
    status._user = _credentials->getUid()->getUser();
    status._job = _jobId;
    status._id = _id;
    _connection->status( status, response, callback );
}

void
Runjob::timeout(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_TRACE_MSG( __FUNCTION__ << " " << boost::system::system_error(error).what() );
    if ( error == boost::asio::error::operation_aborted ) return;

    if ( _status == Status::Initializing ) {
        LOG_WARN_MSG( "timed out waiting for client request" );
        _connection->stop();
        return;
    }
}

void
Runjob::clientDisconnected(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_TRACE_MSG(
            __FUNCTION__ << " " <<
            boost::system::system_error(error).what() << " " <<
            Status::toString( _status.get() )
            );
    if ( error == boost::asio::error::operation_aborted ) return;
    if ( _status == Status::Terminated || _status == Status::Error ) return;

    _timer.expires_from_now(
            boost::posix_time::seconds(60)
            );
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &client::Runjob::clientDisconnected,
                    shared_from_this(),
                    _1
                    )
                )
            );
}

} // client
} // mux
} // runjob
