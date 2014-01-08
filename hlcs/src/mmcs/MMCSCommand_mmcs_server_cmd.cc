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
 * \file MMCSCommand_mmcs_server_command.cc
 * mmcs_server_cmd <command-string>
 * Send a command to the mmcs server for execution and wait for the reply
 * This command is used internally by mmcs_console
 */

#include "MMCSCommand_mmcs_server_cmd.h"
#include "MMCSCommandReply.h"
#include "MMCSConsolePort.h"
#include "ConsoleController.h"
#include "MMCSCommand_mmcs_server_connect.h"

#include <boost/scoped_ptr.hpp>

using namespace std;

LOG_DECLARE_FILE("mmcs");

// Static initializations
PthreadMutex MMCSCommand_mmcs_server_cmd::_reconnect_lock;
bool MMCSCommand_mmcs_server_cmd::_ending;

MMCSCommand_mmcs_server_cmd*
MMCSCommand_mmcs_server_cmd::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(SPECIAL);
    return new MMCSCommand_mmcs_server_cmd("mmcs_server_cmd", "mmcs_server_cmd [<anything>]", commandAttributes);
}

void MMCSCommand_mmcs_server_cmd::reconnect_to_server(
                         MMCSCommandReply& reply,
                         ConsoleController* pController)
{
    PthreadMutexHolder holder;
    holder.Lock(&_reconnect_lock);
    delete pController->getConsolePort();
    pController->setConsolePort(NULL);
    reply << ABORT << "lost connection to mmcs_server; reconnecting..." << DONE;
    std::string cn = "retry";
    deque<string> mmcs_connect;
    mmcs_connect.push_back(cn);
    boost::scoped_ptr<MMCSCommandReply> conn_reply( new MMCSCommandReply(1, pController->getReplyFormat(), false) );
    MMCSCommand_mmcs_server_connect* conncmd = MMCSCommand_mmcs_server_connect::build();
    conncmd->execute(mmcs_connect, *conn_reply, pController);
    if (conn_reply->getStatus() == 0) {
        LOG_INFO_MSG("reconnected to mmcs server");
    } else {
        // we are running disconnected from the server
        LOG_ERROR_MSG(conn_reply->str());
        LOG_ERROR_MSG("mmcs_server reconnect failed.");
        pController->setReplyFormat(1);
    }
}

void
MMCSCommand_mmcs_server_cmd::execute(deque<string> args,
				     MMCSCommandReply& reply,
				     ConsoleController* pController,
				     BlockControllerTarget* pTarget)
{
    MMCSConsolePortClient* serverPort = (MMCSConsolePortClient*) pController->getConsolePort();
    if (serverPort == NULL)
    {
	reply << FAIL << "mmcs_server not available" << DONE;
	return;
    }

    // send the command to the server for execution
    string cmdString;
    for (unsigned i = 0; i < args.size(); ++i)
	cmdString.append(args[i]).append(" ");

    try
        {
            while(true) {
                try {
                    serverPort->sendMessage(cmdString);
                } catch(MMCSConsolePort::Error &e) {
                    switch (e.errcode)
                        {
                        case EINTR:
                        case EAGAIN:
                            sleep(1);
                            continue;
                        default:
                            // "replyformat" comes out of the connect command.
                            if(cmdString.find("replyformat") == std::string::npos)
                                reconnect_to_server(reply, pController);
                            else {
                                reply << FAIL << "mmcs_server aborted connection" << DONE;
                                return;
                            }
                            continue;
                        }
                }
                break;
            }

            // loop receiving command output until end of reply is received
            string replyString;
            bool   nullTerm = false;
            bool   eom = false;
            reply.reset();

            do
                {
                    bool server_failed = false;
                    int timeout = 0;
                    if(pController->getReplyFormat() == 0)
                        timeout = 15;  // Needs to be long enough to give aborted message time to return
                    try {
                        nullTerm = serverPort->receiveMessage(replyString,timeout); // nullTerm == true if '\0' received
                    }
                    catch(MMCSConsolePort::Error &e) {
                        switch (e.errcode)
                            {
                            case EINTR:
                            case EAGAIN:
                                continue;
                            default:
                                server_failed = true;
                                if(cmdString.find("replyformat") == std::string::npos &&
                                   cmdString.find("redirect") == std::string::npos)
                                    reconnect_to_server(reply, pController);
                                else {
                                    reply << FAIL << "mmcs_server aborted connection" << DONE;
                                }
                            }
                    }
                    if (replyString.length() == 0) {
                        if(!server_failed)
                            {
                                reply << ABORT << "internal error: null reply from server" << DONE;
                                return;
                            }
                        else {
                            reply << FAIL << "Server failed.  Reconnect attempted.  Retry command." << DONE;
                            return;
                        }
                    }

                    // reconstruct the MMCSCommandReply from the server response
                    if (reply.getStatus() == MMCSCommandReply::STATUS_NOT_SET) {
                        reply.assign(replyString);
                    }
                    else
                        reply.append(replyString);
                    replyString.clear();
                    // exit loop if
                    // replyformat 0 and a single message was received, or
                    // replyformat 1 and a null terminator was received, or
                    eom = (pController->getReplyFormat() == 0 || nullTerm);
                }
            while (!eom);

            if (!reply.isDone())
                reply << DONE;
        }

    catch(exception &e)
    {
	reply << ABORT << e.what() << DONE;
    }
}

bool MMCSCommand_mmcs_server_cmd::checkArgs(std::deque<std::string>& args) {
    if(args.size() == 0) return false; else return true;
}

void
MMCSCommand_mmcs_server_cmd::help(deque<string> args,
				  MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";Send a command to the mmcs server for execution and wait for the reply"
	  << ";This command is used internally by mmcs_console"
	  << DONE;
}
