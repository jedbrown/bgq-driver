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

#include "Sysrq.h"

#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"
#include "../BlockControllerTarget.h"

#include <control/include/mcServer/MCServerRef.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

Sysrq*
Sysrq::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(true);        // does require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new Sysrq("sysrq", "[<target>] sysrq [options]", commandAttributes);
}

void
Sysrq::execute(
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

    // magic sysrq prefix (^O)
    // For now sysrq is a magic write_con with this as a prefix.
    // We could change to a special mbox message code if necessary.
    text += "\017";
    for (unsigned i = 0; i < args.size(); ++i) {
        text += args[i];
        if (i != args.size() - 1)
            text += " ";
    }

    BlockPtr pBlock = pTarget->getBlockController();    // get selected block

    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        if (pBlock->isIoBlock()) {
            if (nodeInfo->_state == NST_PROGRAM_RUNNING) {
                // Send a MailboxRequest to the I/O node
                MCServerMessageSpec::MailboxRequest mcMailboxRequest;
                mcMailboxRequest._text = text;
                mcMailboxRequest._location.push_back(nodeInfo->location());
                MCServerMessageSpec::MailboxReply   mcMailboxReply;
                try {
                    pBlock->getMCServer()->mailbox(mcMailboxRequest, mcMailboxReply);
                } catch (const exception& e) {
                    mcMailboxReply._rc = -1;
                    mcMailboxReply._rt = e.what();
                } catch (const XML::Exception& e) {
                    ostringstream buf; buf << e;
                    mcMailboxReply._rc = -1;
                    mcMailboxReply._rt = buf.str();
                }
                if (mcMailboxReply._rc) {
                    reply << mmcs_client::FAIL << mcMailboxReply._rt << mmcs_client::DONE;
                    return;
                }
            } else  {
                reply << mmcs_client::FAIL << "No program running" << mmcs_client::DONE;
                return;
            }
        } else {
            reply << mmcs_client::FAIL << "Not an I/O node" << mmcs_client::DONE;
            return;
        }
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
Sysrq::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Sends a sysrq command to the I/O node"
          << ";Options:"
          << ";    loglevel0-8 reBoot tErm Halt kIll showMem showPc unRaw Sync showTasks Unmount Xmon"
          << mmcs_client::DONE;

}

} } } // namespace mmcs::server::command
