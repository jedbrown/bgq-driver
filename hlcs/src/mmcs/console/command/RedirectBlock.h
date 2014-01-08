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

#ifndef MMCS_CONSOLE_COMMAND_REDIRECT_BLOCK_H_
#define MMCS_CONSOLE_COMMAND_REDIRECT_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace console {
namespace command {

/*!
** redirect_block on|off
** Redirect I/O node output for the selected block to the bg_console.
** Directs subsequent mailbox output back to the socket connection that this command is received on.
** Allocating or freeing the block will stop the mailbox redirection.
*/
class RedirectBlock : public common::AbstractCommand
{
public:
    RedirectBlock(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "redirect_block on|off [stdout|stderr]"; }
    static  RedirectBlock* build(); // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& /*args*/) { return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::console::command

#endif
