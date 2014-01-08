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

#ifndef MMCS_SERVER_COMMAND_ALLOCATE_H_
#define MMCS_SERVER_COMMAND_ALLOCATE_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** For specified <blockId>, performs select_block, allocate_block, boot_block and wait_boot.
** <blockId> identifies a block in the BGQBLOCK table.
** This command can be used to initialize a block in free state.
*/
class Allocate: public common::AbstractCommand
{
public:
    Allocate(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "allocate <blockId> [ options ] "; }
    static  Allocate* build();  // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
                         BlockControllerTarget* pTarget,
                         std::vector<std::string>* validnames);
    // Allow just blockid or blockid and hidden argument "no_check" used for I/O blocks
    bool checkArgs(std::deque<std::string>& args) { if (args.size() == 0 || args.size() > 2) return false; else return true;    }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
private:
    void recoverL3Major(std::vector<std::string>& failingNodes,
            std::deque<std::string> allocateArgs,
            mmcs_client::CommandReply& reply,
            DBConsoleController* pController);
};

} } } // namespace mmcs::server::command

#endif
