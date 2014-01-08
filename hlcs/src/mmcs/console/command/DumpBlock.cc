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


#include "DumpBlock.h"

#include "DumpMachine.h"

#include "../../MMCSCommandProcessor.h"


using namespace std;


namespace mmcs {
namespace console {
namespace command {


/*
** dump_block <file.xml> <blockId>
** Export a block <blockId> from the database into <file.xml>
*/
DumpBlock*
DumpBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.mmcsServerCommand(false);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::ADMIN);
    return new DumpBlock("dump_block", "dump_block <file.xml> <blockId>", commandAttributes);
}

void
DumpBlock::execute(deque<string> args,
                    mmcs_client::CommandReply& reply,
                    common::ConsoleController* pController,
                    server::BlockControllerTarget* /*pTarget*/)
{
    deque<string> mmcs_dump_block = MMCSCommandProcessor::parseCommand("mmcs_server_cmd dump_block_server " + args[1]);
    DumpMachine::sendCommand(mmcs_dump_block, args, reply, pController);
}

std::vector<std::string>
DumpBlock::getBlockObjects(std::deque<std::string>& cmdString, server::DBConsoleController* /*pController*/) {
    std::vector<std::string> retvec;
    retvec.push_back(cmdString[1]);
    return retvec;
}

void
DumpBlock::help(deque<string> ,//args,
                 mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
      << ";Export a block <blockId> from the database into <file.xml>"
      << mmcs_client::DONE;
}


} } } // namespace mmcs::console::command
