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

#ifndef MMCS_SERVER_COMMAND_LIST_USERS_H_
#define MMCS_SERVER_COMMAND_LIST_USERS_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
 ** list_users
 ** List mmcs users.  Output includes thread number, block ID and if output is redirected to console.
 */
class ListUsers : public common::AbstractCommand
{
public:
    ListUsers(const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "list_users";}
    static  ListUsers* build();    // factory method
    void execute(std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            DBConsoleController* pController,
            BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
            mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
