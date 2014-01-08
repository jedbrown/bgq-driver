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

#include "GenMidplaneBlock.h"

#include "common/ConsoleController.h"

#include <db/include/api/genblock.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

GenMidplaneBlock*
GenMidplaneBlock::build()
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
    return new GenMidplaneBlock("gen_midplane_block", "gen_midplane_block <blockid> <corner> <asize> <bsize> <csize> <dsize> [aPT bPT cPT dPT]", commandAttributes);
}

void
GenMidplaneBlock::execute(
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
GenMidplaneBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if ((args.size() != 6) && (args.size() != 10)) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    BGQDB::STATUS result;

    int asize, bsize, csize, dsize;
    try {
        asize = boost::lexical_cast<int>(args[2]);
        bsize = boost::lexical_cast<int>(args[3]);
        csize = boost::lexical_cast<int>(args[4]);
        dsize = boost::lexical_cast<int>(args[5]);
    } catch (const boost::bad_lexical_cast& e) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    }

    if ( asize <= 0 ) {
        reply << mmcs_client::FAIL << "Invalid a dimension size: " << args[2] << mmcs_client::DONE;
        return;
    }
    if ( bsize <= 0 ) {
        reply << mmcs_client::FAIL << "Invalid b dimension size: " << args[3] << mmcs_client::DONE;
        return;
    }
    if ( csize <= 0 ) {
        reply << mmcs_client::FAIL << "Invalid c dimension size: " << args[4] << mmcs_client::DONE;
        return;
    }
    if ( dsize <= 0 ) {
        reply << mmcs_client::FAIL << "Invalid d dimension size: " << args[5] << mmcs_client::DONE;
        return;
    }

    if (args.size() == 6) {
        result = BGQDB::genMidplaneBlock(args[0], args[1], asize, bsize, csize, dsize, pController->getUser().getUser() );
    } else {
        result = BGQDB::genMidplaneBlock(args[0], args[1], asize, bsize, csize, dsize, pController->getUser().getUser(), args[6], args[7], args[8], args[9] );
    }

    switch (result) {
        case BGQDB::OK:
            reply << mmcs_client::OK << "Block created with default images, use set_block_info to change images" << mmcs_client::DONE;
            break;
        case BGQDB::DUPLICATE:
            reply << mmcs_client::FAIL << "Block id " << args[0] << " already exists" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << mmcs_client::FAIL << "Block id " << args[0] << " too big or contains invalid characters" << mmcs_client::DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << mmcs_client::FAIL << "Database failure" << mmcs_client::DONE;
            break;
        case BGQDB::NOT_FOUND:
            reply << mmcs_client::FAIL << "Block configuration data not found" << mmcs_client::DONE;
            break;
        case BGQDB::FAILED:
            reply << mmcs_client::FAIL << "Invalid block configuration" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << mmcs_client::FAIL << "Invalid arguments provided" << mmcs_client::DONE;
            break;
        default:
            reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::genMidplaneBlock : " << result << mmcs_client::DONE;
            break;
    }
}

void
GenMidplaneBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Generate a block for a set of midplanes."
          << ";For <corner> specify midplane by location, i.e. Rxx-Mx."
          << ";The size is provided in terms of number of midplanes in A, B, C and D dimensions."
          << ";The <corner> will be in the 0,0,0,0 position of the midplanes that make up the block."
          << ";Except for the <corner>, the midplanes included in the generated block"
          << ";depend on the A,B,C,D cabling of your machine."
          << ";;aPT,bPT,cPT,dPT are optional arguments to specify the use of passthrough and are"
          << ";expected to be a string of 1's (include) and 0's (passthrough)."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
