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

#ifndef MMCS_SERVER_COMMAND_BOOT_BLOCK_H_
#define MMCS_SERVER_COMMAND_BOOT_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** boot_block
** Boots a previously allocated block.
** If a block is in an error state, reset the block first using free_block and allocate_block.
*/
class BootBlock : public common::AbstractCommand
{
public:
    BootBlock(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "boot_block [update] [<options>]";}
    static  BootBlock* build();  // factory method
    void execute(std::deque<std::string> args,
         mmcs_client::CommandReply& reply,
         DBConsoleController* pController,
         BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true;}
    void help(std::deque<std::string> args,
          mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
