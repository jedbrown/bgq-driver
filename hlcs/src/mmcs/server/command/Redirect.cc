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

#include "Redirect.h"

#include "../BlockControllerBase.h"
#include "../BlockControllerTarget.h"
#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include "libmmcs_client/ConsolePort.h"

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

void
Redirect::redirect_block(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController
    )
{
    if (args.size() == 0) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }
    if (args[0] != "on" && args[0] != "off") {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());    // get selected block

    if (!pBlock->getBase()->isConnected()) {
        const std::deque<std::string> args( 1, "mode=monitor" );
        const std::string targspec = "{*}";
        const BlockControllerTarget target(pBlock->getBase(), targspec, reply);
        pBlock->getBase()->connect(args, reply, &target);
        if (reply.getStatus() != 0) {
            return;
        }
    }
    if (args[0] == "on") {
        if (
                pController->getRedirecting() ||            // is this DBConsoleController already redirecting?
                pBlock->getBase()->getRedirectSock() != 0   // is the DBBlockController already redirecting?
           )
        {
            reply << mmcs_client::FAIL << "Block is already redirected in another process" << mmcs_client::DONE;
            return;
        }
        pBlock->getBase()->startRedirection(boost::static_pointer_cast<CxxSockets::SecureTCPSocket>(pController->getConsolePort()->getSock()), reply);
        if (reply.getStatus() != 0)
            return;
        pController->setRedirecting(true);
    } else {
        if (!pController->getRedirecting()) {   // is this DBConsoleController redirecting?
            reply << mmcs_client::FAIL << "Client not redirecting" << mmcs_client::DONE;
            return;
        }
        pBlock->getBase()->stopRedirection(reply);
        pController->setRedirecting(false);
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}


Redirect*
Redirect::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);              // 'help user'  will include this command's summary
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new Redirect("redirect", "redirect <blockid> on|off", commandAttributes);
}


void
Redirect::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    return execute(args, reply, pController, pTarget);
}

void
Redirect::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if (args.size() != 2) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }
    pController->selectBlock(args, reply, true);
    if (reply.getStatus() != 0) {
        return;
    }
    args.pop_front();       // remove block name from args
    redirect_block(args, reply, pController);
    if (reply.str() == "args?") {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
    }
}

void
Redirect::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Redirect I/O node output for the specified block to the mmcs console."
          << ";Directs subsequent mailbox output back to the socket connection that this command is received on."
          << ";Allocating or freeing the block will stop the mailbox redirection."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
