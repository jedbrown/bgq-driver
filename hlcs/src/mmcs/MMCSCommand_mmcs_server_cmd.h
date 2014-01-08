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
 * \file MMCSCommand_mmcs_server_cmd.h
 */

#ifndef _MMCSCOMMAND_MMCS_SERVER_CMD_H
#define _MMCSCOMMAND_MMCS_SERVER_CMD_H

#include "MMCSCommand.h"

/*!
** mmcs_server_cmd <command-string>
** Send a command to the mmcs server for execution and wait for the reply
** This command is used internally by mmcs_console
** Note: an active server port must be set before this command is executed
*/
class MMCSCommand_mmcs_server_cmd: public MMCSCommand
{
    static PthreadMutex _reconnect_lock;
    static bool _ending;
public:
    MMCSCommand_mmcs_server_cmd(const char* name, const char* description, const MMCSCommandAttributes& attributes)
        : MMCSCommand(name,description,attributes) { usage = "mmcs_server_cmd [<anything>]";}
    static  MMCSCommand_mmcs_server_cmd* build();	// factory method
    static  std::string cmdname() { return "mmcs_server_cmd"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
    static void reconnect_to_server(MMCSCommandReply& reply,
                             ConsoleController* pController);
    static bool ending(bool ending) { _ending = ending; return _ending; }
    static bool ending() { return _ending; }
};

#endif
