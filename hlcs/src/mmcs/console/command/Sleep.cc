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

#include "Sleep.h"

#include <unistd.h>

using namespace std;

namespace mmcs {
namespace console {
namespace command {

Sleep*
Sleep::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new Sleep("sleep", "sleep <seconds>", commandAttributes);
}

void
Sleep::execute(deque<string> args,
               mmcs_client::CommandReply& reply,
               common::ConsoleController* pController,
               server::BlockControllerTarget* pTarget)
{
    if (args.size() != 1) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    try {
        const int secs = boost::lexical_cast<int>( args[0] );
        if ( secs <= 0 ) {
            reply << mmcs_client::FAIL << "Number of seconds must be > 0" << mmcs_client::DONE;
            return;
        }

        sleep(secs);
        reply << mmcs_client::OK << mmcs_client::DONE;
    } catch ( const boost::bad_lexical_cast& e ) {
        reply << mmcs_client::FAIL << "Invalid number of seconds: " << args[0] << mmcs_client::DONE;
    }
}

void
Sleep::help(deque<string> args,
            mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
      << ";Pause for the specified number of seconds"
      << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
