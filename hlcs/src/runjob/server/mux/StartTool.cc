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
#include "server/mux/StartTool.h"

#include "server/job/Container.h"
#include "server/job/Debug.h"
#include "server/job/Signal.h"
#include "server/mux/Connection.h"

#include "common/message/Result.h"
#include "common/logging.h"
#include "common/Message.h"

#include "server/Job.h"
#include "server/Security.h"
#include "server/Server.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/job/types.h>

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

void
StartTool::create(
        const Server::Ptr& server, 
        const runjob::Message::Ptr& message,
        const Connection::Ptr& mux
        )
{
    const Ptr result(
            new StartTool( server, message, mux )
            );

    if ( !message->getJobId() ) {
        const BGQDB::job::Id job = result->lookupJob();
        if ( !job ) return;
        message->setJobId( job );
    }

    server->getJobs()->find(
            message->getJobId(),
            boost::bind(
                &StartTool::findJobHandler,
                result,
                _1
                )
            );
}

StartTool::~StartTool()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( !_response ) return;
    if ( !_mux ) return;

    if ( _response->getError() ) {
        LOG_WARN_MSG( _response->getMessage() );
        _mux->write( _response );
    }
}

StartTool::StartTool(
        const Server::Ptr& server, 
        const runjob::Message::Ptr& message,
        const Connection::Ptr& mux
        ) :
    _server( server ),
    _request( boost::static_pointer_cast<message::StartTool>(message) ),
    _response( new message::Result() ),
    _mux( mux )
{
    _response->setClientId( _request->getClientId() );
}

BGQDB::job::Id
StartTool::lookupJob() const
{
    LOGGING_DECLARE_LOCATION_MDC( _mux->hostname() );
    LOG_INFO_MSG( "runjob pid " << _request->_pid );

    try {
        const cxxdb::ConnectionPtr connection(
                BGQDB::DBConnectionPool::Instance().getConnection()
                );
        if ( !connection ) {
            _response->setMessage( "could not get database connection to lookup job for pid " + boost::lexical_cast<std::string>(_request->_pid) );
            _response->setError( error_code::database_error );
            return 0;
        }

        // the job ID can be queried using the pid and hostname
        const cxxdb::QueryStatementPtr statement = connection->prepareQuery(
                std::string("SELECT ") +
                BGQDB::DBTJob::ID_COL + " " +
                "FROM " + BGQDB::DBTJob().getTableName() + " " +
                "WHERE " + BGQDB::DBTJob::PID_COL + "=? AND " +
                BGQDB::DBTJob::HOSTNAME_COL +  "=?",
                { BGQDB::DBTJob::PID_COL, BGQDB::DBTJob::HOSTNAME_COL }
                );

        statement->parameters()[ BGQDB::DBTJob::PID_COL ].set( _request->_pid );
        statement->parameters()[ BGQDB::DBTJob::HOSTNAME_COL ].set( _mux->hostname() );

        const cxxdb::ResultSetPtr results = statement->execute();
        if ( !results->fetch() ) {
            _response->setMessage( "could not find job associated with pid " + boost::lexical_cast<std::string>(_request->_pid) );
            _response->setError( error_code::job_not_found);
            return 0;
        }

        const cxxdb::Columns& columns = results->columns();
        return columns[ BGQDB::DBTJob::ID_COL ].getInt64();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        _response->setMessage(
                "could not find job associated with pid " + 
                boost::lexical_cast<std::string>(_request->_pid) + " " +
                e.what()
                );
        _response->setError( error_code::database_error);
        return 0;
    }
}

void
StartTool::findJobHandler(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _request->getClientId() );
    LOGGING_DECLARE_JOB_MDC( _request->getJobId() );
    if ( !job ) {
        _response->setMessage( "could not find job " + boost::lexical_cast<std::string>(_request->getJobId()) );
        _response->setError( error_code::job_not_found );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    LOG_DEBUG_MSG( "validating job " << _request->getJobId() );

    const bool validate = server->getSecurity()->validate(
            _request->_uid,
            hlcs::security::Action::Execute,
            _request->getJobId()
            );
    if ( !validate ) {
        _response->setMessage( "execute authority on job " + boost::lexical_cast<std::string>(_request->getJobId()) + " denied");
        _response->setError( error_code::permission_denied );
        return;
    }
    try {
        job::Debug::create( job, _request, _mux );
    } catch ( const std::exception & e ) {
        _response->setMessage( "mapping file is invalid for job " + boost::lexical_cast<std::string>(_request->getJobId()));
        _response->setError( error_code::mapping_file_invalid );

        LOG_INFO_MSG( "Start tool request failed, terminating job." );
        job::Signal::create( job, SIGKILL );

        return;
    }
}

} // mux
} // server
} // runjob

