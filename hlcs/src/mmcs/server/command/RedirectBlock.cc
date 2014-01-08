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

namespace mmcs {
namespace server {
namespace command {

RedirectBlock*
RedirectBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // command is for internal use
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);              // 'help user'  will include this command's summary
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new RedirectBlock("redirect_block", "redirect_block on|off", commandAttributes);
}

void
RedirectBlock::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    Redirect::redirect_block(args, reply, pController);
    if (reply.str() == "args?") {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
    }
}

void
RedirectBlock::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Redirect I/O node output for the selected block to the mmcs console."
          << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
          << ";Allocating or freeing the block will stop the mailbox redirection."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
