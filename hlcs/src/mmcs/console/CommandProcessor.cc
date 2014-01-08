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

#include "CommandProcessor.h"

#include "ExternalCommand.h"

#include "common/AbstractCommand.h"
#include "common/ConsoleController.h"

using namespace std;

namespace mmcs {
namespace console {

CommandProcessor::CommandProcessor(
        MMCSCommandMap* mmcsCommands
        ) :
    MMCSCommandProcessor(mmcsCommands)
{
    logFailures(false);
    _bg_console = true;
}

procstat
CommandProcessor::invokeCommand(
        deque<string> cmdStr,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget*  pTarget,
        common::AbstractCommand*  pCmd,
        procstat status,
        std::vector<std::string>& //validnames
        )
{
    // execute the command
    if (status == 0) {
        try {
            status = CMD_EXECUTED;
            pCmd->execute(cmdStr, reply, pController, pTarget);
        } catch (const exception &e) {
            reply << mmcs_client::ABORT << e.what() << mmcs_client::DONE;
        }
    } else if (status == CMD_EXTERNAL) {
        static_cast<ExternalCommand*>(pCmd)->execute(cmdStr, reply, pController, pTarget);
    } else if (status == CMD_NOT_FOUND) {
        // if the command is not available locally,
        // try to forward the command to the mmcs server
        if (pController->getConsolePort() != NULL) {
            status = execute("mmcs_server_cmd", cmdStr, reply, pController);
            if (status == CMD_NOT_FOUND) { // prevent infinite loop
                status = CMD_INVALID;
                reply << mmcs_client::FAIL << "Internal failure: mmcs_server_cmd is missing" << mmcs_client::DONE;
            }
        } else
            reply << mmcs_client::FAIL << "Lost connection to mmcs_server, use mmcs_server_connect to reconnect" << mmcs_client::DONE;
    }

    return status;
}

#ifdef WITH_DB

void
CommandProcessor::executeExternal(deque<string>& cmdStr,
                                             mmcs_client::CommandReply& reply,
                                             common::ConsoleController* pController,
                                             server::BlockControllerTarget* pTarget)
{
    // Find the command and call its execute with the args.
}

#endif

} } // namespace mmcs::console
