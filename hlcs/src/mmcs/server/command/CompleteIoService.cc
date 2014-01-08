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

#include "CompleteIoService.h"

#include "RebootNodes.h"

#include "../BlockControllerTarget.h"
#include "../BlockHelper.h"
#include "../DBConsoleController.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/dataconv.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/TxObject.h>
#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <utility/include/Log.h>

#include <boost/scoped_ptr.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

CompleteIoService*
CompleteIoService::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false) ;      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(common::SPECIAL);
    commandAttributes.bgadminAuth(true);
    return new CompleteIoService("complete_io_service", "complete_io_service <location>", commandAttributes);
}

BGQDB::STATUS
CompleteIoService::impl(
        const std::string& location,
        std::string& containingBlock
        )
{
    SQLRETURN sqlrc;

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return BGQDB::CONNECTION_ERROR;
    }

    // must be an IO drawer location of length 6 (i.e.  R00-ID, Q60-I0)
    if (location.length() != 6) {
        return BGQDB::INVALID_ID;
    }

    // If there is a booted block that encompasses this drawer, return the name of the block so that
    // MMCS can reboot the nodes
    std::string block;
    block.append(" ( select a.blockid from bgqblock a, bgqioblockmap b where numionodes > 8 and ");
    block.append("     a.blockid = b.blockid and substr(location,1,6)  = '");
    block.append(location.substr(0,6));
    block.append("'  )");

    BGQDB::DBTBlock dbo;
    BGQDB::ColumnsBitmap colBitmap;
    colBitmap.set(dbo.BLOCKID);
    dbo._columns = colBitmap.to_ulong();

    std::string sqlstr("where status in ('I', 'A', 'B', 'C') and numionodes > 0 and  blockid in ");
    sqlstr.append(block);
    sqlrc = tx.query(&dbo, sqlstr.c_str());
    if (sqlrc == SQL_SUCCESS) {
        sqlrc = tx.fetch(&dbo);

        if (sqlrc == SQL_SUCCESS) {
            BGQDB::trim_right_spaces((char *)dbo._blockid);
            containingBlock = std::string(dbo._blockid);
        } else {
            tx.close(&dbo);
            // mark the Nodes in the drawer in A, if there is no block that needs to be rebooted

            // Note: there are other locations associated with the service action, such as the IO drawer itself, Icons, etc. and
            // the status of that hardware will be handled by the service action code.

            sqlstr = "update bgqionode set status = 'A' where iopos = '" + location + std::string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                return BGQDB::DB_ERROR;
            }
        }
    }

    return BGQDB::OK;
}

void
CompleteIoService::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::string containingBlock;
    const BGQDB::STATUS result = this->impl(args[0], containingBlock);
    if (result != BGQDB::OK) {
        reply << mmcs_client::FAIL << "database error: " << result << mmcs_client::DONE;
        return;
    }

    if ( !containingBlock.empty() ) {
        // reboot the IO drawer's nodes within the block
        LOG_INFO_MSG( "rebooting " << args[0] << " nodes in block " << containingBlock );
        std::deque<std::string> selectArgs;
        selectArgs.push_back(containingBlock);
        pController->selectBlock(selectArgs, reply, true);
        if ( !pController->getBlockHelper() )
            return;
        if ( !pController->getBlockHelper()->getBase() )
            return;

        const std::string cmd( "{" + args[0] + std::string("-J..$}") );
        BlockControllerTarget target(pController->getBlockHelper()->getBase(), cmd, reply);
        std::deque<std::string> args;
        boost::scoped_ptr<RebootNodes> reboot_nodes_cmd_ptr(RebootNodes::build());
        reboot_nodes_cmd_ptr->execute(args, reply, pController, &target);
    } else {
        reply << mmcs_client::OK << mmcs_client::DONE;
    }
}

void
CompleteIoService::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
  reply << mmcs_client::OK << description()
        << ";Internal command, not to be used from console, only to be used internally during service actions."
        << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
