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
#include "server/CommandHandler.h"

#include "common/commands/convert.h"
#include "common/commands/create.h"

#include "server/handlers/locate_rank/Impl.h"

#include "server/handlers/ChangeCiosConfig.h"
#include "server/handlers/DumpProctable.h"
#include "server/handlers/EndTool.h"
#include "server/handlers/JobAcl.h"
#include "server/handlers/JobStatus.h"
#include "server/handlers/KillJob.h"
#include "server/handlers/LogLevel.h"
#include "server/handlers/RefreshConfig.h"
#include "server/handlers/Status.h"
#include "server/handlers/ToolStatus.h"

#include "common/logging.h"

#include "server/Ras.h"
#include "server/Server.h"

#include "server/CommandConnection.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/runjob/commands/Message.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

void
CommandHandler::create(
        const Server::Ptr& server,
        const runjob::commands::Request::Ptr& request,
        const boost::shared_ptr<CommandConnection>& connection
        )
{
    LOG_INFO_MSG( "handling " << runjob::commands::Message::toString( request->getTag() ) << " message" );

    // create handler based on type
    Ptr handler;
    switch ( request->getTag() ) {
        case runjob::commands::Message::Tag::ChangeCiosConfig:
            handler.reset( new handlers::ChangeCiosConfig(server) );
            break;
        case runjob::commands::Message::Tag::DumpProctable:
            handler.reset( new handlers::DumpProctable(server) );
            break;
        case runjob::commands::Message::Tag::EndTool:
            handler.reset( new handlers::EndTool(server) );
            break;
        case runjob::commands::Message::Tag::JobAcl:
            handler.reset( new handlers::JobAcl(server) );
            break;
        case runjob::commands::Message::Tag::JobStatus:
            handler.reset( new handlers::JobStatus(server) );
            break;
        case runjob::commands::Message::Tag::KillJob:
            handler.reset( new handlers::KillJob(server) );
            break;
        case runjob::commands::Message::Tag::LocateRank:
            handler.reset( new handlers::locate_rank::Impl(server) );
            break;
        case runjob::commands::Message::Tag::LogLevel:
            handler.reset( new handlers::LogLevel(server) );
            break;
        case runjob::commands::Message::Tag::RefreshConfig:
            handler.reset( new handlers::RefreshConfig(server) );
            break;
        case runjob::commands::Message::Tag::ServerStatus:
            handler.reset( new handlers::Status(server) );
            break;
        case runjob::commands::Message::Tag::ToolStatus:
            handler.reset( new handlers::ToolStatus(server) );
            break;
        default:
            LOG_INFO_MSG( "unsupported message: " << runjob::commands::Message::toString( request->getTag() ) );
            return;
    }

    // validate and handle the request
    if  (
            connection->getUserType() == bgq::utility::portConfig::UserType::Normal &&
            handler->getUserType() == bgq::utility::portConfig::UserType::Administrator
        )
    {
        // kick out non-administrator users
        LOG_WARN_MSG( runjob::commands::Message::toString(request->getTag()) << " permission denied" );
        runjob::commands::Response::Ptr response = runjob::commands::create(
                request->getTag()
                );
        response->setError( runjob::commands::error::permission_denied );
        connection->write(response);

        Ras::create( Ras::AdministrativeAuthorityDenied ).
            detail( "USER", connection->getUser()->getUser() ).
            detail( "COMMAND", std::string("runjob_server command: ") + runjob::commands::Message::toString(request->getTag()) )
            ;

        return;
    }

    try {
        handler->handle( request, connection );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

CommandHandler::CommandHandler(
        const Server::Ptr& server
        ) :
    _options( server->getOptions() ),
    _server( server)
{

}

CommandHandler::~CommandHandler()
{

}

BGQDB::job::Id
CommandHandler::getJob(
        const cxxdb::ConnectionPtr& connection,
        const pid_t pid,
        const std::string& host
        )
{
    LOG_TRACE_MSG( "looking up job associated with runjob pid " << pid << " on host '" << host << "'" );

    try {
        // the job ID can be queried using the pid and hostname
        const cxxdb::QueryStatementPtr statement = connection->prepareQuery(
                std::string("SELECT ") +
                BGQDB::DBTJob::ID_COL + " " +
                "FROM " + BGQDB::DBTJob().getTableName() + " " +
                "WHERE " + BGQDB::DBTJob::PID_COL + "=? AND " +
                BGQDB::DBTJob::HOSTNAME_COL +  "=?",
                { BGQDB::DBTJob::PID_COL, BGQDB::DBTJob::HOSTNAME_COL }
                );

        statement->parameters()[ BGQDB::DBTJob::PID_COL ].set( pid );
        statement->parameters()[ BGQDB::DBTJob::HOSTNAME_COL ].set( host );

        const cxxdb::ResultSetPtr results = statement->execute();
        if ( !results->fetch() ) {
            LOG_INFO_MSG( "could not find job associated with runjob " << pid << " on host " << host );
            return 0;
        }

        const cxxdb::Columns& columns = results->columns();
        return columns[ BGQDB::DBTJob::ID_COL ].getInt64();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        return 0;
    }
}

} // server
} // runjob
