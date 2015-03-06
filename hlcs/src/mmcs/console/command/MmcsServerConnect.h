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
 * \file MmcsServerConect.h
 */

#ifndef MMCS_CONSOLE_COMMAND_MMCS_SERVER_CONNECT_H_
#define MMCS_CONSOLE_COMMAND_MMCS_SERVER_CONNECT_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace console {
namespace command {

/*!
** mmcs_server_connect [<retry>]
** Establish a TCP connection to the mmcs server.
** @param retry retry until the connection is established or a SIGINT is received
** Once established, the username and replyformat are sent to the server
** A pointer to the MMCSConsolePort object is saved in the common::ConsoleController object
** The server ip address and port are taken from the Properties object
** This command is used internally by bg_console
*/
class MmcsServerConnect : public common::AbstractCommand
{
public:
    MmcsServerConnect(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "mmcs_server_connect [<retry>]";}
    static  MmcsServerConnect* build(); // factory method
    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget* pTarget=NULL
            );
    bool checkArgs(std::deque<std::string>& args) {
        if (args.size() > 1) return false; else return true;
    }
    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );

private:
    void raiseUid() const;
};

} } } // namespace mmcs::console::command

#endif
