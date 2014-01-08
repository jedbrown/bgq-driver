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
 * \file MMCSConsoleCommand_username.h
 * Set the username
 */

#ifndef _MMCSCONSOLECOMMAND_USERNAME_H
#define _MMCSCONSOLECOMMAND_USERNAME_H

#include "MMCSCommand.h"

/*!
** username <username> - returns the username
*/
class MMCSConsoleCommand_username: public MMCSCommand
{
public:
    MMCSConsoleCommand_username(const char* name, const char* description, const MMCSCommandAttributes& attributes)
	: MMCSCommand(name,description,attributes) {}
    static  MMCSConsoleCommand_username* build();	// factory method
    static  std::string cmdname() { return "username"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 1) return false; else return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
