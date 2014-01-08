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

#include "WaitBoot.h"

#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include <utility/include/Log.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

WaitBoot*
WaitBoot::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new WaitBoot("wait_boot", "wait_boot [<minutes>]", commandAttributes);
}

void
WaitBoot::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());    // get selected block

    pBlock->waitBoot(args, reply, true);
    if (reply.str() == "args?") {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }
}

void
WaitBoot::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Wait for the selected block to complete booting."
          << ";<minutes> specifies the maximum time to wait, in minutes. The default is 20 minutes."
          << ";<minutes> can be a fraction, example: 1.25 for 75 seconds."
          << ";The command does not complete until the block is fully initialized to the point"
          << ";that kernels are ready for job submission or the wait time has expired."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
