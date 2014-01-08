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

#include "StopHwPolling.h"

#include "../env/LocationList.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

StopHwPolling*
StopHwPolling::build() {
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);           // does not require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require connected target set
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.helpCategory(common::ADMIN);    // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new StopHwPolling("stop_hw_polling", "stop_hw_polling <location>", commandAttributes);
}

void
StopHwPolling::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if ( env::LocationList::instance().stop(args[0]) ) {
        reply << mmcs_client::OK << mmcs_client::DONE;
    } else {
        reply << mmcs_client::FAIL << ";No polling in effect for location " << args[0] << mmcs_client::DONE;
    }

    return;
}

void
StopHwPolling::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Stop polling the specific hardware location for environmental readings"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
