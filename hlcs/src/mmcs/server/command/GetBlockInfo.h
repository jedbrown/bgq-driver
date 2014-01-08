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

#ifndef MMCS_SERVER_COMMAND_GET_BLOCK_INFO_H_
#define MMCS_SERVER_COMMAND_GET_BLOCK_INFO_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
 ** get_block_info [<blockId>]
 ** Prints boot information, boot mode and status for specified block.
 */
class GetBlockInfo : public common::AbstractCommand
{
    public:
    GetBlockInfo(const char* name, const char* description, const Attributes& attributes)
            : AbstractCommand(name,description,attributes) { _usage = "get_block_info <blockId>"; }
        static  GetBlockInfo* build();    // factory method
        void execute(std::deque<std::string> args,
                mmcs_client::CommandReply& reply,
                common::ConsoleController* pController,
                BlockControllerTarget* pTarget=NULL,
                std::vector<std::string>* validnames = 0
                );
        void execute(std::deque<std::string> args,
                mmcs_client::CommandReply& reply,
                common::ConsoleController* pController,
                BlockControllerTarget* pTarget=NULL
                );
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
