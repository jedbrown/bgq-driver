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
 * \file MMCSCommand_mmcs_server_connect.h
 */

#ifndef _MMCSCOMMAND_MMCS_SERVER_CONNECT_H
#define _MMCSCOMMAND_MMCS_SERVER_CONNECT_H

#include "MMCSCommand.h"

/*!
** mmcs_server_connect [<retry>]
** Establish a tcp connection to the mmcs server.
** @param retry		retry until the connection is established or a SIGINT is received
** Once established, the username and replyformat are sent to the server
** A pointer to the MMCSConsolePort object is saved in the ConsoleController object
** The server ip address and port are taken from the MMCSProperties object
** This command is used internally by mmcs_console
*/
class MMCSCommand_mmcs_server_connect: public MMCSCommand
{
public:
    MMCSCommand_mmcs_server_connect(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "mmcs_server_connect [<retry>]";}
    static  MMCSCommand_mmcs_server_connect* build();	// factory method
    static  std::string cmdname() { return "mmcs_server_connect"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) {
        if(args.size() > 1) return false; else return true;
    }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);

private:
    void raiseUid() const;
};

#endif
