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

#ifndef MMCS_SERVER_COMMAND_WRITE_CON_H_
#define MMCS_SERVER_COMMAND_WRITE_CON_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** write_con
** [<target>] write_con <console-command>
** Send <console-command> to target node for execution.
** Output will be returned to mailbox (either console or I/O node log).
*/
class WriteCon: public common::AbstractCommand
{
public:
    WriteCon(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "[<target>] write_con <console-command>"; }
    static WriteCon* build(); // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
