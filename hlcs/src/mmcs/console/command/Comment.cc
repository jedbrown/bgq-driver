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

#include "Comment.h"

#include "common/Properties.h"

using namespace std;

namespace mmcs {
namespace console {
namespace command {

Comment*
Comment::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new Comment("#", "# [<comment>]", commandAttributes);
}

void
Comment::execute(deque<string> ,//args,
                 mmcs_client::CommandReply& reply,
                 common::ConsoleController* ,//pController,
                 server::BlockControllerTarget* /*pTarget*/)
{
    if (common::Properties::getProperty(NO_SHELL) == "true") {
        reply << mmcs_client::FAIL << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
Comment::help(deque<string> ,//args,
              mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
      << ";Indicates a comment line"
      << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
