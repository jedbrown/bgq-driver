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

#include "StartHwPolling.h"

#include "../env/LocationList.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

StartHwPolling*
StartHwPolling::build() {
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);           // does not require a BlockController object
    commandAttributes.requiresConnection(false);      // does not require connected target set
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);          // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(false);
    commandAttributes.helpCategory(common::ADMIN);    // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new StartHwPolling("start_hw_polling", "start_hw_polling <type> <location> <seconds>", commandAttributes);
}

void
StartHwPolling::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    int seconds = 0;
    try {
        seconds = boost::lexical_cast<unsigned>(args[2]);
        if ( seconds <= 0 ) {
            reply << mmcs_client::FAIL << ";Invalid value for seconds: " << args[2] << mmcs_client::DONE;
            return;
        }
    } catch (const boost::bad_lexical_cast& e) {
        reply << mmcs_client::FAIL << ";Invalid value for seconds" << mmcs_client::DONE;
        return;
    }

    if (args[0]  != "service" && args[0] != "node" && args[0] != "io" && args[0] != "bulk") {
        reply << mmcs_client::FAIL << ";Polling could not be started, " << args[0] << " is an invalid type" << mmcs_client::DONE;
        return;
    }

    if ( env::LocationList::instance().start(args[1], args[0], seconds) ) {
        reply << mmcs_client::OK << mmcs_client::DONE;
    } else {
        reply << mmcs_client::FAIL << ";Polling could not be started, or is already in effect for location " << args[1] << mmcs_client::DONE;
    }
}

void
StartHwPolling::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Start polling a specific hardware location for environmental readings"
          << ";supported values for type are service, node, io, and bulk"
          << ";location supports regular expressions"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
