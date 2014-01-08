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

#include "ListHwPolling.h"

#include "../env/LocationList.h"

namespace mmcs {
namespace server {
namespace command {

ListHwPolling*
ListHwPolling::build() {
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);           // does not require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require connected target set
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new ListHwPolling("list_hw_polling", "list_hw_polling", commandAttributes);
}

void
ListHwPolling::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    env::LocationList::instance().list(reply);
}

void
ListHwPolling::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";List the specific hardware locations that have polling in effect"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
