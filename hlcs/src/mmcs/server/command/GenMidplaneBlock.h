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

#ifndef MMCS_SERVER_COMMAND_GEN_MIDPLANE_BLOCK_H_
#define MMCS_SERVER_COMMAND_GEN_MIDPLANE_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** gen_midplane_block <blockid> <corner> <asize> <bsize> <csize> <dsize> [aPT bPT cPT dPT]
** Generate a block for a set of midplanes.

** For <corner> specify midplane by location, i.e. Rxx-Mx.
** The size is provided in terms of number of midplanes in A, B, C and D dimensions.
** The <corner> will be in the 0,0,0,0 position of the midplanes that make up the block.
** Except for the <corner>, the midplanes included in the generated block
** depend on A,B,C,D cabling of your machine.
*/
class GenMidplaneBlock : public common::AbstractCommand
{
public:
    GenMidplaneBlock (const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "gen_midplane_block <blockid> <corner> <asize> <bsize> <csize> <dsize> [aPT bPT cPT dPT]";}
    static  GenMidplaneBlock* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if ((args.size() != 6) && (args.size() != 10)) return false; else return true;}
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
