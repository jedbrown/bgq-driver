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

#include "Status.h"

#include "../DBConsoleController.h"

#include "common/Properties.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

using mmcs::common::Properties;

namespace mmcs {
namespace server {
namespace command {

Status::Status(
        const char* name,
        const char* description,
        const Attributes& attributes
        ) :
    AbstractCommand(name, description, attributes)
{
    // nothing to do
}

Status*
Status::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock( false );
    commandAttributes.requiresConnection( false );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    commandAttributes.mmcsServerCommand( true );
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    return new Status("status", "status", commandAttributes);
}

void
Status::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    reply <<
        mmcs_client::OK << "BG/Q " << Properties::getProperty(MMCS_PROCESS) <<
        " " << Properties::getProperty(MMCS_VERSION) <<
        std::endl;

    reply << "running in pid " << getpid() << std::endl;
    reply << "configured from " << Properties::getProperties()->getFilename() << std::endl;

    reply << std::endl;
    {
        PthreadMutexHolder holder;
        holder.Lock( &DBConsoleController::getBlockMapMutex() );
        const size_t blocks = DBConsoleController::getBlockMap().size();
        reply << blocks << " block" << (blocks == 1 ? "" : "s") << std::endl;
    }

    reply << std::endl;
    {
        PthreadMutexHolder holder;
        holder.Lock( &DBConsoleController::_consoleControllerListMutex );
        const size_t users = DBConsoleController::_consoleControllerList.size();
        reply << users << " user" << (users == 1 ? "" : "s") << std::endl;
    }

    reply << std::endl;
    const BGQDB::DBConnectionPool& db = BGQDB::DBConnectionPool::instance();
    const unsigned available = db.availableCount();
    reply << available << " database connection" << (available == 1 ? "" : "s") << " available" << std::endl;
    const unsigned used = db.usedCount();
    reply << used << " database connection" << (used == 1 ? "" : "s") << " used" << std::endl;
    const unsigned size = db.size();
    reply << size << " configured pool size" << std::endl;
    const unsigned max = db.maxCount();
    reply << max << " maximum connection" << (max == 1 ? "" : "s") << std::endl;
    reply << mmcs_client::DONE;
}

void
Status::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description();
    reply << ";Displays internal status of mmcs_server.";

    reply << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
