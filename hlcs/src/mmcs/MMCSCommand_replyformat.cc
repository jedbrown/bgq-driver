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
 * \file MMCSCommand_replyformat.cc
 * \brief This command sets the reply format for commands received on a particular DBConsoleController session.
 * See MMCSCommandReply.h for the different reply formats
 */

#include "ConsoleController.h"
#include "MMCSCommand_replyformat.h"
#include "MMCSCommandReply.h"

using namespace std;

/*!
** build() - MMCSCommand factory
** This is invoked at MMCS startup when MMCS builds its list of commands
** @return an MMCSCommand object for this specific command
*/
MMCSCommand_replyformat*
MMCSCommand_replyformat::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(SPECIAL);
    return new MMCSCommand_replyformat("replyformat", "replyformat [0|1]", commandAttributes);
}

/*!
** execute() - Perform specific MMCS command
** @param args the command arguments
** @param reply       the command output stream. Refer to class MMCSCommandReply
** @param pController the ConsoleController object that the command is to work on
*/
void
MMCSCommand_replyformat::execute(deque<string> args,
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
	    pController->setReplyFormat(replyformat);
	    reply << OK << DONE;	// current reply is sent in the old format
	}
	else
	    reply << FAIL << "args? " << usage <<  DONE;
    }
}

bool MMCSCommand_replyformat::checkArgs(deque<string>& args) {
    if(args.size() == 0 || args.size() == 1) return true; else return false;
}

/*!
** help() - Print extended command help to the reply stream
** @param args  the help command arguments
** @param reply the command output stream. Refer to class MMCSCommandReply
*/
void
MMCSCommand_replyformat::help(deque<string> args,
			      MMCSCommandReply& reply)
{
    reply << OK << description()
	  << ";With no parameters, displays the current reply format"
	  << ";0 means that each reply is returned as a single text line, embedded newlines are indicated by a semicolon and then end of the reply is indicated by a newline"
	  << ";1 means that each reply is returned as one or more text lines, each line terminated by a newline. The end of the reply is indicated by a '\0'"
	  << DONE;

}
