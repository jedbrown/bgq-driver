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

#include "RedirectInput.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

namespace mmcs {
namespace console {
namespace command {

RedirectInput*
RedirectInput::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a BlockControllerTarget object
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new RedirectInput("<", "< <filename>", commandAttributes);
}

void
RedirectInput::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    if (common::Properties::getProperty(NO_SHELL) == "true") {
        reply << mmcs_client::FAIL << mmcs_client::DONE;
        return;
    }

    if (args.size() != 1) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    FILE* const fin = fopen(args[0].c_str(), "r");
    if (!fin) {
        reply << mmcs_client::FAIL << "Cannot open \"" << args[0] << "\"" << mmcs_client::DONE;
        return;
    }

    if (!pController->pushInput(fin)) {
        reply << mmcs_client::FAIL << "Input stream recursion too deep" << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
RedirectInput::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";Reads subsequent commands from <filename>."
        << ";Returns to reading stdin when EOF is reached on <filename>"
        << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
