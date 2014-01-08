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

#include "DeselectBlock.h"

#include "../DBConsoleController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

DeselectBlock*
DeselectBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::DEFAULT);           // 'help default'  will include this command's summary
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new DeselectBlock("deselect_block", "deselect_block", commandAttributes);
}

void
DeselectBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    pController->deselectBlock();
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
DeselectBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Stop working with the selected block" << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
