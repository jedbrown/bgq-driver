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
 * \file MMCSConsoleCommand_replyformat.h
 * \brief  This command sets the reply format for commands received on a particular DBConsoleController session.
 * See MMCSCommandReply.h for the different reply formats
 */

#ifndef _MMCSCONSOLECOMMAND_REPLYFORMAT_H
#define _MMCSCONSOLECOMMAND_REPLYFORMAT_H

#include "MMCSCommand.h"

// NOTE: Do NOT remove this command.  It is used by diags over its
// socket connection.  You can no-op it, but don't remove it without
// working with diags.
class MMCSConsoleCommand_replyformat: public MMCSCommand
{
public:
    MMCSConsoleCommand_replyformat(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "replyformat [0|1]"; }
    static  MMCSConsoleCommand_replyformat* build();	// factory method
    static  std::string cmdname() { return "replyformat"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(deque<string>& args);
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
