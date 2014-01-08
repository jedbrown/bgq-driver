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

#include "common/AbstractCommand.h"


using namespace std;

namespace mmcs {
namespace server {

#ifdef WITH_DB

CommandProcessor::CommandProcessor(MMCSCommandMap* mmcsCommands) :
    MMCSCommandProcessor(mmcsCommands)
{

}

procstat
CommandProcessor::invokeCommand(
        deque<string> cmdStr,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget*  pTarget,
        common::AbstractCommand*  pCmd,
        procstat status,
        std::vector<std::string>& validnames
        )
{
    // execute the command
    if (status == 0) {
        try {
            status = CMD_EXECUTED;

            if(!validnames.empty())// && pCmd->attributes().requiresBlock() == false)
                pCmd->execute(cmdStr, reply, (server::DBConsoleController*) pController, pTarget, &validnames);
            else
                pCmd->execute(cmdStr, reply, (server::DBConsoleController*) pController, pTarget);
        } catch (const exception &e) {
            reply << mmcs_client::ABORT << e.what() << mmcs_client::DONE;
        }
    } else if (status == CMD_NOT_FOUND) {
        reply << mmcs_client::FAIL << "Command not found" << mmcs_client::DONE;
    }

    return status;
}

procstat
CommandProcessor::validate(
            std::deque<std::string>& cmdStr,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget** pTarget,
            common::AbstractCommand** pCmd,
            std::vector<std::string>* validnames)
{
    return MMCSCommandProcessor::validate(cmdStr, reply, pController, pTarget, pCmd, validnames);
}

#endif // WITH_DB


} } // namespace mmcs::server
