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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include "ShowBarrier.h"

#include "../BlockControllerBase.h"
#include "../BlockHelper.h"

#include "common/ConsoleController.h"

#include "libmmcs_client/CommandReply.h"

#include <utility/include/Log.h>

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

ShowBarrier*
ShowBarrier::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new ShowBarrier("show_barrier", "show_barrier", commandAttributes);
}

void
ShowBarrier::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    BlockPtr pBlock = pController->getBlockHelper()->getBase(); // get selected block
    pBlock->show_barrier(reply);
    if (reply.str() == "args?") {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
    }
}

void
ShowBarrier::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Show nodes with unsatisfied Control System barrier bits in the selected block"
          << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
