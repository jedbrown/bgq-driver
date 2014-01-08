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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MMCS_CONSOLE_COMMAND_SUB_SHELL_H_
#define MMCS_CONSOLE_COMMAND_SUB_SHELL_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace console {
namespace command {

/*!
** !
** ! [<shell-command>]
** Escapes you to a sub_shell if no <command> is specified.
** Use 'exit' to return to mmcs$ prompt.
** If optional <command> is specified, executes that command in a sub_shell and returns you to mmcs$ prompt.
*/
class SubShell : public common::AbstractCommand
{
public:
    SubShell(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "! [<shell-command>]"; }
    static  SubShell* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>&) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::console::command

#endif
