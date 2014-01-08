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

#include "GetBlockSize.h"

#include "../BlockHelper.h"
#include "../CNBlockController.h"
#include "../IOBlockController.h"

#include "common/ConsoleController.h"

namespace mmcs {
namespace server {
namespace command {

GetBlockSize*
GetBlockSize::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new GetBlockSize("get_block_size", "get_block_size [ a | b | c | d | e | * ]", commandAttributes);
}

void
GetBlockSize::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    const BlockPtr block = pController->getBlockHelper()->getBase();
    const CNBlockPtr compute_block = boost::dynamic_pointer_cast<CNBlockController>( block );
    const IOBlockPtr io_block = boost::dynamic_pointer_cast<IOBlockController>( block );
    const BGQBlockNodeConfig* bnc = block->getBlockNodeConfig();

    reply << mmcs_client::OK;
    if ( compute_block ) {
        this->compute( args, bnc, reply );
    } else if ( io_block ) {
        this->io( args, bnc, reply );
    } else {
        reply << mmcs_client::FAIL << args[0] << " is not a compute or I/O block" << mmcs_client::DONE;
    }
}

void
GetBlockSize::io(
        const std::deque<std::string>& args,
        const BGQBlockNodeConfig* bnc,
        mmcs_client::CommandReply& reply
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
        reply << mmcs_client::FAIL << "args?";
    }
    reply << mmcs_client::DONE;
}

void
GetBlockSize::compute(
        const std::deque<std::string>& args,
        const BGQBlockNodeConfig* bnc,
        mmcs_client::CommandReply& reply
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
        reply << mmcs_client::FAIL << "args?";
    }
    reply << mmcs_client::DONE;
}

void
GetBlockSize::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Retrieve the block dimensions of selected block."
          << ";Specifiying a, b, c, d, e, or io retrieves that one dimension."
          << ";Specifying * retrieves all dimensions."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
