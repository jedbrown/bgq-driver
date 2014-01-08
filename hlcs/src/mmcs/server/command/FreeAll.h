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

#ifndef MMCS_SERVER_COMMAND_FREE_ALL_H_
#define MMCS_SERVER_COMMAND_FREE_ALL_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

class FreeAll: public common::AbstractCommand
{
    std::vector<std::string> _blocks;
public:
    FreeAll(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "free_all"; }
    static  FreeAll* build();  // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
                         BlockControllerTarget* pTarget = 0);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 1) return false; else return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
};

} } } // namespace mmcs::server::command

#endif
