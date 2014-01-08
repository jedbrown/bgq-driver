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

#include "FreeBlock.h"

#include "../BlockControllerBase.h"
#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include <boost/scope_exit.hpp>

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

FreeBlock*
FreeBlock::build()
{
    Attributes commandAttributes;

    commandAttributes.requiresBlock(true);         // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new FreeBlock("free_block", "free_block [abnormal]", commandAttributes);
}

void
FreeBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if (!args.empty()) {
        if (args[0] != "abnormal") {
            reply << mmcs_client::FAIL << "unknown argument " << args[0] << mmcs_client::DONE;
            return;
        }
    }

    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get selected block
    const string blockName = pBlock->getBase()->getBlockName(); // get the block name

    if (!DBConsoleController::setAllocating(blockName)) {
        reply << mmcs_client::FAIL << "Block is being allocated or freed in another thread" << mmcs_client::DONE;
        return;
    }

    BOOST_SCOPE_EXIT( (&blockName) ) {
        DBConsoleController::doneAllocating(blockName);
    } BOOST_SCOPE_EXIT_END;

    pBlock->freeBlock(args, reply);
    if (reply.getStatus()) {
        pController->deselectBlock();
        return;
    }

    pController->deselectBlock();

    // wait for the block to complete Termination
    pBlock->waitFree(reply);
}

void
FreeBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Release a selected block. An alternate form of free that operates on the currently selected block."
          << ";options:"
          << ";  abnormal - Skip normal kernel shutdown. Also skips I/O link shutdown for compute blocks as part of the"
          << ";             abnormal action. This will leave the I/O node software in an unknown state and may cause"
          << ";             spurious RAS events. The status of all linked I/O nodes is changed to Software (F)ailure"
          << ";             when using this option for compute blocks."
          << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
