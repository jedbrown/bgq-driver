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
 * \file MMCSCommand_reboot.h
 */

#ifndef MMCS_SERVER_COMMAND_REBOOT_NODES_H_
#define MMCS_SERVER_COMMAND_REBOOT_NODES_H_


#include "common/AbstractCommand.h"


namespace mmcs {
namespace server {
namespace command {


class RebootNodes : public common::AbstractCommand
{
public:
    RebootNodes(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { usage = "[<target>] reboot_nodes";}
    static  RebootNodes* build();
    static  std::string cmdname() { return "reboot_nodes"; }

    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            DBConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );

    bool checkArgs(std::deque<std::string>& args) { return true; }

    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );
};

} } } // namespace mmcs::server::command

#endif
