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
 * \file MMCSConsoleCommand_replyformat.cc
 * \brief This command sets the reply format for commands received on a particular ConsoleController session.
 * See MMCSCommandReply.h for the different reply formats
 */

#include "MMCSConsoleCommand_replyformat.h"
#include "MMCSCommandReply.h"
#include "MMCSCommandProcessor.h"
#include "ConsoleController.h"

using namespace std;

/*!
** build() - MMCSCommand factory
** This is invoked at MMCS startup when MMCS builds its list of commands
** @return an MMCSCommand object for this specific command
*/
MMCSConsoleCommand_replyformat*
MMCSConsoleCommand_replyformat::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(ADMIN);
    return new MMCSConsoleCommand_replyformat("replyformat", "replyformat [0|1]", commandAttributes);
}

/*!
** execute() - Perform specific MMCS command
** @param args the command arguments
** @param reply       the command output stream. Refer to class MMCSCommandReply
** @param pController the ConsoleController object that the command is to work on
*/
void
MMCSConsoleCommand_replyformat::execute(deque<string> args,
					MMCSCommandReply& reply,
					ConsoleController* pController,
					BlockControllerTarget* pTarget)
{
    if (args.size() == 0)
	reply << OK << pController->getReplyFormat() << DONE;
    else
    {
	if (args[0].compare("0") == 0 || args[0].compare("1") == 0)
	{
	    unsigned replyformat = atoi(args[0].c_str());
	    if (pController->getConsolePort() != NULL) // are we connected to the server?
	    {
		std::deque<string> cmdStr;
		cmdStr.push_back("replyformat");
		cmdStr.push_back(args[0]);
		pController->getCommandProcessor()->execute("mmcs_server_cmd", cmdStr, reply, pController);
		if (reply.getStatus() == 0)
		    pController->setReplyFormat(replyformat);
	    }
	    else
	    {
		pController->setReplyFormat(replyformat);
		reply << OK << DONE;	// current reply is sent in the old format
	    }
	}
	else
	    reply << FAIL << "args? " << usage <<  DONE;
    }
}

bool MMCSConsoleCommand_replyformat::checkArgs(deque<string>& args) {
    if(args.size() == 0 || args.size() == 1) return true; else return false;
}

/*!
** help() - Print extended command help to the reply stream
** @param args  the help command arguments
** @param reply the command output stream. Refer to class MMCSCommandReply
*/
void
MMCSConsoleCommand_replyformat::help(deque<string> args,
				     MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";With no parameters, displays the current reply format"
	  << ";0 means that each reply is returned as a single text line, embedded newlines are indicated by a semicolon and then end of the reply is indicated by a newline"
	  << ";1 means that each reply is returned as one or more text lines, each line terminated by a newline. The end of the reply is indicated by a '\0'"
	  << DONE;

}
