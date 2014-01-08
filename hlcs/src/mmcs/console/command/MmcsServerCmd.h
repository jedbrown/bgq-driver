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
 * \file MmcsServerCmd.h
 */

#ifndef MMCS_CONSOLE_COMMAND_MMCS_SERVER_CMD_H_
#define MMCS_CONSOLE_COMMAND_MMCS_SERVER_CMD_H_

#include "common/AbstractCommand.h"

#include <bgq_util/include/pthreadmutex.h>

namespace mmcs {
namespace console {
namespace command {

/*!
** mmcs_server_cmd <command-string>
** Send a command to the mmcs server for execution and wait for the reply
** This command is used internally by bg_console
** Note: an active server port must be set before this command is executed
*/
class MmcsServerCmd : public common::AbstractCommand
{
    static PthreadMutex _reconnect_lock;
    static bool _ending;
public:
    MmcsServerCmd(const char* name, const char* description, const Attributes& attributes)
        : common::AbstractCommand(name,description,attributes) { _usage = "mmcs_server_cmd [<anything>]";}
    static  MmcsServerCmd* build();	// factory method
    void execute(std::deque<std::string> args,
			 mmcs_client::CommandReply& reply,
			 common::ConsoleController* pController,
			 server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args);
    void help(std::deque<std::string> args,
		      mmcs_client::CommandReply& reply);
    static void reconnect_to_server(mmcs_client::CommandReply& reply,
            common::ConsoleController* pController);
    static bool ending(bool ending) { _ending = ending; return _ending; }
    static bool ending() { return _ending; }
};

} } } // namespace mmcs::console::command

#endif
