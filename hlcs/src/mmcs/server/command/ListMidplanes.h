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

#ifndef MMCS_SERVER_COMMAND_LIST_MIDPLANES_H_
#define MMCS_SERVER_COMMAND_LIST_MIDPLANES_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
 ** list_midplanes
 ** Prints midplane information, including blocks that are booted on that midplane.
 */
class ListMidplanes : public common::AbstractCommand
{
public:
    ListMidplanes(const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "list_midplanes"; }
    static  ListMidplanes* build();    // factory method
    void execute(std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
            mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
