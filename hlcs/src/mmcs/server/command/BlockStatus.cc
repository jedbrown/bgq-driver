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

#include "BlockStatus.h"

#include "../BCNodeInfo.h"
#include "../BlockControllerBase.h"
#include "../BlockControllerTarget.h"
#include "../BlockHelper.h"

#include "common/ConsoleController.h"

#include "libmmcs_client/CommandReply.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

BlockStatus*
BlockStatus::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // requires a BlockController object
    commandAttributes.requiresConnection(false);      // does not requires an  mc_server connections
    commandAttributes.requiresTarget(true);           // requires a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new BlockStatus("block_status", "[<target>] block_status", commandAttributes);
}

void
BlockStatus::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    BlockPtr pBlock = pController->getBlockHelper()->getBase();    // get selected block
    if (!pBlock->getNodes().size() && !pBlock->getIcons().size()) {
        reply << mmcs_client::FAIL << "Not connected" << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::OK;
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo* nodeInfo = pTarget->getNodes()[i];
        if (nodeInfo->isIOnode()) {
            nodeInfo = pBlock->getNodes()[i];
        }

        reply << "{" << nodeInfo->_locateId << "} " << (nodeInfo->isIOnode() ? "io     " : "compute" );

        if (nodeInfo->_open == false) {
            reply << " [disconnected]";
        } else {
            switch (nodeInfo->_state) {
                case NST_IN_RESET:
                    reply << " [vacant]";
                    break;
                case NST_PROGRAM_RUNNING:
                    reply << " [program running]";
                    break;
                case NST_TERMINATED:
                    reply << " [terminated]";
                    break;
            }
            if (nodeInfo->isIOnode()) {
                if (nodeInfo->_initialized)
                    reply << " [initialized]";
                else if (nodeInfo->_haltComplete)
                    reply << " [halt complete]";
            }
        }
        reply << "\n";
    }
    reply << mmcs_client::DONE;
}

void
BlockStatus::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Lists nodes in allocated block, the type of each node, and whether it is running a program or vacant."
          << ";Nodes in disconnected targets will show as \"disconnected\". I/O block targets are disconnected after booting."
          << ";(If block has been booted, it will show that a program is running.)"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
