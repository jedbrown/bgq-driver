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

#include "AllocateBlock.h"

#include "../DBBlockController.h"
#include "../DBConsoleController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

AllocateBlock*
AllocateBlock::build()
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
    return new AllocateBlock("allocate_block", "allocate_block <blockId>", commandAttributes);
}

void
AllocateBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    execute(args, reply, pController, pTarget);
}

void
AllocateBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    BGQDB::BLOCK_STATUS bState;
    const BGQDB::STATUS result = BGQDB::getBlockStatus(args[0], bState);
    if ( result != BGQDB::OK ) {
        reply << mmcs_client::FAIL << "Could not get block status: " << result << mmcs_client::DONE;
        return;
    }

    if ( bState != BGQDB::FREE ) {
        reply << mmcs_client::FAIL << "Block is not free" << mmcs_client::DONE;
        return;
    }

    if (!DBConsoleController::setAllocating(args[0])) {
        reply << mmcs_client::FAIL << "Block is being allocated or freed in another thread" << mmcs_client::DONE;
        return;
    }

    //  select the block
    pController->selectBlock(args, reply, false);
    if (reply.getStatus() != 0) {
        DBConsoleController::doneAllocating(args[0]);
        return;
    }

    // allocate the block
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->allocateBlock(args, reply);
    if (reply.getStatus() != 0) {
        pController->deselectBlock();
    }
    DBConsoleController::doneAllocating(args[0]);
}

void
AllocateBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";For specified <blockId>, marks block as allocated but does not boot the block."
          << ";options:"
          << ";  no_connect - don't connect to block hardware"
          << ";  pgood - reset pgood on block hardware"
          << ";  diags - enable block to be created when components are in Service status"
          << ";  no_check - enable block to be created when nodes are in Error status"
          << ";  shared - enable node board resources to be shared between blocks. Implies no_connect."
          << ";  svchost_options=<svc_host_configuration_file>"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
