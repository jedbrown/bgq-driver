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

#ifndef MMCS_SERVER_COMMAND_GEN_BLOCKS_H_
#define MMCS_SERVER_COMMAND_GEN_BLOCKS_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** gen_blocks [<blockidprefix>]
** Generate a block for each midplane on a machine.
** The <blockidprefix> is optional.
** If omitted, each block name will be the position of the midplane, i.e. Rxx-Mx.
*/
class GenBlocks : public common::AbstractCommand
{
public:
    GenBlocks (const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "gen_blocks [<blockidprefix>]";}
    static  GenBlocks* build();    // factory method
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
    bool checkArgs(std::deque<std::string>& args) { if((args.size() != 0) && (args.size() != 1)) return false; else return true;  }
};

} } } // namespace mmcs::server::command

#endif
