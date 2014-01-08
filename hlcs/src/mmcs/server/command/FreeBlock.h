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

#ifndef MMCS_SERVER_COMMAND_FREE_BLOCK_H_
#define MMCS_SERVER_COMMAND_FREE_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** Release a selected block.  An alternate form of free that operates on the currently selected block.
*/
class FreeBlock: public common::AbstractCommand
{
public:
    FreeBlock(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "free_block";}
    static FreeBlock* build();    // factory method
    void execute(
            std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             DBConsoleController* pController,
             BlockControllerTarget* pTarget=NULL
             );
    bool checkArgs(std::deque<std::string>& args) { 
        if ( args.empty() ) return true;
        if ( args.size() == 1 ) return true;
        return false;
    }

    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );
};

} } } // namespace mmcs::server::command

#endif
