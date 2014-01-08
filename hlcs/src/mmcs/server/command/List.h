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

#ifndef MMCS_SERVER_COMMAND_LIST_H_
#define MMCS_SERVER_COMMAND_LIST_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
 ** list <db2table> [<value>]
 ** Print the contents of <db2table>.
 ** Optional <value> specifies contents of a field within that table.
 ** Following are the only valid table / field combinations that can be used.
 ** bgqnode / location
 ** bgqnodecard / location
 ** bgqprocessorcard / location
 ** bgqmidplane / location
 ** bgqmachine / alias
 ** bgqblock / blockid (wild card with %)
 ** bgqproducttype / productid
 ** bgqeventlog / block
 */
class List : public common::AbstractCommand
{
public:
    List(const char* name, const char* description, const Attributes& attributes)
            : AbstractCommand(name,description,attributes) { _usage = "list <db2table> [<value>]"; }
    static  List* build();    // factory method
    void execute(std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() < 1) return false; else return true; }
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
