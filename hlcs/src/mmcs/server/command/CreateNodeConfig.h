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

#ifndef MMCS_SERVER_COMMAND_CREATE_NODE_CONFIG_H_
#define MMCS_SERVER_COMMAND_CREATE_NODE_CONFIG_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*
** create_node_config
** Create a node configuration for use in booting a block
*/
class CreateNodeConfig : public common::AbstractCommand
{
public:
    CreateNodeConfig (const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "create_node_config <configname> [<raspolicy>] [<nodeoptions>] <domaininfo>";}
    static  CreateNodeConfig* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget);
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
    bool checkArgs(std::deque<std::string>& args) { return true; }
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController) {
        std::vector<std::string> empty_vec;
        return empty_vec;
    }
};

} } } // namespace mmcs::server::command

#endif
