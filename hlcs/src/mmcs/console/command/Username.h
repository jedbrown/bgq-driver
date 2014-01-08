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

#ifndef MMCS_CONSOLE_COMMAND_USERNAME_H_
#define MMCS_CONSOLE_COMMAND_USERNAME_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace console {
namespace command {

/*!
** username <username> - returns the username
*/
class Username : public common::AbstractCommand
{
public:
    Username(const char* name, const char* description, const Attributes& attributes)
	: AbstractCommand(name,description,attributes) {}
    static  Username* build();	// factory method
    void execute(std::deque<std::string> args,
			 mmcs_client::CommandReply& reply,
			 common::ConsoleController* pController,
			 server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 1) return false; else return true;}
    void help(std::deque<std::string> args,
		      mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::console::command

#endif
