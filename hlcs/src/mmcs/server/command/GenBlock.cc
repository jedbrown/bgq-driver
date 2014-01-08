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


#include "GenBlock.h"

#include "common/ConsoleController.h"

#include <db/include/api/genblock.h>


using namespace std;


namespace mmcs {
namespace server {
namespace command {


GenBlock*
GenBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new GenBlock("gen_block", "gen_block <blockid> <midplane>", commandAttributes);
}


void
GenBlock::execute(deque<string> args,
                       mmcs_client::CommandReply& reply,
                       common::ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
GenBlock::execute(deque<string> args,
                     mmcs_client::CommandReply& reply,
                     common::ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    if(args.size() != 2)
    {
        reply << mmcs_client::FAIL << "args? " << usage << mmcs_client::DONE;
        return;
    }

    BGQDB::STATUS result;
    result = BGQDB::genBlock(args[0], args[1], pController->getUser().getUser());
    switch (result) {
    case BGQDB::OK:
    reply << mmcs_client::OK << "block created with default images, use set_block_info to change images" << mmcs_client::DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << mmcs_client::FAIL << "blockid " << args[0] << " too big or contains invalid characters" << mmcs_client::DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << mmcs_client::FAIL << "unable to connect to database" << mmcs_client::DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << mmcs_client::FAIL << "database failure or duplicate block name" << mmcs_client::DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << mmcs_client::FAIL << "midplane not found" << mmcs_client::DONE;
    break;
    case BGQDB::FAILED:
    reply << mmcs_client::FAIL << "invalid block configuration" << mmcs_client::DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << mmcs_client::FAIL << "invalid arguments provided" << mmcs_client::DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << mmcs_client::FAIL << "blockid " << args[0] << " already exists" << mmcs_client::DONE;
    break;
    default:
    reply << mmcs_client::FAIL << "unexpected return code from BGQDB::genBlock : " << result << mmcs_client::DONE;
    break;
    }
}

void
GenBlock::help(deque<string> args,
                  mmcs_client::CommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << mmcs_client::OK << description()
      << ";Generate block for a midplane.  Specify <midplane> by location, i.e. Rxx-Mx."
      << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command

