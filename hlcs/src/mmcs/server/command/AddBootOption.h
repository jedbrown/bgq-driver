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

#ifndef MMCS_SERVER_COMMAND_ADD_BOOT_OPTION_H_
#define MMCS_SERVER_COMMAND_ADD_BOOT_OPTION_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*
** add_boot_option <blockid|ALL> <bootoptions>
** Add a boot option for a block, or for all blocks.
*/
class AddBootOption : public common::AbstractCommand
{
public:
    AddBootOption (const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "add_boot_option <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoption>";}
    static  AddBootOption* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 2) return false; else return true;}
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
