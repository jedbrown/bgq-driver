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

#include "RedirectBlock.h"

#include "Redirect.h"

#include "common/ConsoleController.h"

#include "../../MMCSCommandProcessor.h"

using namespace std;

namespace mmcs {
namespace console {
namespace command {

RedirectBlock*
RedirectBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // internal use only command
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.helpCategory(common::USER);              // 'help user'  will include this command's summary
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new RedirectBlock("redirect_block", "redirect_block on|off stdout|stderr", commandAttributes);
}

void
RedirectBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    // get the currently selected block
    deque<string> list_selected_block = MMCSCommandProcessor::parseCommand("mmcs_server_cmd list_selected_block");
    pController->getCommandProcessor()->execute(list_selected_block, reply, pController);
    if (reply.getStatus() != 0) {
        if (reply.str() == "args?")
            reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    // add the name of the selected block to the front of the argument list
    args.push_front(reply.str());

    // redirect the block output
    Redirect::redirect(args, reply, pController, pTarget);
    if (reply.str() == "args?")
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
}

void
RedirectBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
      << ";Redirect I/O node output for the selected block back to the mmcs console."
      << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
      << ";Allocating or freeing the block will stop the mailbox redirection."
      << ";By default, redirection goes to standard out. The stdout|stderr option"
      << ";allows the user to optionally send the output to standard error"
      << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
