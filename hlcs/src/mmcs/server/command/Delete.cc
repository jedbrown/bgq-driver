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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "Delete.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/TxObject.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

Delete*
Delete::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    commandAttributes.bgadminAuth(true);
    return new Delete("delete", "delete <db2table> <id>", commandAttributes);
}

void
Delete::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if ((args[0] == "bgqblock")) {
        BGQDB::STATUS result = BGQDB::deleteBlock(args[1]);
        switch (result) {
            case BGQDB::OK:
                reply << mmcs_client::OK << mmcs_client::DONE;
                break;
            case BGQDB::INVALID_ID:
                reply << mmcs_client::FAIL << "Invalid block id " << args[1] << mmcs_client::DONE;
                break;
            case BGQDB::CONNECTION_ERROR:
                reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
                break;
            case BGQDB::DB_ERROR:
                reply << mmcs_client::FAIL << "Database failure" << mmcs_client::DONE;
                break;
            case BGQDB::NOT_FOUND:
                reply << mmcs_client::FAIL << "Block " << args[1] << " not found" << mmcs_client::DONE;
                break;
            case BGQDB::FAILED:
                reply << mmcs_client::FAIL << "Invalid block status" << mmcs_client::DONE;
                break;
            default:
                reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::deleteBlock : " << result << mmcs_client::DONE;
                break;
        }
        return;
    } else if ((args[0] == "bgqnodeconfig")) {
        string sqlstmt = " delete from bgqnodeconfig where nodeconfig = '";
        sqlstmt += args[1] + "' ";

        BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
        if (!tx.getConnection()) {
            reply << mmcs_client::FAIL << "Unable to obtain database connection" << mmcs_client::DONE;
            return;
        }
        SQLRETURN rc = tx.execStmt(sqlstmt.c_str());
        if (rc == 0)
            reply << mmcs_client::OK << mmcs_client::DONE;
        else {
            reply << mmcs_client::FAIL << "Database failure, node config not found or in use by a block" << mmcs_client::DONE;
        }
        return;
    }  else {
        reply << mmcs_client::FAIL << "Invalid table" << mmcs_client::DONE;
        return;
    }
}

void
Delete::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Delete record <id> from <db2table>"
          << ";db2table is one of the following:"
          << ";   bgqblock "
          << ";   bgqnodeconfig "
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
