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

/*!
 * \file MMCSCommand_block_status.cc
 */

#include "BlockControllerNodeInfo.h"
#include "BlockControllerTarget.h"
#include "ConsoleController.h"
#include "MMCSCommand_block_status.h"
#include "MMCSCommandReply.h"

MMCSCommand_block_status*
MMCSCommand_block_status::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // requires a BlockController object
    commandAttributes.requiresConnection(false);      // does not requires an  mc_server connections
    commandAttributes.requiresTarget(true);           // requires a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(DEFAULT);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_block_status("block_status", "[<target>] block_status", commandAttributes);
}

void
MMCSCommand_block_status::execute(deque<string> args,
                MMCSCommandReply& reply,
                ConsoleController* pController,
                BlockControllerTarget* pTarget)
{
    BlockPtr pBlock = pController->getBlockBaseController();    // get selected block
    if (!pBlock->getNodes().size() && !pBlock->getIcons().size())
    {
        reply << FAIL << "not connected" << DONE;
        return;
    }


    reply << OK;
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
    {
        BCNodeInfo* nodeInfo = pTarget->getNodes()[i];
        if(nodeInfo->isIOnode())
            nodeInfo = pBlock->getNodes()[i];

        reply << "{" << nodeInfo->_locateId << "} "
            << (nodeInfo->isIOnode() ? "io     " : "compute" );

        if(nodeInfo->_open == false) {
            reply << " [disconnected]";
        } else {
            switch (nodeInfo->_state)
                {
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
            if (nodeInfo->isIOnode())
                {
                    if (nodeInfo->_initialized)
                        reply << " [initialized]";
                    else if (nodeInfo->_haltComplete)
                        reply << " [halt complete]";
                }
        }
        reply << "\n";
    }
    reply << DONE;
}

void
MMCSCommand_block_status::help(deque<string> args,
             MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";Lists nodes in allocated block, the type of each node, and whether it is running a program or vacant."
          << ";Nodes in disconnected targets will show as \"disconnected\".  IO block targets are disconnected after booting."
          << ";(If block has been booted, it will show that a program is running.)"
          << DONE;
}
