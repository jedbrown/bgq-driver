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

#include "Allocate.h"

#include "../BlockControllerBase.h"
#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

Allocate*
Allocate::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);

    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    commandAttributes.addAuthPair(blockread);
    return new Allocate("allocate", "allocate <blockId>", commandAttributes);
}


void
Allocate::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> vn;
    vn.push_back(args[0]);
    execute(args, reply, pController, pTarget, &vn);
    return;
}

void
Allocate::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if (validnames->size() != 1) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    const std::string blockName = validnames->at(0);

    BGQDB::BLOCK_STATUS bState;
    const BGQDB::STATUS result = BGQDB::getBlockStatus(blockName, bState);
    if (result == BGQDB::NOT_FOUND) {
        reply << mmcs_client::FAIL << "Block not found: " << blockName << mmcs_client::DONE;
        return;
    } else if ( result != BGQDB::OK ) {
        reply << mmcs_client::FAIL << "Could not get block status: " << DBBlockController::strDBError(result) << mmcs_client::DONE;
        return;
    }

    if ( bState != BGQDB::FREE && bState != BGQDB::ALLOCATED ) {
        reply << mmcs_client::FAIL << "Block is not free" << mmcs_client::DONE;
        return;
    }

    if (!DBConsoleController::setAllocating(blockName)) {
        reply << mmcs_client::FAIL << "Block is being allocated or freed in another thread" << mmcs_client::DONE;
        return;
    }

    // Select the block
    pController->selectBlock(args, reply, false);
    if (reply.getStatus() != 0) {
        if (reply.str() == "args?") {
            reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        }
        DBConsoleController::doneAllocating(blockName);
        return;
    }

    // Allocate the block (supports "no_check" option)
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->allocateBlock(args, reply);
    if (reply.getStatus() != 0) {
        if (reply.str() == "args?") {
            reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        }
        if (pController->getBlockHelper() != 0) {
            pController->deselectBlock();
        }
        DBConsoleController::doneAllocating(blockName);

        return;
    }

    // Remove the arguments for boot_block call
    args.clear();

    // Boot the block
    pBlock->boot_block(args, reply);
    if (reply.getStatus() != 0) {
        const BGQDB::STATUS result = BGQDB::getBlockStatus(blockName, bState);
        if ( result == BGQDB::OK ) {
            if ( bState != BGQDB::TERMINATING ) {
                // Return the block to terminating state
                (void)pBlock->setBlockStatus(BGQDB::TERMINATING);
            }
        } else {
            LOG_ERROR_MSG( "Could not get block status: " << DBBlockController::strDBError(result) );
        }

        if (pController->getBlockHelper() != 0) {
            pBlock->getBase()->setDisconnecting(true, "boot failed");
            pController->deselectBlock();
            DBConsoleController::doneAllocating(blockName);
        }

        return;
    }

    DBConsoleController::doneAllocating(blockName);

    // wait for the boot to complete
    const std::deque<std::string> waitBoot_args; // empty argument list
    pBlock->waitBoot(waitBoot_args, reply);
    if (reply.str() == "args?") {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }
}

void
Allocate::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";For specified <blockId>, performs select_block, allocate_block, boot_block and wait_boot."
          << ";<blockId> identifies a block in the BGQBLOCK table."
          << ";This command can be used to initialize a block in free status."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
