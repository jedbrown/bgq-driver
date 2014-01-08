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

#include "ListSelectedBlock.h"

#include "../BlockControllerBase.h"
#include "../BlockHelper.h"
#include "../DBConsoleController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

ListSelectedBlock*
ListSelectedBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require  mc_server connections
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // internal use only commmand
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);              // 'help user'  will include this command's summary
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new ListSelectedBlock("list_selected_block", "list_selected_block", commandAttributes);
}

void ListSelectedBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if (pController->getBlockHelper() == 0) {
        reply << mmcs_client::FAIL << "Block not selected" << mmcs_client::DONE;
    } else {
        return execute(args, reply, pController, pTarget);
    }
}

void
ListSelectedBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    reply << mmcs_client::OK << pController->getBlockHelper()->getBase()->getBlockName() << mmcs_client::DONE;
}

void
ListSelectedBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Prints the name of the currently selected block"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
