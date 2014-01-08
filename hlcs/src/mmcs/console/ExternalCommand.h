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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef MMCS_CONSOLE_EXTERNAL_COMMAND_H
#define MMCS_CONSOLE_EXTERNAL_COMMAND_H

#include "common/AbstractCommand.h"

namespace mmcs {
namespace console {

class ExternalCommand : public common::AbstractCommand
{
public:
    ExternalCommand(const char* name, const char* description, const Attributes& attributes)
        : common::AbstractCommand(name,description,attributes) { /*? usage set ?  */}
    static ExternalCommand* build(const std::string& name, const std::string& description);
    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget* pTarget=NULL
            );
    bool checkArgs(std::deque<std::string>& /*args*/) { return true; }
    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );

private:
    void runcmd(mmcs_client::CommandReply& reply, const std::string& cmd_and_args, const std::string& executable);
};

} } // namespace mmcs::console

#endif
