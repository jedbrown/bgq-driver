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


#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>
#include <control/include/mcServer/MCServerRef.h>
#include <db/include/api/BGQDBlib.h>
#include <utility/include/Log.h>
#include "MMCSCommand_reboot.h"
#include "MMCSCommandReply.h"
#include "ConsoleController.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSConsolePort.h"

using namespace std;


LOG_DECLARE_FILE( "mmcs.command" );


void reboot_nodes(std::deque<std::string> args, MMCSCommandReply& reply,
                  DBConsoleController* pController, BlockControllerTarget* pTarget) {
    int result;
    string db_uloader;
    deque<string> boot_args;	// options to pass to BlockController::boot_block

    if(!pTarget && !pController) {  // Need a target or a selected block
        reply << FAIL << "Reboot request failed.  Must supply a blockid or have a block selected" << DONE;
        return;
    }

    const BlockPtr pBlock = pTarget->getBlockController();

    if (pBlock->getNodes().size() == 0 || pTarget->getNodes().size() == 0) {
        reply << FAIL << "list of targets for reboot_nodes is empty." << DONE;
        return;
    }

    string blockName = pBlock->getBlockName();

    if(!DBConsoleController::setAllocating(blockName)) {
        reply << FAIL << "Block is being allocated in another thread" << DONE;
        return;
    }

    BOOST_SCOPE_EXIT( (&blockName) ) {
        DBConsoleController::doneAllocating(blockName);
    } BOOST_SCOPE_EXIT_END;

    if((pBlock)->isIOBlock() == false) {
        reply << FAIL << "reboot_nodes does not support compute blocks." << DONE;
        return;
    }

    // - block must be initialized or allocated with no connected compute blocks
    // get the current block state

    // check for block in proper state
    BGQDB::BlockInfo bInfo;
    if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK)
        {
            reply << FAIL << "getBlockInfo(" << blockName << ") failed with error " << result << DONE;
            return;
        }

    if ((strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) != 0)) {
        reply << FAIL << "Nodes cannot be rebooted unless in INITIALIZED state with no jobs running." << DONE;
        return;
    }

    // See if there are any attached compute blocks
    vector<std::string> locations, connected_blocks;
    BOOST_FOREACH(BCNodeInfo* node, pBlock->getNodes()) {
        // If the location of the node is in the target set,
        // add it to the locations.
        BOOST_FOREACH(BCNodeInfo* tnode, pTarget->getNodes()) {
            if(node->location() == tnode->location())
                locations.push_back(node->location());
        }
    }

    result = BGQDB::checkIONodeConnection(locations, connected_blocks);
    if(result != BGQDB::OK) {
        reply << FAIL << "Could not get compute nodes connected to " << blockName << DONE;
        return;
    }

    if(connected_blocks.size() != 0) {
        // Cannot reboot anything as long as something is connected.
        std::ostringstream blocklist;
        bool first = true;
        BOOST_FOREACH(std::string block, connected_blocks) {
            if(first == true) {
                first = false;
                blocklist << block;
            } else {
                blocklist << ", " << block;
            }
        }
        reply << FAIL << "Cannot reboot " << blockName
              << " with compute blocks " << blocklist.str()
              << " connected." << DONE;
        return;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_INFO_MSG("unable to connect to database");
        reply << FAIL << "Unable to connect to database." << DONE;
    }

    std::ostringstream sqlstrm;
    sqlstrm << "select location from  BGQIONODE where  status='E' and ( ";

    bool first = true;
    BOOST_FOREACH(std::string& node, locations) {
        if(first) {
            sqlstrm << "LOCATION=\'" << node;
            first = false;
        }
        else sqlstrm << "\' OR LOCATION=\'" << node;
    }

    sqlstrm << "\'  )";
    SQLHANDLE hstmt;
    SQLLEN index[1];
    SQLRETURN sqlresult = tx.execQuery( sqlstrm.str().c_str(), &hstmt);
    char location[11];
    SQLBindCol(hstmt, 1, SQL_C_CHAR, &location, 11, &index[0]);
    sqlresult =  SQLFetch(hstmt);
    if (sqlresult == SQL_SUCCESS) {
        std::ostringstream errstrm;
        errstrm << "One or more nodes being rebooted is in 'E' status.";
        if(std::find(args.begin(), args.end(), "no_check") == args.end()) {
            // If we're not no_check, then we can't have any 'E' nodes.
            // Just return.
            LOG_ERROR_MSG(errstrm.str());
            reply << FAIL << errstrm.str() << DONE;
            return;
        }
        while(sqlresult == SQL_SUCCESS) {
            std::string locstr(location);
            locations.erase(std::find(locations.begin(), locations.end(), locstr));
            sqlresult =  SQLFetch(hstmt);
        }
    }

    // the locations list.
    SQLCloseCursor(hstmt);

    if(locations.size() == 0) {
        reply << FAIL << "No nodes to reboot.  Selected nodes may be in error." << DONE;
        return;
    }

    sqlstrm.str("");
    sqlstrm << "update BGQIONODE set status='F' where ";

    first = true;
    BOOST_FOREACH(std::string& node, locations) {
        if(first) {
            sqlstrm << "LOCATION=\'" << node;
            first = false;
        }
        else sqlstrm << "\' OR LOCATION=\'" << node;
    }

    sqlstrm << "\'";

    LOG_DEBUG_MSG("Sending query \"" << sqlstrm.str() << "\" to database.");
    sqlresult = tx.execStmt( sqlstrm.str().c_str());
    if(sqlresult != 0) {
        std::ostringstream errstrm;
        errstrm << "SQL query to set node states to 'F' failed.";
        LOG_ERROR_MSG(errstrm.str());
        reply << FAIL << errstrm.str() << DONE;
        return;
    }

    // Connect the block if we need to.
    if((pBlock)->isConnected() == false) {
        pBlock->connect(args, reply, pTarget);
    } else {
        std::deque<std::string> args;
        args.push_back("no_shutdown");
        // Can't redirect shutdown. It's a mess with target sets
        // to do it.
        pBlock->resetRedirection();
        pBlock->disconnect(args);
        pBlock->connect(args,reply,pTarget);
    } 

    if (strlen(bInfo.uloaderImg) > 0)
        db_uloader.assign("uloader=").append(bInfo.uloaderImg);

    boot_args.push_back(db_uloader);

    // If it isn't 'shared', make it so.
    bool shared = pBlock->getShared();
    if(shared == false) {
        pBlock->setShared(true);
    }

    // Shutdown
    pBlock->shutdown_block(reply, pTarget, false);
    if(reply.getStatus() != MMCSCommandReply::STATUS_OK) {
        return;
    }

    // Specified IO nodes get the normal boot request.
    pBlock->reboot_nodes(pTarget, boot_args, reply);

    // Reset the 'shared' flag.
    pBlock->setShared(shared);

    // Now start the boot complete thread and do a waitboot.
    std::deque<std::string> cmdargs;
    const DBBlockPtr pDBBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pDBBlock->startBootCheck(pTarget);
    pDBBlock->waitBoot(cmdargs,reply, false, true); // Wait for the reboot to complete.
    pDBBlock->getBase()->setRebooted(false);  // Now toggle it back for next try.
}

/*!
//
//  reboot_nodes
//  syntax:
//       reboot_nodes
*/
MMCSCommand_reboot_nodes*
MMCSCommand_reboot_nodes::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(true);           // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.internalCommand(true);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_reboot_nodes("reboot_nodes", "[<target>] reboot_nodes [no_check]", commandAttributes);
}

void
MMCSCommand_reboot_nodes::execute(std::deque<std::string> args,
				MMCSCommandReply& reply,
				DBConsoleController* pController,
                                  BlockControllerTarget* pTarget)
{
  reboot_nodes(args, reply, pController, pTarget);
}


void
MMCSCommand_reboot_nodes::help(deque<string> args,
			     MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Reboot specific I/O nodes or the entire block."
          << ";By default, all nodes get rebooted."
          << ";The 'no_check' option allows the nodes to be rebooted"
          << ";even if one or more of them are in ERROR state."
	  << DONE;
}
