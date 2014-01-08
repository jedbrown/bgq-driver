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
 * \file MMCSCommand_help.h
 */

#ifndef _MMCSCOMMAND_HELP_H
#define _MMCSCOMMAND_HELP_H

#include "MMCSCommand.h"

/*!
** help
** help [<command-name>]
** Print command summaries for all commands, or extended help for a specific command
*/
class MMCSCommand_help: public MMCSCommand
{
public:
    MMCSCommand_help(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "help [<command-name>|<category>]";}
    static  MMCSCommand_help* build();	// factory method
    static  std::string cmdname() { return "help"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 2) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
