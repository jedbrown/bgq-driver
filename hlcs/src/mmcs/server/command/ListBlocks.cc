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

#include "ListBlocks.h"

#include "../BlockControllerBase.h"
#include "../BlockHelper.h"
#include "../DBConsoleController.h"
#include "../DBBlockController.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

ListBlocks*
ListBlocks::build()
{
    Attributes commandAttributes;
    commandAttributes.mmcsServerCommand(true);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new ListBlocks("list_blocks", "list_blocks", commandAttributes);
}

void
ListBlocks::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> validnames;
    execute(args, reply, pController, pTarget, &validnames);
}

void
ListBlocks::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    BOOST_ASSERT( validnames );

    const cxxdb::ConnectionPtr connection(
            BGQDB::DBConnectionPool::instance().getConnection()
            );
    const cxxdb::QueryStatementPtr statement(
            connection->prepareQuery(
                std::string("SELECT ") + BGQDB::DBTBlock::STATUS_COL + " FROM " +
                BGQDB::DBTBlock().getTableName() + " " +
                "WHERE " + BGQDB::DBTBlock::BLOCKID_COL + "=?",
                boost::assign::list_of( "id" )
                )
            );

    // administrators can see everything, results are not filtered
    const bool admin( pController->getUserType() == CxxSockets::Administrator );

    // count how many blocks are filtered due to security settings
    unsigned filtered = 0;

    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&pController->_blockMapMutex);
    assert(mutex_rc == 0);

    reply << mmcs_client::OK;
    for (
            BlockMap::const_iterator it = pController->_blockMap.begin();
            it != pController->_blockMap.end();
            ++it)
    {
        statement->parameters()[ "id" ].set( it->first );
        const cxxdb::ResultSetPtr result( statement->execute() );
        if ( !result ) {
            continue;
        }

        if ( !result->fetch() ) {
            continue;
        }

        // filter results for non administrators
        const bool valid(
                admin ? true : std::find(validnames->begin(), validnames->end(), it->first) != validnames->end()
        );

        if ( !valid ) {
            ++filtered;
            continue;
        }

        reply << std::setw(16) << std::left << it->first << " " << result->columns()[ BGQDB::DBTBlock::STATUS_COL ].getString() << " "
        << std::left << std::setw(8) << it->second->getBase()->getUserName() << "(" << it->second->getBase()->peekBlockThreads() << ")" ;
        if (it->second->getBase()->isConnected())
            reply << "\tconnected";
        if (it->second->getBase()->peekDisconnecting())
            reply << "\tdisconnecting - " << it->second->getBase()->disconnectReason();
        if (it->second->getBase()->getRedirectSock() != 0)
            reply << "\tredirecting";
        reply << ";";
    }

    if ( filtered ) {
        reply << ";Some blocks were not displayed due to security settings;";
    }

    reply << mmcs_client::DONE;

    return;
}

std::vector<std::string>
ListBlocks::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
)
{
    PthreadMutexHolder holder;
    holder.Lock(&DBConsoleController::getBlockMapMutex());
    std::vector<std::string> retvec;
    for (
            BlockMap::const_iterator i = DBConsoleController::getBlockMap().begin();
            i != DBConsoleController::getBlockMap().end();
            ++i
    )
    {
        retvec.push_back(i->first);
    }
    return retvec;
}

void
ListBlocks::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
          << ";Lists allocated blocks."
          << ";Output includes user, number of consoles started, and if output is redirected to console."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
