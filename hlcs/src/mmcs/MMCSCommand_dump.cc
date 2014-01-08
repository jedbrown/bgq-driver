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
 * \file MMCSCommand_dump.cc
 */

#include "BlockControllerTarget.h"
#include "BlockControllerBllInfo.h"
#include "CNBlockController.h"
#include "ConsoleController.h"
#include "IOBlockController.h"
#include "MMCSCommandReply.h"
#include "MMCSCommand_dump.h"

#include <control/include/bgqconfig/BGQPersonality.h>

using namespace std;

MMCSCommand_dump_machine*
MMCSCommand_dump_machine::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new MMCSCommand_dump_machine("dump_machine", "dump_machine", commandAttributes);
}

void
MMCSCommand_dump_machine::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    BlockPtr pBlock = pController->getBlockBaseController();    // get selected block

    if (pBlock->getMachineXML() != NULL)
        reply << OK << *(pBlock->getMachineXML()) << DONE;
    else
        reply << OK << DONE;
}

void
MMCSCommand_dump_machine::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description()
        << ";Print the BGQMachineXML configuration object"
        << DONE;

}

MMCSCommand_dump_block*
MMCSCommand_dump_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_dump_block("dump_block", "dump_block", commandAttributes);
}

void
MMCSCommand_dump_block::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    BlockPtr pBlock = pController->getBlockBaseController();    // get selected block

    if (pBlock->getBlockXML() != NULL)
        reply << OK << *(pBlock->getBlockXML()) << DONE;
    else
        reply << OK << DONE;
}

void
MMCSCommand_dump_block::help(
        deque<string> args,
        MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Print the BGQBlockXML configuration object"
        << DONE;

}

MMCSCommand_dump_personalities*
MMCSCommand_dump_personalities::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_dump_personalities("dump_personalities", "[<target>] dump_personalities [unformatted]", commandAttributes);
}

void
MMCSCommand_dump_personalities::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    bool formatted = true;

    if (args.size() > 0)
    {
        if (args[0].compare("unformatted") == 0)
            formatted = false;
        else
        {
            reply << FAIL <<  "args? " << usage <<  DONE;
            return;
        }
    }

    reply << OK;
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
    {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        reply << "{" << nodeInfo->_locateId << "} " << "Dumping personality:";
        if (!formatted)
        {
            uint8_t numPersonalityWords = nodeInfo->personality().PersonalitySizeWords;
            unsigned* personalityWord = (unsigned*) &nodeInfo->personality();

            for (unsigned i = 0; i < numPersonalityWords; ++i)
            {
                if (i % 4 == 0)
                    reply << "\n" << hex << setw(8) << setfill('0') << i*4 << ":";
                reply << " " << hex << setw(8) << setfill('0') << *personalityWord;
                ++personalityWord;
            }
            reply << dec;
        }
        else
            reply << "\n" << nodeInfo->personality() << "\n";
    }
    for (unsigned i = 0; i < pTarget->getLinkchips().size(); ++i)
    {
        BCLinkchipInfo *bllInfo = pTarget->getLinkchips()[i];
        reply << "{" << bllInfo->_locateId << "} " << "Dumping personalities:";
        //     for (unsigned i = 0; i < bllInfo->_chipRoute.size(); ++i) {
        //       for (unsigned j = 0; j < bllInfo->_chipRoute[i]._bgqLinkChipRouteSetting.size(); ++j) {
        //         reply << "\n" << bllInfo->_chipRoute[i]._bgqLinkChipRouteSetting[j] << "\n";
        //       }
        //     }
    }
    reply << DONE;
}

void
MMCSCommand_dump_personalities::help(
        deque<string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description()
        << ";Print the node personality objects"
        << DONE;

}

MMCSCommand_get_block_size*
MMCSCommand_get_block_size::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(DEFAULT);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_get_block_size("get_block_size", "get_block_size [ a | b | c | d | e | * ]", commandAttributes);
}

void
MMCSCommand_get_block_size::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    const BlockPtr block = pController->getBlockBaseController();
    const CNBlockPtr compute_block = boost::dynamic_pointer_cast<CNBlockController>( block );
    const IOBlockPtr io_block = boost::dynamic_pointer_cast<IOBlockController>( block );
    const BGQBlockNodeConfig* bnc = block->getBlockNodeConfig();

    reply << OK;
    if ( compute_block ) {
        this->compute( args, bnc, reply );
    } else if ( io_block ) {
        this->io( args, bnc, reply );
    } else {
        reply << FAIL << args[0] << " is not a compute or I/O block" << DONE;
    }
}

void
MMCSCommand_get_block_size::io(
        const std::deque<std::string>& args,
        const BGQBlockNodeConfig* bnc,
        MMCSCommandReply& reply
        )
{
    if      (args[0] == "a")  reply << bnc->aIONodeSize();
    else if (args[0] == "b")  reply << bnc->bIONodeSize();
    else if (args[0] == "c")  reply << bnc->cIONodeSize();
    else if (args[0] == "d")  reply << bnc->dIONodeSize();
    else if (args[0] == "*")  {
        reply << bnc->aIONodeSize() << "x"
            << bnc->bIONodeSize() << "x"
            << bnc->cIONodeSize() << "x"
            << bnc->dIONodeSize();
    } else {
        reply << FAIL << "args?";
    }
    reply << DONE;
}

void
MMCSCommand_get_block_size::compute(
        const std::deque<std::string>& args,
        const BGQBlockNodeConfig* bnc,
        MMCSCommandReply& reply
        )
{
    if      (args[0] == "a")  reply << bnc->aNodeSize();
    else if (args[0] == "b")  reply << bnc->bNodeSize();
    else if (args[0] == "c")  reply << bnc->cNodeSize();
    else if (args[0] == "d")  reply << bnc->dNodeSize();
    else if (args[0] == "e")  reply << bnc->eNodeSize();
    else if (args[0] == "*")  {
        reply << bnc->aNodeSize() << "x"
            << bnc->bNodeSize() << "x"
            << bnc->cNodeSize() << "x"
            << bnc->dNodeSize() << "x"
            << bnc->eNodeSize();
    } else {
        reply << FAIL << "args?";
    }
    reply << DONE;
}
void
MMCSCommand_get_block_size::help(
        deque<string> args,
        MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Retrieve the block dimensions of selected block."
        << ";Specifiying a, b, c, d, e, or io retrieves that one dimension."
        << ";Specifying * retrieves all dimensions."
        << DONE;
}
