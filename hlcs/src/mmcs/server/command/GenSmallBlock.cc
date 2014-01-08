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

#include "GenSmallBlock.h"

#include "common/ConsoleController.h"

#include <db/include/api/genblock.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

GenSmallBlock*
GenSmallBlock::build()
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
    return new GenSmallBlock("gen_small_block", "gen_small_block <blockid> <midplane> <cnodes> <nodecard>", commandAttributes);
}

void
GenSmallBlock::execute(
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
GenSmallBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    BGQDB::STATUS result;

    int nodes = 0;
    try {
        nodes = boost::lexical_cast<int>(args[2]);
    } catch (const boost::bad_lexical_cast& e) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    }

    if ( nodes <= 0 ) {
        reply << mmcs_client::FAIL << "Invalid node count: " << args[2] << mmcs_client::DONE;
        return;
    }

    result = BGQDB::genSmallBlock(args[0], args[1], nodes, args[3], pController->getUser().getUser());
    switch (result) {
    case BGQDB::OK:
        reply << mmcs_client::OK << "Block created with default images, use set_block_info to change images" << mmcs_client::DONE;
        break;
    case BGQDB::INVALID_ID:
        reply << mmcs_client::FAIL << "Invalid block id " << args[0] << mmcs_client::DONE;
        break;
    case BGQDB::CONNECTION_ERROR:
        reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
        break;
    case BGQDB::DB_ERROR:
        reply << mmcs_client::FAIL << "Database failure or duplicate block name" << mmcs_client::DONE;
        break;
    case BGQDB::NOT_FOUND:
        reply << mmcs_client::FAIL << "Midplane not found" << mmcs_client::DONE;
        break;
    case BGQDB::FAILED:
        reply << mmcs_client::FAIL << "Invalid block configuration" << mmcs_client::DONE;
        break;
    case BGQDB::INVALID_ARG:
        reply << mmcs_client::FAIL << "Invalid arguments provided" << mmcs_client::DONE;
        break;
    case BGQDB::DUPLICATE:
        reply << mmcs_client::FAIL << "Block " << args[0] << " already exists" << mmcs_client::DONE;
        break;
    default:
        reply << mmcs_client::FAIL << "Uexpected return code from BGQDB::genSmallBlock : " << result << mmcs_client::DONE;
    break;
    }
}

void
GenSmallBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Generate a sub-midplane block."
          << ";Specify <midplane> by location, i.e. Rxx-Mx."
          << ";The <cnodes> is the total number of compute nodes and must be 32, 64, 128, or 256."
          << ";The <nodecard> is the location of the compute nodes for the block, i.e. N00, N01, etc."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
