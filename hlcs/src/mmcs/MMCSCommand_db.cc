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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file MMCSCommand_db.cc
 */

#include <boost/scoped_ptr.hpp>
#include <db/include/api/job/types.h>
#include <db/include/api/BGQDBlib.h>
#include "DBBlockController.h"
#include "BlockControllerTarget.h"
#include "ConsoleController.h"
#include "DBConsoleController.h"
#include "MMCSCommandProcessor.h"
#include "MMCSCommandReply.h"
#include "MMCSCommand_db.h"
#include "MMCSCommand_reboot.h"

#include <control/include/mcServer/MCServerAPIHelpers.h>
#include <control/include/mcServer/MCServerRef.h>

#include "RunJobConnection.h"
#include <hlcs/include/runjob/commands/KillJob.h>

using namespace MCServerMessageSpec;

MMCSCommand_kill_midplane_jobs*
MMCSCommand_kill_midplane_jobs::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false) ;      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(SPECIAL);
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_kill_midplane_jobs("kill_midplane_jobs", "kill_midplane_jobs <location> [kill|list]", commandAttributes);
}


void
MMCSCommand_kill_midplane_jobs::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    BGQDB::STATUS result;

    vector<BGQDB::job::Id> jobs;

    bool listOnly = false;

    if (args.size() > 1)   {
        string subcmd( args[1] );
        if (subcmd == string("kill"))  {
            // Nothing to do.
        }
        else if (subcmd == string("list")) {
            listOnly = true;
        }
        else {
            // Invalid argument
            reply << FAIL << "use kill or list" << DONE;
            return;
        }
    }

    if (pController->getBlockHelper() != 0)
        pController->deselectBlock();  // If we have a selected block, deselect it.

    // first call to get list (3rd argument is "listOnly")
    result = BGQDB::killMidplaneJobs(args[0].c_str(), &jobs, true);
    if (result != BGQDB::OK)
        {
            reply << FAIL << "error on BGQDB::KillMidplaneJobs" << DONE;
            return;
        }


    if (listOnly == true)
    {
        reply << OK;
        // list was requested, not kill, so just return the list of job IDs to the caller
        for (vector<BGQDB::job::Id>::const_iterator i(jobs.begin()) ; i != jobs.end() ; ++i )
        {
            if ( i != jobs.begin() )
            {
                reply << ";";
            }
            reply << *i;
        }
    } else {
        int signal = 9;

        // kill the jobs
        for (vector<BGQDB::job::Id>::const_iterator i(jobs.begin()) ; i != jobs.end() ; ++i )
            {
                if(RunJobConnection::Kill(*i, signal) != 0) {
                    reply << FAIL << "error contacting runjob server to kill jobs" << DONE;
                    return;
                  }

            }

        // now call to free the blocks
        result = BGQDB::killMidplaneJobs(args[0].c_str(), &jobs, false);
        if (result != BGQDB::OK)
            {
                reply << FAIL << "error on BGQDB::KillMidplaneJobs trying to free blocks" << DONE;
                return;
            }

        reply << OK;
    }

    reply << DONE;
}

void
MMCSCommand_kill_midplane_jobs::help(deque<string> args,
              MMCSCommandReply& reply)
{
  reply << OK << description()
    << ";Internal command, not to be used from console, only to be used internally during service actions. "
    << DONE;
}


MMCSCommand_complete_io_service*
MMCSCommand_complete_io_service::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false) ;      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(SPECIAL);
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_complete_io_service("complete_io_service", "complete_io_service <location>", commandAttributes);
}


void
MMCSCommand_complete_io_service::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    BGQDB::STATUS result;

    std::string containingBlock = "<none>";
    result = BGQDB::completeIOService(args[0].c_str(), containingBlock);
    if (result != BGQDB::OK)
    {
        reply << FAIL << "error on BGQDB::completeIOService" << DONE;
        return;
    }

    if (containingBlock != "<none>") {
        // reboot the IO drawer's nodes within the block

        // first get the block controller for the containingBlock
        deque<string> selectArgs;
        selectArgs.push_back(containingBlock);
        pController->selectBlock(selectArgs, reply, true);
        if ( !pController->getBlockHelper() ) return;
        if ( !pController->getBlockBaseController() ) return;

        string cmd;
        cmd = "{" + args[0] + string("-J..$}");
        BlockControllerTarget target(pController->getBlockBaseController(), cmd, reply);
        deque<std::string> args;
        MMCSCommand_reboot_nodes* rn = MMCSCommand_reboot_nodes::build();
        rn->execute(args, reply, pController, &target);
    }
    else
        reply << OK << DONE;
}

void
MMCSCommand_complete_io_service::help(deque<string> args,
              MMCSCommandReply& reply)
{
  reply << OK << description()
    << ";Internal command, not to be used from console, only to be used internally during service actions. "
    << DONE;
}


MMCSCommand_delete*
MMCSCommand_delete::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(ADMIN);
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_delete("delete", "delete <db2table> <id>", commandAttributes);
}

void
MMCSCommand_delete::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    if ((args[0] == "bgqblock")) {
        BGQDB::STATUS result = BGQDB::deleteBlock(args[1]);
        switch (result) {
        case BGQDB::OK:
            reply << OK << DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << FAIL << "invalid block id " << args[1] << DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << FAIL << "unable to connect to database" << DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << FAIL << "database failure" << DONE;
            break;
        case BGQDB::NOT_FOUND:
            reply << FAIL << "block " << args[1] << " not found" << DONE;
            break;
        case BGQDB::FAILED:
            reply << FAIL << "invalid block state" << DONE;
            break;
        default:
            reply << FAIL << "unexpected return code from BGQDB::deleteBlock : " << result << DONE;
            break;
        }
        return;
    } else
          if ((args[0] == "bgqnodeconfig")) {
              string sqlstmt = " delete from bgqnodeconfig where nodeconfig = '";
              sqlstmt += args[1] + "' ";

              BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
              if (!tx.getConnection()) {
                  reply << FAIL << "Unable to obtain database connection" << DONE;
                  return;
              }
              SQLRETURN rc = tx.execStmt(sqlstmt.c_str());
              if (rc == 0)
                  reply << OK << DONE;
              else {
                  reply << FAIL << "database failure, node config not found or in use by a block" << DONE;
              }
              return;
          }  else {
              reply << FAIL << "invalid table" << DONE;
              return;
          }
}

void
MMCSCommand_delete::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";Delete record <id> from <db2table>"
      << ";db2table is one of the following:"
      << ";   bgqblock "
      << ";   bgqnodeconfig "
      << DONE;
}

MMCSCommand_sql*
MMCSCommand_sql::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(ADMIN);
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_sql("sql", "sql <sql-string>", commandAttributes);
}

void
MMCSCommand_sql::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    string sqlstmt;
    for (unsigned i = 0; i < args.size(); i++)
    {
        sqlstmt += args[i] + " ";
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        reply << FAIL << "Unable to obtain database connection" << DONE;
        return;
    }

    SQLRETURN rc = tx.execStmt(sqlstmt.c_str());
    if (rc == SQL_SUCCESS)
        reply << OK << DONE;
    else
        reply << FAIL << "error code=" << rc << DONE;
    return;
}

void
MMCSCommand_sql::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    reply << OK << description()
      << ";execute <sql-string>"
      << ";This command cannot be used to run any sql statement that returns output (i.e., queries)."
      << DONE;
}
