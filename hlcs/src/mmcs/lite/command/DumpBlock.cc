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


#include "DumpBlock.h"

#include "server/BlockControllerBase.h"
#include "server/BlockHelper.h"

#include "common/ConsoleController.h"


namespace mmcs {
namespace lite {
namespace command {


DumpBlock*
DumpBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new DumpBlock("dump_block", "dump_block", commandAttributes);
}


void
DumpBlock::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    server::BlockPtr pBlock = pController->getBlockHelper()->getBase();    // get selected block

    if (pBlock->getBlockXML() != NULL)
        reply << mmcs_client::OK << *(pBlock->getBlockXML()) << mmcs_client::DONE;
    else
        reply << mmcs_client::OK << mmcs_client::DONE;
}


void
DumpBlock::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
        << ";Print the BGQBlockXML configuration object"
        << mmcs_client::DONE;

}


} } } // namespace mmcs::lite::command
