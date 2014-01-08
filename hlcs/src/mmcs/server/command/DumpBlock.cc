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

#include "DumpBlock.h"

#include "../DBBlockController.h"

using namespace std;

namespace mmcs {
namespace server {
namespace command {

DumpBlock*
DumpBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    commandAttributes.helpCategory(common::SPECIAL);
    return new DumpBlock("dump_block_server", "dump_block <file.xml> <blockId>", commandAttributes);
}

void
DumpBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> validnames;
    validnames.push_back(args[0]);
    execute(args, reply, pController, pTarget, &validnames);
}

void
DumpBlock::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    // Write XML file into stream object
    std::ostringstream blockStreamXML;
    BGQDB::STATUS result = BGQDB::OK;
    try {
        result = BGQDB::getBlockXML(blockStreamXML, validnames->at(0));
        if ( result == BGQDB::OK ) {
            reply << mmcs_client::OK << blockStreamXML.str() << mmcs_client::DONE;
            return;
        }
    } catch(...) {
        reply << mmcs_client::FAIL << "Database error, could not dump block " << validnames->at(0) << ". " << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::FAIL
          << "Generation of XML failed for block " << validnames->at(0) << ": "
          << DBBlockController::strDBError(result) << mmcs_client::DONE;
}

std::vector<std::string>
DumpBlock::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
)
{
    std::vector<std::string> retvec;
    retvec.push_back(cmdString[0]);
    return retvec;
}

void
DumpBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Export a block <blockId> from the database into <file.xml>"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
