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

#include "GenBlocks.h"

#include "common/ConsoleController.h"

#include <db/include/api/genblock.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

GenBlocks*
GenBlocks::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new GenBlocks("gen_blocks", "gen_blocks [<blockidprefix>]", commandAttributes);
}

void
GenBlocks::execute(
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
GenBlocks::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    BGQDB::STATUS result;

    if (args.size() == 1) {
        result = BGQDB::genBlocks(pController->getUser().getUser(), args[0]);
    } else {
        result = BGQDB::genBlocks(pController->getUser().getUser());
    }

    switch (result) {
        case BGQDB::OK:
            reply << mmcs_client::OK << "Blocks created with default images, use set_block_info to change images" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << mmcs_client::FAIL << "blockidprefix " << args[0] << " too big or contains invalid characters" << mmcs_client::DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << mmcs_client::FAIL << "Database failure or duplicate block name" << mmcs_client::DONE;
            break;
        case BGQDB::FAILED:
            reply << mmcs_client::FAIL << "Invalid block configuration" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << mmcs_client::FAIL << "Invalid arguments provided" << mmcs_client::DONE;
            break;
        case BGQDB::DUPLICATE:
            reply << mmcs_client::FAIL << "Duplicate block name" << mmcs_client::DONE;
            break;
        default:
            reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::genBlocks : " << result << mmcs_client::DONE;
            break;
    }
}

void
GenBlocks::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Generate a block for each midplane on the machine."
          << ";The <blockidprefix> is optional."
          << ";If omitted, each block name will be the location of the midplane, i.e. Rxx-Mx."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
