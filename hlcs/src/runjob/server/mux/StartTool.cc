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

    const BGQDB::job::Id job = message->getJobId() ? message->getJobId() : result->lookupJob();
    if ( !job ) return;

    server->getJobs()->find(
            job,
            boost::bind(
                &StartTool::findJobHandler,
                result,
                _1
                )
            );

}

StartTool::StartTool(
        const Server::Ptr& server, 
        const runjob::Message::Ptr& message,
        const Connection::Ptr& mux
        ) :
    _server( server ),
    _message( boost::static_pointer_cast<message::StartTool>(message) ),
    _mux( mux )
{

}

BGQDB::job::Id
StartTool::lookupJob() const
{
    LOGGING_DECLARE_LOCATION_MDC( _mux->hostname() );
    LOG_TRACE_MSG( "looking up job associated with runjob pid " << _message->_pid );

    try {
        const cxxdb::ConnectionPtr connection(
                BGQDB::DBConnectionPool::Instance().getConnection()
                );
        if ( !connection ) {
            const message::Result::Ptr msg( new message::Result() );
            msg->setMessage( "could not get database connection to lookup job for pid " + boost::lexical_cast<std::string>(_message->_pid) );
            msg->setError( error_code::database_error );
            msg->setClientId( _message->getClientId() );
            _mux->write( msg );
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

        statement->parameters()[ BGQDB::DBTJob::PID_COL ].set( _message->_pid );
        statement->parameters()[ BGQDB::DBTJob::HOSTNAME_COL ].set( _mux->hostname() );

        const cxxdb::ResultSetPtr results = statement->execute();
        if ( !results->fetch() ) {
            const message::Result::Ptr msg( new message::Result() );
            msg->setMessage( "could not find job associated with pid " + boost::lexical_cast<std::string>(_message->_pid) );
            msg->setError( error_code::job_not_found);
            msg->setClientId( _message->getClientId() );
            _mux->write( msg );
            return 0;
        }

        const cxxdb::Columns& columns = results->columns();
        return columns[ BGQDB::DBTJob::ID_COL ].getInt64();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        const message::Result::Ptr msg( new message::Result() );
        msg->setMessage(
                "could not find job associated with pid " + 
                boost::lexical_cast<std::string>(_message->_pid) + " " +
                e.what()
                );
        msg->setError( error_code::database_error);
        msg->setClientId( _message->getClientId() );
        _mux->write( msg );
        return 0;
    }
}

void
StartTool::findJobHandler(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _message->getClientId() );
    LOGGING_DECLARE_JOB_MDC( _message->getJobId() );
    if ( !job ) {
        const message::Result::Ptr msg( new message::Result() );
        msg->setMessage( "could not find job " + boost::lexical_cast<std::string>(_message->getJobId()) );
        msg->setError( error_code::job_not_found );
        msg->setClientId( _message->getClientId() );
        _mux->write( msg );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const bool validate = server->getSecurity()->validate(
            _message->_uid,
            hlcs::security::Action::Execute,
            _message->getJobId()
            );
    if ( !validate ) {
        const message::Result::Ptr msg( new message::Result() );
        msg->setMessage( "execute authority on job " + boost::lexical_cast<std::string>(_message->getJobId()) + " denied");
        msg->setError( error_code::permission_denied );
        msg->setClientId( _message->getClientId() );
        _mux->write( msg );
        return;
    }

    job::Debug::create( job, _message, _mux );
}

} // mux
} // server
} // runjob

