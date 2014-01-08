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

#include "RebootNodes.h"

#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"
#include "../DBConsoleController.h"
#include "../DBBlockController.h"
#include "../DBStatics.h"
#include "../BlockControllerTarget.h"
#include "../IOBlockController.h"

#include <control/include/mcServer/MCServerRef.h>

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

namespace mmcs {
namespace server {
namespace command {

void
reboot_nodes(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if (!pTarget && !pController) {
        reply << mmcs_client::FAIL << "Reboot request failed. Must supply a blockid or have a block selected" << mmcs_client::DONE;
        return;
    }

    const BlockPtr pBlock = pTarget->getBlockController();

    if (!pBlock->isIoBlock()) {
        reply << mmcs_client::FAIL << "reboot_nodes does not support compute blocks." << mmcs_client::DONE;
        return;
    }

    if (pBlock->getNodes().empty() || pTarget->getNodes().empty()) {
        reply << mmcs_client::FAIL << "List of targets for reboot_nodes is empty." << mmcs_client::DONE;
        return;
    }

    const string blockName = pBlock->getBlockName();
    if (!DBConsoleController::setAllocating(blockName)) {
        reply << mmcs_client::FAIL << "Block is being allocated in another thread" << mmcs_client::DONE;
        return;
    }

    BOOST_SCOPE_EXIT( (&blockName) ) {
        DBConsoleController::doneAllocating(blockName);
    } BOOST_SCOPE_EXIT_END;

    // - block must be initialized or allocated with no connected compute blocks
    // get the current block status

    // check for block in proper status
    BGQDB::BlockInfo bInfo;
    BGQDB::STATUS result;
    if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK) {
        reply << mmcs_client::FAIL << "getBlockInfo(" << blockName << ") failed with error " << result << mmcs_client::DONE;
        return;
    }

    if ((strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) != 0)) {
        reply << mmcs_client::FAIL << "Nodes cannot be rebooted unless block has INITIALIZED status with no jobs running." << mmcs_client::DONE;
        return;
    }

    // See if there are any attached compute blocks
    vector<std::string> locations, connected_blocks;
    BOOST_FOREACH(const BCNodeInfo* node, pBlock->getNodes()) {
        // If the location of the node is in the target set,
        // add it to the locations.
        BOOST_FOREACH(const BCNodeInfo* tnode, pTarget->getNodes()) {
            if (node->location() == tnode->location())
                locations.push_back(node->location());
        }
    }

    result = BGQDB::checkIONodeConnection(locations, connected_blocks);
    if (result != BGQDB::OK) {
        reply << mmcs_client::FAIL << "Could not get compute nodes connected to " << blockName << mmcs_client::DONE;
        return;
    }

    if (!connected_blocks.empty()) {
        // Cannot reboot anything as long as something is connected.
        std::ostringstream blocklist;
        bool first = true;
        BOOST_FOREACH(std::string block, connected_blocks) {
            if (first == true) {
                first = false;
                blocklist << block;
            } else {
                blocklist << ", " << block;
            }
        }
        reply << mmcs_client::FAIL << "Cannot reboot " << blockName
              << " with compute blocks " << blocklist.str()
              << " connected." << mmcs_client::DONE;
        return;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to connect to database");
        reply << mmcs_client::FAIL << "Unable to connect to database." << mmcs_client::DONE;
    }

    std::ostringstream sqlstrm;
    sqlstrm << "select location from BGQIONODE where status='E' and ( ";

    bool first = true;
    BOOST_FOREACH(const std::string& node, locations) {
        if (first) {
            sqlstrm << "LOCATION=\'" << node;
            first = false;
        } else
            sqlstrm << "\' OR LOCATION=\'" << node;
    }

    sqlstrm << "\'  )";
    SQLHANDLE hstmt;
    SQLLEN index[1];
    tx.execQuery( sqlstrm.str().c_str(), &hstmt);
    char location[11];
    SQLBindCol(hstmt, 1, SQL_C_CHAR, &location, 11, &index[0]);
    SQLRETURN sqlresult = SQLFetch(hstmt);
    if (sqlresult == SQL_SUCCESS) {
        std::ostringstream errstrm;
        errstrm << "One or more nodes being rebooted is in 'E' status.";
        if (std::find(args.begin(), args.end(), "no_check") == args.end()) {
            // If we're not no_check, then we can't have any 'E' nodes.
            // Just return.
            LOG_ERROR_MSG(errstrm.str());
            reply << mmcs_client::FAIL << errstrm.str() << mmcs_client::DONE;
            return;
        }
        while (sqlresult == SQL_SUCCESS) {
            std::string locstr(location);
            locations.erase(std::find(locations.begin(), locations.end(), locstr));
            sqlresult =  SQLFetch(hstmt);
        }
    }

    SQLCloseCursor(hstmt);

    if (locations.empty()) {
        reply << mmcs_client::FAIL << "No nodes to reboot. Selected nodes may be in error." << mmcs_client::DONE;
        return;
    }

    if ( !DBStatics::setLocationStatus(locations, DBStatics::SOFTWARE_FAILURE, bgq::util::Location::ComputeCardOnIoBoard) ) {
        reply << mmcs_client::FAIL << "Could not update node status to Software Failure" << mmcs_client::DONE;
        return;
    }

    // Connect the block if we need to.
    if ((pBlock)->isConnected() == false) {
        pBlock->connect(args, reply, pTarget);
    } else {
        std::deque<std::string> args;
        args.push_back("no_shutdown");
        // Can't redirect shutdown. It's a mess with target sets
        // to do it.
        pBlock->resetRedirection();
        pBlock->disconnect(args, reply);
        pBlock->connect(args, reply, pTarget);
    }

    string db_uloader;
    if (strlen(bInfo.uloaderImg) > 0)
        db_uloader.assign("uloader=").append(bInfo.uloaderImg);

    deque<string> boot_args; // options to pass to BlockController::boot_block
    boot_args.push_back(db_uloader);

    // If it isn't 'shared', make it so.
    const bool shared = pBlock->getShared();
    if (!shared) {
        pBlock->setShared(true);
    }

    // Shutdown
    pBlock->shutdown_block(reply, args); // ignore reply, don't care if shutdown fails

    // Specified IO nodes get the normal boot request.
    boost::dynamic_pointer_cast<IOBlockController>(pBlock)->reboot_nodes(pTarget, boot_args, reply);
    if ( reply.getStatus() ) {
        return;
    }

    // Reset the 'shared' flag.
    pBlock->setShared(shared);

    // Now start the boot complete thread and do a waitboot.
    const DBBlockPtr pDBBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pDBBlock->startBootCheck(pTarget);
    const std::deque<std::string> cmdargs;
    pDBBlock->waitBoot(cmdargs, reply, false, true); // Wait for the reboot to complete.
}

RebootNodes*
RebootNodes::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(true);           // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new RebootNodes("reboot_nodes", "[<target>] reboot_nodes [no_check]", commandAttributes);
}

void
RebootNodes::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if ( !args.empty() && args[0] != "no_check" ) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    reboot_nodes(args, reply, pController, pTarget);
}

void
RebootNodes::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Reboot specific I/O nodes or the entire block."
          << ";By default, all nodes get rebooted."
          << ";The 'no_check' option allows the nodes to be rebooted"
          << ";even if one or more of them are in ERROR state."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
