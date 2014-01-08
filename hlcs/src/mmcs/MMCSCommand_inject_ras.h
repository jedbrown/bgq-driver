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
 * \file MMCSCommand_inject_ras.h
 */

#ifndef _MMCSCOMMAND_INJECT_RAS_H
#define _MMCSCOMMAND_INJECT_RAS_H

#include "MMCSCommand.h"

// By default, we do not build this command.  It's
// for debug and doesn't have the validation
// mechanisms of production code.  To build it, it
// needs to be added back to MMCSCommandProcessor.cc's
// list and Makefile's DB targets where it is commented
// now.
class MMCSCommand_inject_ras: public MMCSCommand
{
public:
    MMCSCommand_inject_ras(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "inject_ras";}
    static  MMCSCommand_inject_ras* build();	// factory method
    static  std::string cmdname() { return "inject_ras"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

class MMCSCommand_inject_console: public MMCSCommand
{
public:
    MMCSCommand_inject_console(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "inject_console";}
    static  MMCSCommand_inject_console* build();	// factory method
    static  std::string cmdname() { return "inject_console"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
