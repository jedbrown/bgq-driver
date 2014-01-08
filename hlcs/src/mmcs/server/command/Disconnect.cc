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

#include "Disconnect.h"

#include "../BlockControllerBase.h"
#include "../BlockHelper.h"

#include "common/ConsoleController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

Disconnect*
Disconnect::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::ADMIN);              // 'help admin'  will include this command's summary
    Attributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    Attributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new Disconnect("disconnect", "disconnect", commandAttributes);
}

void
Disconnect::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    const BlockPtr pBlock = pController->getBlockHelper()->getBase();  // get selected block
    args.push_back( "no_shutdown" );
    pBlock->disconnect(args, reply);
}

void
Disconnect::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Disconnect from a set of the block's resources" << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
