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

#include "MMCSCommand_status.h"
#include "DBConsoleController.h"
#include "MMCSProperties.h"

#include <db/include/api/tableapi/DBConnectionPool.h>

LOG_DECLARE_FILE( "mmcs" );

MMCSCommand_status::MMCSCommand_status(
        const char* name,
        const char* description,
        const MMCSCommandAttributes& attributes
        ) :
    MMCSCommand(name, description, attributes)
{
    // nothing to do
}

MMCSCommand_status*
MMCSCommand_status::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock( false );
    commandAttributes.requiresConnection( false );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    commandAttributes.mmcsServerCommand( true );
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    return new MMCSCommand_status("status", "status", commandAttributes);
}

void
MMCSCommand_status::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    reply <<
        OK << "BG/Q " << MMCSProperties::getProperty(MMCS_PROCESS) <<
        " " << MMCSProperties::getProperty(MMCS_VERSION) << 
        std::endl;

    reply << "running in pid " << getpid() << std::endl;
    reply << "configured from " << MMCSProperties::getProperties()->getFilename() << std::endl;

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
        holder.Lock( &DBConsoleController::_midplaneControllerListMutex );
        const size_t users = DBConsoleController::_midplaneControllerList.size();
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
    reply << DONE;
}

void
MMCSCommand_status::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description();
    reply << ";displays internal status of mmcs_server.";

    reply << DONE;
}
