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

#include "Connect.h"

#include "../BlockControllerBase.h"
#include "../BlockHelper.h"

#include "common/ConsoleController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

Connect*
Connect::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::ADMIN);              // 'help admin'  will include this command's summary
    Attributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    Attributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new Connect("connect", "[<target>] connect [ options ]", commandAttributes);
}

void
Connect::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    const BlockPtr pBlock = pController->getBlockHelper()->getBase();  // get selected block
    pBlock->connect(args, reply, pTarget);
}

void
Connect::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Connect to a set of the block's resources"
          << ";options:"
          << ";  targetset=[ temp | perm ]                              - create the mcServer target set as temporary (default) or permanent"
          << ";  outfile=<filename>                                     - direct the console messages to a file, must be fully qualified path"
          << ";                                                           and not already exist"
          << ";  rasfile=<filename>                                     - direct the RAS messages to a file, must be fully qualified path and"
          << ";                                                           not already exist"
          << ";  mode=[control | debug | service | reserve | monitor]   - specify mode to open target set."
          << ";  pgood                                                  - reset pgood on block hardware"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
