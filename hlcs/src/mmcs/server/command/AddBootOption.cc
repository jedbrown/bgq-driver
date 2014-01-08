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

#include "AddBootOption.h"

#include <db/include/api/BGQDBlib.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

AddBootOption*
AddBootOption::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new AddBootOption("add_boot_option", "add_boot_option <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoption>", commandAttributes);
}

void
AddBootOption::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
AddBootOption::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    BGQDB::STATUS result;

    if (args.size() != 2) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    if (args[1].size() > BGQDB::BlockInfo::OptionsSize) {
        reply << mmcs_client::FAIL << "Boot options too long" << mmcs_client::DONE;
        return;
    }

    result = BGQDB::addBootOption(args[0],  args[1]);
    switch (result) {
        case BGQDB::OK:
            reply << mmcs_client::OK << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << mmcs_client::FAIL << "Invalid block id " << args[0] << mmcs_client::DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << mmcs_client::FAIL << "Database failure" << mmcs_client::DONE;
            break;
        case BGQDB::NOT_FOUND:
            reply << mmcs_client::FAIL << "Block " << args[0] << " not found or not in valid status" << mmcs_client::DONE;
            break;
        case BGQDB::FAILED:
            reply << mmcs_client::FAIL << "Invalid block status" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << mmcs_client::FAIL << "Invalid arguments provided" << mmcs_client::DONE;
            break;
        default:
            reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::addBootOption : " << result << mmcs_client::DONE;
            break;
    }
}

void
AddBootOption::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Add boot option for a block, all I/O blocks, all compute blocks, or all blocks."
          << ";To add multiple boot options, separate by commas, with no spaces."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
