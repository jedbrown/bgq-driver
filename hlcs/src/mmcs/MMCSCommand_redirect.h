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
 * \file MMCSCommand_redirect.h
 */

#ifndef _MMCSCOMMAND_REDIRECT_H
#define _MMCSCOMMAND_REDIRECT_H

#include "MMCSCommand.h"

/*!
** redirect <blockid> on|off
** Redirect I/O node output for the specified block to the mmcs console.
** Directs subsequent mailbox output back to the socket connection that this command is received on.
** Allocating or freeing the block will stop the mailbox redirection.
*/
class MMCSCommand_redirect: public MMCSCommand
{
public:
    MMCSCommand_redirect(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "redirect <blockid> on|off";}
    static  MMCSCommand_redirect* build();	// factory method
    static  std::string cmdname() { return "redirect"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 2) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

/*!
** redirect_block on|off
** Redirect I/O node output for the selected block to the mmcs console.
** Directs subsequent mailbox output back to the socket connection that this command is received on.
** Allocating or freeing the block will stop the mailbox redirection.
*/
class MMCSCommand_redirect_block: public MMCSCommand
{
public:
    MMCSCommand_redirect_block(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "redirect_block on|off";}
    static  MMCSCommand_redirect_block* build();	// factory method
    static  std::string cmdname() { return "redirect_block"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
