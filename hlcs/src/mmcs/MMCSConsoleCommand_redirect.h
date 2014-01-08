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
 * \file MMCSConsoleCommand_redirect.h
 */

#ifndef _MMCSCONSOLECOMMAND_REDIRECT_H
#define _MMCSCONSOLECOMMAND_REDIRECT_H

#include "MMCSCommand.h"

/*!
** redirect <blockId> on|off
** Redirect I/O node output for the specified block to the mmcs console.
** Directs subsequent mailbox output back to the socket connection that this command is received on.
** Allocating or freeing the block will stop the mailbox redirection.
*/
class MMCSConsoleCommand_redirect: public MMCSCommand
{
public:
    MMCSConsoleCommand_redirect(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "redirect <blockId> [on|off] [stdout|stderr]"; }
    static  MMCSConsoleCommand_redirect* build();	// factory method
    static  std::string cmdname() { return "redirect"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
** redirect_block on|off
** Redirect I/O node output for the selected block to the mmcs console.
** Directs subsequent mailbox output back to the socket connection that this command is received on.
** Allocating or freeing the block will stop the mailbox redirection.
*/
class MMCSConsoleCommand_redirect_block: public MMCSCommand
{
public:
    MMCSConsoleCommand_redirect_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "redirect_block on|off [stdout|stderr]"; }
    static  MMCSConsoleCommand_redirect_block* build();	// factory method
    static  std::string cmdname() { return "redirect_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
