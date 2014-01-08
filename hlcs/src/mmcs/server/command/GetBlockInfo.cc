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

#include "GetBlockInfo.h"

#include <db/include/api/BGQDBlib.h>

namespace mmcs {
namespace server {
namespace command {

GetBlockInfo*
GetBlockInfo::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::DEFAULT);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new GetBlockInfo("get_block_info", "get_block_info <blockId>", commandAttributes);
}

std::vector<std::string>
GetBlockInfo::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
)
{
    std::vector<std::string> block_to_use;
    block_to_use.push_back(cmdString[0]);
    return block_to_use;
}

void
GetBlockInfo::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> validnames;
    if ( !args.empty() ) {
        validnames.push_back( args[0] );
    }
    this->execute( args, reply, pController, pTarget, &validnames );
}

void
GetBlockInfo::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    BGQDB::BlockInfo bInfo;

    const BGQDB::STATUS result = BGQDB::getBlockInfo(validnames->at(0), bInfo);

    if (result == BGQDB::OK) {
        reply << mmcs_client::OK;
        reply << "Boot info for block " << args[0] << ":\n";
        reply << "uloader: " << bInfo.uloaderImg << "\n";
        reply << "node config: " << bInfo.nodeConfig << "\n";
        reply << "status: " << bInfo.status << "\n";
        reply << "boot options: " << bInfo.bootOptions << "\n";

        if (bInfo.domains.size() > 0) {
            reply << "\n";
            reply << "Node config info for " << bInfo.nodeConfig << ":\n";
            for(unsigned dm = 0 ; dm < bInfo.domains.size() ; ++dm) {
                reply << " domain id: " << bInfo.domains[dm].id << "\n";
                reply << "   images: " << bInfo.domains[dm].imageSet << "\n";
                reply << "   starting core: " << bInfo.domains[dm].startCore << "\n";
                reply << "   ending core: " << bInfo.domains[dm].endCore << "\n";
                reply << "   starting address: " << "0x" << std::hex << bInfo.domains[dm].startAddr << "\n";
                if ( bInfo.domains[dm].endAddr == -1 ) {
                    reply << "   ending address: " << std::dec << bInfo.domains[dm].endAddr << "\n";
                } else {
                    reply << "   ending address: " << "0x" << std::hex << bInfo.domains[dm].endAddr << "\n";
                }
                reply << "   Customization address: " << "0x" << std::hex << bInfo.domains[dm].custAddr << "\n";
                reply << "\n";
            }
        }

        reply << mmcs_client::DONE;
        return;
    } else if (result == BGQDB::NOT_FOUND) {
        reply << mmcs_client::FAIL << "Block not found: " << args[0] << mmcs_client::DONE;
    } else if (result == BGQDB::DB_ERROR) {
        reply << mmcs_client::FAIL << "Database query failed" << mmcs_client::DONE;
    } else if (result == BGQDB::INVALID_ID) {
        reply << mmcs_client::FAIL << "Invalid id: " << args[0] << mmcs_client::DONE;
    } else {
        reply << mmcs_client::FAIL << "Unexpected database error" << mmcs_client::DONE; /* returned status should only be one of the above */
    }
}

void
GetBlockInfo::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Prints boot information, boot mode and status for the specified block."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
