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

#ifndef MMCS_SERVER_COMMAND_GET_BLOCK_SIZE_H_
#define MMCS_SERVER_COMMAND_GET_BLOCK_SIZE_H_

#include "common/AbstractCommand.h"

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

namespace mmcs {
namespace server {
namespace command {

/*!
** get_block_size
** get_block_size [ a | b | c | | d | e | * ]
** Retrieve the block dimensions of selected block.
** Specifiying a, b, c, d, or e retrieves that one dimension.
** Specifying * retrieves all 5 dimensions.
*/
class GetBlockSize: public common::AbstractCommand
{
public:
    GetBlockSize(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "get_block_size [ a | b | c | d | e | * ]";}
    static  GetBlockSize* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 1) return false; else return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
private:
    void compute(
            const std::deque<std::string>& args,
            const BGQBlockNodeConfig* block,
            mmcs_client::CommandReply& reply
    );

    void io(
            const std::deque<std::string>& args,
            const BGQBlockNodeConfig* block,
            mmcs_client::CommandReply& reply
    );
};

} } } // namespace mmcs::server::command

#endif
