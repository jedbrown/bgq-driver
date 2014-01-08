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

#include "SetBlockInfo.h"

#include <db/include/api/BGQDBlib.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

SetBlockInfo*
SetBlockInfo::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockupdate(hlcs::security::Object::Block, hlcs::security::Action::Update);
    commandAttributes.addAuthPair(blockupdate);
    return new SetBlockInfo("set_block_info", "set_block_info <blockid> <uloader> <nodecfg> [<bootoptions>]", commandAttributes);
}

void
SetBlockInfo::execute(
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
SetBlockInfo::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;

    if (args.size() < 3) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    if (args[1].size() >= BGQDB::BlockInfo::MicroLoaderSize) {
        reply << mmcs_client::FAIL << "microloader image name too long" << mmcs_client::DONE;
        return;
    }

    if (args[2].size() > sizeof(bInfo.nodeConfig)) {
        reply << mmcs_client::FAIL << "node config name too long" << mmcs_client::DONE;
        return;
    }

    strcpy(bInfo.uloaderImg, args[1].c_str());
    strcpy(bInfo.nodeConfig, args[2].c_str());

    if (args.size() > 3) {
        if (args[3].size() > BGQDB::BlockInfo::OptionsSize) {
            reply << mmcs_client::FAIL << "boot options too long" << mmcs_client::DONE;
            return;
        }
        strcpy(bInfo.bootOptions, args[3].c_str());
    } else {
        strcpy(bInfo.bootOptions, "\0");
    }

    if (args.size() == 5) {
        if (args[4].length() >= sizeof(bInfo.options)) {
            reply << mmcs_client::FAIL << "options too long" << mmcs_client::DONE;
            return;
        }
        strcpy(bInfo.options, args[4].c_str());

    } else if (args.size() < 5) {
        strcpy(bInfo.options, " ");
    } else {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    result = BGQDB::setBlockInfo(args[0], bInfo);
    switch (result) {
        case BGQDB::OK:
            reply << mmcs_client::OK << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << mmcs_client::FAIL << "Invalid block id " << args[0] << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << mmcs_client::FAIL << "Invalid argument" << mmcs_client::DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << mmcs_client::FAIL << "Database failure or invalid node config specified" << mmcs_client::DONE;
            break;
        case BGQDB::NOT_FOUND:
            reply << mmcs_client::FAIL << "Block " << args[0] << " not found or not in valid status" << mmcs_client::DONE;
            break;
        case BGQDB::FAILED:
            reply << mmcs_client::FAIL << "Invalid block status" << mmcs_client::DONE;
            break;
        default:
            reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::setBlockInfo : " << result << mmcs_client::DONE;
            break;
    }
}

void
SetBlockInfo::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description() << ";Set boot details for a block." << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
