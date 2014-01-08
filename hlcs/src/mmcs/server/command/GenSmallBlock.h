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

#ifndef MMCS_SERVER_COMMAND_GEN_SMALL_BLOCK_H_
#define MMCS_SERVER_COMMAND_GEN_SMALL_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*
** gen_small_block <blockid> <midplane> <cnodes> <nodecard>
** Generate a sub-midplane block.
** Specify <midplane> by location, i.e. Rxx-Mx.
** The <cnodes> is the total number of compute nodes and must be 32, 64, 128 or 256.
** The <nodecard> is the starting location of the compute nodes for the block, i.e. N00.
*/
class GenSmallBlock : public common::AbstractCommand
{
public:
    GenSmallBlock (const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "gen_small_block <blockid> <midplane> <cnodes> <nodecard>";}
    static  GenSmallBlock* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 4) return false; else return true;  }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
