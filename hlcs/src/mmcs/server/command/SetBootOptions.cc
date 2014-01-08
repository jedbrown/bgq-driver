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


#include "SetBootOptions.h"

#include <db/include/api/BGQDBlib.h>


using namespace std;


namespace mmcs {
namespace server {
namespace command {


SetBootOptions*
SetBootOptions::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockupdate(hlcs::security::Object::Block, hlcs::security::Action::Update);
    commandAttributes.addAuthPair(blockupdate);
    return new SetBootOptions("set_boot_options", "set_boot_options <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoptions|NULL>", commandAttributes);
}

void
SetBootOptions::execute(deque<string> args,
                       mmcs_client::CommandReply& reply,
                       common::ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
SetBootOptions::execute(deque<string> args,
                     mmcs_client::CommandReply& reply,
                     common::ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    if (args[1].size() > BGQDB::BlockInfo::OptionsSize) {
        reply << mmcs_client::FAIL << "boot options too long" << mmcs_client::DONE;
        return;
    }

    result = BGQDB::setBootOptions(args[0],  args[1]);
    switch (result) {
    case BGQDB::OK:
        reply << mmcs_client::OK << mmcs_client::DONE;
        break;
    case BGQDB::INVALID_ID:
        reply << mmcs_client::FAIL << "invalid block id " << args[0] << mmcs_client::DONE;
        break;
    case BGQDB::CONNECTION_ERROR:
        reply << mmcs_client::FAIL << "unable to connect to database" << mmcs_client::DONE;
        break;
    case BGQDB::DB_ERROR:
        reply << mmcs_client::FAIL << "database failure" << mmcs_client::DONE;
        break;
    case BGQDB::NOT_FOUND:
        reply << mmcs_client::FAIL << "block " << args[0] << " not found or not in valid state" << mmcs_client::DONE;
        break;
    case BGQDB::INVALID_ARG:
        reply << mmcs_client::FAIL << "boot options too long" << mmcs_client::DONE;
        break;
    case BGQDB::FAILED:
        reply << mmcs_client::FAIL << "invalid block state" << mmcs_client::DONE;
        break;
    default:
        reply << mmcs_client::FAIL << "unexpected return code from BGQDB::setBootOptions : " << result << mmcs_client::DONE;
        break;
    }
}

void
SetBootOptions::help(deque<string> args,
                      mmcs_client::CommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
  reply << mmcs_client::OK << description()
    << ";Set boot options for a block, all io blocks, all compute blocks, or all blocks."
    << ";To set multiple boot options, separate by commas, with no spaces."
    << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
