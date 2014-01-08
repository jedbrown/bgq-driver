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


#ifndef MMCS_COMMON_COMMAND_HELP_H_
#define MMCS_COMMON_COMMAND_HELP_H_

#include "../AbstractCommand.h"


namespace mmcs {
namespace common {
namespace command {


/*!
** help
** help [<command-name>]
** Print command summaries for all commands, or extended help for a specific command
*/
class Help : public AbstractCommand
{
public:
    Help(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "help [<command-name>|<category>]";}
    static  Help* build();	// factory method
    void execute(std::deque<std::string> args,
			 mmcs_client::CommandReply& reply,
			 ConsoleController* pController,
			 server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 2) return false; else return true;}
    void help(std::deque<std::string> args,
		      mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::common::command

#endif
