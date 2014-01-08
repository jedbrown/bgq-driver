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

#ifndef MMCS_SERVER_COMMAND_WAIT_BOOT_H_
#define MMCS_SERVER_COMMAND_WAIT_BOOT_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** wait_boot
** Wait for the selected block to complete booting.
** The command does not complete until the block is fully initialized to the point that mmcs can communicate with ciod on all of the I/O nodes.
*/
class WaitBoot: public common::AbstractCommand
{
public:
    WaitBoot(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "wait_boot [<minutes>]";}
    static WaitBoot* build(); // factory method
    void execute(std::deque<std::string> args,
         mmcs_client::CommandReply& reply,
         DBConsoleController* pController,
         BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 1) return false; else return true;}
    void help(std::deque<std::string> args,
          mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
