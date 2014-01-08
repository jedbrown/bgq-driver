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

#include "Quit.h"

#include "common/ConsoleController.h"

using namespace std;

namespace mmcs {
namespace console {
namespace command {

Quit*
Quit::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new Quit("quit", "quit", commandAttributes);
}

void
Quit::execute(deque<string> args,
                 mmcs_client::CommandReply& reply,
                 common::ConsoleController* pController,
                 server::BlockControllerTarget* pTarget)
{
    pController->quit(EXIT_SUCCESS);
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
Quit::help(deque<string> args,
                  mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
      << ";end the console."
      << ";This command does not change the status of any allocated blocks"
      << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
