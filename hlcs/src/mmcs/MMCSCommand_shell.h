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
 * \file MMCSCommand_shell.h
 */

#ifndef _MMCSCOMMAND_SHELL_H
#define _MMCSCOMMAND_SHELL_H

#include "MMCSCommand.h"

/*!
** #
** # [<comment>]
** Indicates a comment line
*/
class MMCSCommand_comment: public MMCSCommand
{
public:
    MMCSCommand_comment(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "# [<comment>]";}
    static  MMCSCommand_comment* build();    // factory method
    static  std::string cmdname() { return "#"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>&) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** quit
** quit
** Ends mmcs_hw. The block is implicitly destroyed and the ido connections are dropped.
*/
class MMCSCommand_quit: public MMCSCommand
{
public:
    MMCSCommand_quit(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "quit";}
    static  MMCSCommand_quit* build();    // factory method
    static  std::string cmdname() { return "quit"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** end the mmcs console
** This command does not change the status of any allocated blocks
*/
class MMCSConsoleCommand_quit: public MMCSCommand
{
public:
    MMCSConsoleCommand_quit(const char* name, const char* description, const MMCSCommandAttributes& attributes)
    : MMCSCommand(name,description,attributes) {}
    static  MMCSConsoleCommand_quit* build();    // factory method
    static  std::string cmdname() { return "quit"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** sleep
** sleep <seconds>
** Pause for the specified number of seconds
*/
class MMCSCommand_sleep: public MMCSCommand
{
public:
    MMCSCommand_sleep(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "sleep <seconds>" ;}
    static  MMCSCommand_sleep* build();    // factory method
    static  std::string cmdname() { return "sleep"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** !
** ! [<shell-command>]
** Escapes you to a sub_shell if no <command> is specified.
** Use 'exit' to return to mmcs$ prompt.
** If optional <command> is specified, executes that command in a sub_shell and returns you to mmcs$ prompt.
*/
class MMCSCommand_sub_shell: public MMCSCommand
{
public:
    MMCSCommand_sub_shell(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "! [<shell-command>]"; }
    static  MMCSCommand_sub_shell* build();    // factory method
    static  std::string cmdname() { return "!"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>&) { return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

/*!
** <
** < <filename>
** Reads subsequent commands from <filename>.
** Returns to reading stdin when EOF is reached on <filename>
*/
class MMCSCommand_redirect_input: public MMCSCommand
{
public:
    MMCSCommand_redirect_input(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "< <filename>";}
    static  MMCSCommand_redirect_input* build();    // factory method
    static  std::string cmdname() { return "<"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() < 1) return false; return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
