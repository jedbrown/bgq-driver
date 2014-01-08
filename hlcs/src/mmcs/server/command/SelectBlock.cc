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

#include "SelectBlock.h"

#include "../DBConsoleController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

SelectBlock*
SelectBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);           // 'help default'  will include this command's summary
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new SelectBlock("select_block", "select_block <blockId>", commandAttributes);
}

void
SelectBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if (validnames->size() != 1 || validnames->at(0) != args[0]) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }
    pController->selectBlock(args, reply, true);
}

void
SelectBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> vn;
    vn.push_back(args[0]);
    execute(args, reply, pController, pTarget, &vn);
}

void
SelectBlock::help(deque<string> args,
                   mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
          << ";Select an already allocated block to work with."
          << ";Does not initialize the block nor reset icon connections. "
          << ";This command is intended to be used to reconnect to an"
          << ";allocated block from another bg_console session."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
