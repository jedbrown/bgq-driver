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

#include "WriteCon.h"

#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"
#include "../BlockControllerTarget.h"

#include "common/Properties.h"

#include <control/include/mcServer/MCServerRef.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

WriteCon*
WriteCon::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new WriteCon("write_con", "[<target>] write_con <console-command>", commandAttributes);
}

void
WriteCon::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    string text;

    if (pTarget->getNodes().size() == 0) {
        reply << mmcs_client::FAIL << "No targets selected" << mmcs_client::DONE;
        return;
    }

    for (unsigned i = 0; i < args.size(); ++i) {
        text += args[i];
        if (i != args.size() - 1)
            text += " ";
    }

    BlockPtr pBlock = pTarget->getBlockController();    // get selected block

    bool wasconnected = true;
    if (!pBlock->isConnected()) {
        wasconnected = false;
        // Need to connect
        std::deque<string> pargs;
        pargs.push_back(common::Properties::getProperty(DFT_TGTSET_TYPE));
        pBlock->connect(pargs, reply, pTarget);
    }

    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];

        if (!nodeInfo->_iopos.trainOnly()) {
            // Send a MailboxRequest to the node
            MCServerMessageSpec::MailboxRequest mcMailboxRequest;
            mcMailboxRequest._text = text;
            mcMailboxRequest._location.push_back(nodeInfo->location());
            MCServerMessageSpec::MailboxReply mcMailboxReply;
            pBlock->getMCServer()->mailbox(mcMailboxRequest, mcMailboxReply);
            if (mcMailboxReply._rc) {
                reply << mmcs_client::FAIL << mcMailboxReply._rt << mmcs_client::DONE;
                return;
            }
        }
    }
    if (!wasconnected) {
        // Now disconnect since we had to connect
        std::deque<std::string> args;
        args.push_back("no_shutdown");
        pBlock->disconnect(args, reply);
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
WriteCon::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Send <console-command> to target node for execution."
          << ";Output will be returned asynchronously to mailbox (either console or I/O node log)."
          << ";Requires write access to the target set, use connect to obtain write access."
          << mmcs_client::DONE;

}

} } } // namespace mmcs::server::command
