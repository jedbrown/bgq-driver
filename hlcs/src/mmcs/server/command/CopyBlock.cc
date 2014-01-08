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

#include "CopyBlock.h"

#include "common/ConsoleController.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

CopyBlock*
CopyBlock::build()
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
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new CopyBlock("copy_block", "copy_block <existingblockid> <newblockid>", commandAttributes);
}

std::vector<std::string>
CopyBlock::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
)
{
    std::vector<std::string> blocks;
    blocks.push_back(cmdString[0]);
    blocks.push_back(cmdString[1]);
    return blocks;
}

bool
CopyBlock::doSpecialAuths(
        std::vector<std::string>& blocks,
        const boost::shared_ptr<hlcs::security::Enforcer>& enforcer,
        procstat& procstat,
        const bgq::utility::UserId& user
)
{

    // Assume success!
    procstat = CMD_EXECUTED;

    try {
        if(blocks.size() != 2)
            procstat = CMD_INVALID;
        hlcs::security::Object existing_blockobj(hlcs::security::Object::Block, blocks[0]);
        if(enforcer->validate(existing_blockobj, hlcs::security::Action::Read, user) == false)
            procstat = CMD_INVALID;
        hlcs::security::Object new_blockobj(hlcs::security::Object::Block, blocks[1]);
        if(enforcer->validate(new_blockobj, hlcs::security::Action::Create, user) == false)
            procstat =  CMD_INVALID;
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        procstat =  CMD_INVALID;
    }

    if (procstat == CMD_INVALID) {
        blocks.clear();
    }

    // Always return true because we're running here.
    return true;
}

void
CopyBlock::execute(
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
CopyBlock::execute(
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

    result = BGQDB::copyBlock(args[0],  args[1], pController->getUser().getUser());
    switch (result) {
        case BGQDB::OK:
            reply << mmcs_client::OK << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << mmcs_client::FAIL << "Invalid block id " << args[1] << " or block already exists" << mmcs_client::DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << mmcs_client::FAIL << "Database failure or block id " << args[1] << " already exists"<< mmcs_client::DONE;
            break;
        case BGQDB::NOT_FOUND:
            reply << mmcs_client::FAIL << "Block " << args[0] << " not found" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << mmcs_client::FAIL << "Invalid arguments provided" << mmcs_client::DONE;
            break;
        default:
            reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::copyBlock : " << result << mmcs_client::DONE;
            break;
    }
}

void
CopyBlock::help(std::deque<std::string> args,
        mmcs_client::CommandReply& reply)
{
    BGQDB::DBTBlock db;
    reply << mmcs_client::OK << description()
          << ";Copy an existing block, along with all of its boot information."
          << ";The maximum size of the new block ID is " << sizeof(BGQDB::DBTBlock()._blockid) - 1 << " characters"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
