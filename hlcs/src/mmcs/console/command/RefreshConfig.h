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

#ifndef MMCS_CONSOLE_COMMAND_REFRESH_CONFIG_H_
#define MMCS_CONSOLE_COMMAND_REFRESH_CONFIG_H_

#include "common/AbstractCommand.h"

#include <vector>

namespace mmcs {
namespace console {
namespace command {

class RefreshConfig : public common::AbstractCommand
{
public:
    RefreshConfig(
            const char* name,
            const char* description,
            const Attributes& attributes
            );

    static  RefreshConfig* build();

    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* controller,
            server::BlockControllerTarget* target=NULL
            );
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 2) return false; else return true;}
    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );

private:
    void refresh(
            const std::string& server,
            const std::string& file,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* controller
            );

private:
    typedef std::vector<std::string> ServerContainer;
    ServerContainer _servers;
};

} } } // namespace mmcs::console::command

#endif
