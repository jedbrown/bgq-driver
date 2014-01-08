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
 * \file MMCSCommand_ionodes.h
 */

#ifndef _MMCSCOMMAND_IONODES_H
#define _MMCSCOMMAND_IONODES_H

#include "MMCSCommand.h"

/*!
** sysrq
** [<target>] sysrq [option]
** Sends a sysrq command to the I/O node
** Options:
** loglevel0-8 reBoot tErm Halt kIll showMem showPc unRaw Sync showTasks Unmount Xmon
*/

class MMCSCommand_sysrq: public MMCSCommand
{
public:
    MMCSCommand_sysrq(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] sysrq [options]";}
    static  MMCSCommand_sysrq* build();	// factory method
    static  std::string cmdname() { return "sysrq"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};


/*!
** write_con
** [<target>] write_con <console-command>
** Send <console-command> to target node for execution.
** Output will be returned to mailbox (either console or I/O node log).
*/
class MMCSCommand_write_con: public MMCSCommand
{
public:
    MMCSCommand_write_con(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] write_con <console-command>"; }
    static  MMCSCommand_write_con* build();	// factory method
    static  std::string cmdname() { return "write_con"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

class MMCSCommand_wc : public MMCSCommand_write_con
{
public:
    MMCSCommand_wc(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand_write_con(name,description,attributes) { }
    static  MMCSCommand_wc* build();
};

#endif
