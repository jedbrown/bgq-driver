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

#include "Sql.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/TxObject.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

Sql*
Sql::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    commandAttributes.bgadminAuth(true);
    return new Sql("sql", "sql <sql-string>", commandAttributes);
}

void
Sql::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    string sqlstmt;
    for (unsigned i = 0; i < args.size(); i++) {
        sqlstmt += args[i] + " ";
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        reply << mmcs_client::FAIL << "Unable to obtain database connection" << mmcs_client::DONE;
        return;
    }

    SQLRETURN rc = tx.execStmt(sqlstmt.c_str());
    if (rc == SQL_SUCCESS)
        reply << mmcs_client::OK << mmcs_client::DONE;
    else
        reply << mmcs_client::FAIL << "Error code=" << rc << mmcs_client::DONE;
    return;
}

void
Sql::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";execute <sql-string>"
          << ";This command cannot be used to run any SQL statement that returns output (i.e., queries)."
          << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
