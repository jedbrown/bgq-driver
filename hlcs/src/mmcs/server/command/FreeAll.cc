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

#include "FreeAll.h"

#include "Free.h"

#include "../DBConsoleController.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/tableapi/gensrc/DBTBlock.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/scoped_ptr.hpp>

using namespace std;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

FreeAll*
FreeAll::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(false);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new FreeAll("free_all", "free_all [ options ]", commandAttributes);
}

void
FreeAll::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    getBlockObjects(args, pController);
    execute(args, reply, pController, pTarget, &_blocks);
    _blocks.clear();
}

void
FreeAll::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    // Here's the heavy lifting to do the actual freeing
    size_t failureCount = 0;
    for (unsigned int i = 0; i < validnames->size(); ++i) {
        std::vector<std::string> name;
        const std::string block = validnames->at(i);
        name.push_back(block);
        const boost::scoped_ptr<Free> free_cmd( command::Free::build() );
        free_cmd->execute(
                boost::assign::list_of(std::string(block)),
                reply,
                pController,
                pTarget,
                &name
                );
        if ( reply.getStatus() != 0 ) {
            LOG_WARN_MSG( "Could not free block " << block << ": " << reply.str());
            failureCount++;
            reply.str("");
        }
        name.clear();
    }

    // send reply if we didn't have any failures
    if ( !failureCount ) {
        reply << mmcs_client::OK << "Freed " << validnames->size() << " " << (validnames->size() == 1 ? "block" : "blocks") << mmcs_client::DONE;
    } else {
        reply << mmcs_client::FAIL << "Freed " << validnames->size() - failureCount << " out of " << validnames->size() << " blocks" << mmcs_client::DONE;
    }
}

std::vector<std::string>
FreeAll::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
)
{
    // default scope is to free all compute blocks, the arguments to this
    // command can alter that behavior
    std::vector<std::string> blocks;
    std::string scope( "AND " + BGQDB::DBTBlock::NUMCNODES_COL + " <> 0" );
    if ( cmdString.size() == 1 ) {
        if ( cmdString[0] == "io" ) {
            scope = std::string( "AND " + BGQDB::DBTBlock::NUMIONODES_COL + " <> 0" );
        } else if ( cmdString[0] == "compute" ) {
            // default scope is fine
        } else if ( cmdString[0] == "both" ) {
            scope.clear();
        } else {
            LOG_ERROR_MSG( "Unknown argument: " << cmdString[0]);
            return blocks;
        }
    }

    try {
        // get list of all non-free blocks owned by this user
        const cxxdb::ConnectionPtr connection(
                BGQDB::DBConnectionPool::instance().getConnection()
                );
        const cxxdb::QueryStatementPtr statement(
                connection->prepareQuery(
                    "SELECT " + BGQDB::DBTBlock::BLOCKID_COL + ", " +
                    BGQDB::DBTBlock::NUMIONODES_COL + " FROM TBGQBlock "
                    "WHERE " + BGQDB::DBTBlock::STATUS_COL + " <> 'F' "
                    "AND " + BGQDB::DBTBlock::USERNAME_COL + "=? "
                    + scope,
                    boost::assign::list_of( BGQDB::DBTBlock::USERNAME_COL )
                    )
                );
        statement->parameters()[ BGQDB::DBTBlock::USERNAME_COL ].set( pController->getUser().getUser() );
        const cxxdb::ResultSetPtr result( statement->execute() );
        std::vector<std::string> ioblocks;
        while ( result->fetch() ) {
            LOG_DEBUG_MSG("Block " << result->columns()[ BGQDB::DBTBlock::BLOCKID_COL ].getString() << " set to be freed.");
            if (result->columns()[BGQDB::DBTBlock::NUMIONODES_COL].getInt64() == 0)
                blocks.push_back( result->columns()[ BGQDB::DBTBlock::BLOCKID_COL ].getString() );
            else
                ioblocks.push_back(result->columns()[ BGQDB::DBTBlock::BLOCKID_COL ].getString() );
        }

        // Now put any/all IO blocks at the end of the list so the CN blocks get freed first.
        for (std::vector<std::string>::iterator it = ioblocks.begin(); it != ioblocks.end(); ++it) {
            blocks.push_back(*it);
        }
    } catch ( const cxxdb::DatabaseException& e ) {
        LOG_ERROR_MSG(e.what());
    }

    _blocks = blocks;
    return blocks;
}

void
FreeAll::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Release all blocks in use by the console user. By default only compute"
          << ";blocks are freed, to alter this behavior use one of the options below."
          << ";Options:"
          << ";  io - free I/O blocks"
          << ";  compute - free compute blocks"
          << ";  both - free both compute and I/O blocks"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
