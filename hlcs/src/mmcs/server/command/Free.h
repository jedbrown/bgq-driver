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

#ifndef MMCS_SERVER_COMMAND_FREE_H_
#define MMCS_SERVER_COMMAND_FREE_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** Release a specified block.  Drops the mc_server connections and marks a block as free in the BGQBLOCK table.
*/
class Free: public common::AbstractCommand
{
public:
    Free(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "free <blockid>";}
    static Free* build();  // factory method
    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            DBConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );
    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            DBConsoleController* pController,
            BlockControllerTarget* pTarget,
            std::vector<std::string>* validnames
            );
    bool checkArgs(std::deque<std::string>& args) { 
        if ( args.empty() ) return false;
        if ( args.size() > 2 ) return false;
        return true;
    }

    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );
};

} } } // namespace mmcs::server::command

#endif
