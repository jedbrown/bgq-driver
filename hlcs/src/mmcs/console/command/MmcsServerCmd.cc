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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file MmcsServerCmd.cc
 * mmcs_server_cmd <command-string>
 * Send a command to the mmcs server for execution and wait for the reply
 * This command is used internally by bg_console
 */

#include "MmcsServerCmd.h"

#include "MmcsServerConnect.h"

#include "common/ConsoleController.h"

#include "libmmcs_client/CommandReply.h"
#include "libmmcs_client/ConsolePort.h"

#include <boost/scoped_ptr.hpp>

LOG_DECLARE_FILE("mmcs.console");

using namespace std;

namespace mmcs {
namespace console {
namespace command {

// Static initializations
PthreadMutex MmcsServerCmd::_reconnect_lock;
bool MmcsServerCmd::_ending;

MmcsServerCmd*
MmcsServerCmd::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::SPECIAL);
    return new MmcsServerCmd("mmcs_server_cmd", "mmcs_server_cmd [<anything>]", commandAttributes);
}

void
MmcsServerCmd::reconnect_to_server(
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController
        )
{
    PthreadMutexHolder holder;
    holder.Lock(&_reconnect_lock);

    const deque<string> args( 1, "retry" );

    boost::scoped_ptr<MmcsServerConnect> conn_cmd_ptr(MmcsServerConnect::build());
    conn_cmd_ptr->execute(args, reply, pController);
    if (reply.getStatus() == 0) {
        LOG_INFO_MSG("Reconnected to mmcs_server");
    } else {
        // We are running disconnected from the server
        LOG_ERROR_MSG(reply.str());
        LOG_ERROR_MSG("mmcs_server reconnect failed.");
    }
}

void
MmcsServerCmd::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* /*pTarget*/
        )
{
    mmcs_client::ConsolePortClient* const serverPort = dynamic_cast<mmcs_client::ConsolePortClient*>(pController->getConsolePort());
    if (serverPort == NULL) {
        reply << mmcs_client::FAIL << "mmcs_server not available" << mmcs_client::DONE;
        return;
    }

    // send the command to the server for execution
    string cmdString;
    for (unsigned i = 0; i < args.size(); ++i)
        cmdString.append(args[i]).append(" ");

    try {
        while (true) {
            try {
                serverPort->sendMessage(cmdString);
            } catch (const mmcs_client::ConsolePort::Error &e) {
                switch (e.errcode) {
                    case EINTR:
                    case EAGAIN:
                        sleep(1);
                        continue;
                    default:
                        reconnect_to_server(reply, pController);
                        continue;
                }
            }
            break;
        }

        // loop receiving command output until end of reply is received
        string replyString;
        bool nullTerm = false;
        bool eom = false;
        reply.reset();

        do {
            bool server_failed = false;
            try {
                nullTerm = serverPort->receiveMessage(replyString); // nullTerm == true if '\0' received
            }
            catch (const mmcs_client::ConsolePort::Error &e) {
                switch (e.errcode) {
                    case EINTR:
                    case EAGAIN:
                        continue;
                    default:
                        server_failed = true;
                        if (cmdString.find("replyformat") == std::string::npos &&
                                cmdString.find("redirect") == std::string::npos)
                            reconnect_to_server(reply, pController);
                        else {
                            reply << mmcs_client::FAIL << "mmcs_server aborted connection" << mmcs_client::DONE;
                        }
                }
            }
            if (replyString.length() == 0) {
                if (!server_failed) {
                    reply << mmcs_client::ABORT << "Internal error: null reply from server" << mmcs_client::DONE;
                    return;
                } else {
                    reply << mmcs_client::FAIL << "Server failed, reconnect attempted. Retry the command." << mmcs_client::DONE;
                    return;
                }
            }

            // reconstruct the mmcs_client::CommandReply from the server response
            if (reply.getStatus() == mmcs_client::CommandReply::STATUS_NOT_SET) {
                reply.assign(replyString);
            } else {
                reply.append(replyString);
            }
            replyString.clear();
            // exit loop if a null terminator was received
            eom = nullTerm;
        } while (!eom);

        if (!reply.isDone())
            reply << mmcs_client::DONE;
    } catch (const exception &e) {
        reply << mmcs_client::ABORT << e.what() << mmcs_client::DONE;
    }
}

bool
MmcsServerCmd::checkArgs(
        std::deque<std::string>& args
        )
{
    if (args.size() == 0)
        return false;
    else
        return true;
}

void
MmcsServerCmd::help(
        deque<string> /* args */,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";Send a command to the mmcs_server for execution and wait for the reply."
        << ";This command is used internally by bg_console."
        << mmcs_client::DONE;
}

} } } // namespace mmcs::console::command
