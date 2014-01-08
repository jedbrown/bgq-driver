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

#ifndef MMCS_SERVER_COMMAND_REDIRECT_H_
#define MMCS_SERVER_COMMAND_REDIRECT_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** redirect <blockid> on|off
** Redirect I/O node output for the specified block to the mmcs console.
** Directs subsequent mailbox output back to the socket connection that this command is received on.
** Allocating or freeing the block will stop the mailbox redirection.
*/
class Redirect : public common::AbstractCommand
{
public:

    static void redirect_block(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            DBConsoleController* pController
        );

    Redirect(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "redirect <blockid> on|off";}
    static Redirect* build();  // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 2) return false; else return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
