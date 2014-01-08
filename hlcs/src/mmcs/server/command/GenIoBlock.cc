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


#include "GenIoBlock.h"

#include "common/ConsoleController.h"

#include <db/include/api/genblock.h>


using namespace std;


namespace mmcs {
namespace server {
namespace command {


GenIoBlock*
GenIoBlock::build()
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
    return new GenIoBlock("gen_io_block", "gen_io_block <blockid> <location> <ionodes>", commandAttributes);
}

void
GenIoBlock::execute(deque<string> args,
                       mmcs_client::CommandReply& reply,
                       common::ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
GenIoBlock::execute(deque<string> args,
                       mmcs_client::CommandReply& reply,
                       common::ConsoleController* pController,
                       BlockControllerTarget* pTarget,
                       std::vector<std::string>* validnames)
{
    int nodes = 0;
    try {
        nodes = boost::lexical_cast<int>(args[2]);
    } catch (const boost::bad_lexical_cast& e) {
        reply << mmcs_client::FAIL << "bad node count: " << e.what() << mmcs_client::DONE;
        return;
    }
    if ( nodes <= 0 ) {
        reply << mmcs_client::FAIL << "invalid node count: " << args[2] << mmcs_client::DONE;
        return;
    }

    BGQDB::STATUS result;
    result = BGQDB::genIOBlock(args[0], args[1], nodes, pController->getUser().getUser());

    switch (result) {
    case BGQDB::OK:
    reply << mmcs_client::OK << "block created with default images, use set_block_info to change images" << mmcs_client::DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << mmcs_client::FAIL << "blockid " << args[0] << " already exists" << mmcs_client::DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << mmcs_client::FAIL << "blockid " << args[0] << " too big or contains invalid characters" << mmcs_client::DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << mmcs_client::FAIL << "unable to connect to database" << mmcs_client::DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << mmcs_client::FAIL << "database failure" << mmcs_client::DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << mmcs_client::FAIL << "block configuration data not found" << mmcs_client::DONE;
    break;
    case BGQDB::FAILED:
    reply << mmcs_client::FAIL << "invalid block configuration" << mmcs_client::DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << mmcs_client::FAIL << "invalid arguments provided" << mmcs_client::DONE;
    break;
    default:
    reply << mmcs_client::FAIL << "unexpected return code from BGQDB::genIOBlock : " << result << mmcs_client::DONE;
    break;
    }
}

void
GenIoBlock::help(deque<string> args,
                    mmcs_client::CommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << mmcs_client::OK << description()
      << ";Generate a block for a set of IO nodes."
      << ";For <location> specify an IO node, i.e. Qxx-Ix-Jxx or Rxx-Ix-Jxx, or an IO drawer, i.e. Qxx-Ix or Rxx-Ix."
      << ";The number of nodes must be a multiple of 8 when specifying an IO drawer."
      << ";The number of nodes must 1, 2, or 4 when specifying an IO node."
      << ";For 2 nodes, the node location must be J00, J02, J04, or J06."
      << ";For 4 nodes, the node location must be J00 or J04."
      << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command

