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

#ifndef MMCS_SERVER_COMMAND_DUMP_BLOCK_H_
#define MMCS_SERVER_COMMAND_DUMP_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*
** dump_block <file.xml> <blockId>
** Export a block <blockId> from the database into <file.xml>
*/
class DumpBlock : public common::AbstractCommand
{
public:
    DumpBlock(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "dump_block <file.xml> <blockId>"; }
    static  DumpBlock* build();    // factory method
    void execute(std::deque<std::string> args,
                 mmcs_client::CommandReply& reply,
                 common::ConsoleController* pController,
                 BlockControllerTarget* pTarget);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    virtual std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true;}
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
