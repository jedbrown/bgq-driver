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

#ifndef MMCS_SERVER_COMMAND_COPY_BLOCK_H_
#define MMCS_SERVER_COMMAND_COPY_BLOCK_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*
** copy_block <existingblockid> <newblockid>
** Copy a block
*/
class CopyBlock : public common::AbstractCommand
{
public:
    CopyBlock (const char* name, const char* description, const Attributes& attributes)
    : AbstractCommand(name,description,attributes) { _usage = "copy_block <existingblockid> <newblockid>";}
    static  CopyBlock* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget,
             std::vector<std::string>* validnames);
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             BlockControllerTarget* pTarget);
    std::vector<std::string> getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController);
    bool doSpecialAuths(std::vector<std::string>& blocks, const boost::shared_ptr<hlcs::security::Enforcer>& enforcer, procstat& stat, const bgq::utility::UserId& user);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() != 2) return false; else return true;}
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
