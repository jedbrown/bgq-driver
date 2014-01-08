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

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/assign.hpp>


using namespace std;


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
ListBlocks::execute(deque<string> args,
                 mmcs_client::CommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget)
{
    std::vector<std::string> validnames;
    execute(args, reply, pController, pTarget, &validnames);
}

void
ListBlocks::execute(deque<string> args,
                 mmcs_client::CommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget,
                 std::vector<std::string>* validnames)
{
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

    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&pController->_blockMapMutex);
    assert(mutex_rc == 0);

    reply << mmcs_client::OK;
    if (pController->_blockMap.size() != 0)
    {
        for(BlockMap::iterator it = pController->_blockMap.begin(); it != pController->_blockMap.end(); ++it)
        {
            statement->parameters()[ "id" ].set( it->first );
            const cxxdb::ResultSetPtr result( statement->execute() );
            if ( !result ) continue;
            if ( !result->fetch() ) continue;

            bool valid = true;
            DBBlockPtr pBlock;
            std::string blockname;
#ifdef WITH_SECURITY
            if(std::find(validnames->begin(), validnames->end(), it->first) == validnames->end())
                valid = false;
            pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());
            if(pBlock != 0) {
                blockname = pBlock->getBase()->getBlockName();
            }
#endif

            if(valid || it->first == blockname) {
                reply << setw(16) << left << it->first << " " << result->columns()[ BGQDB::DBTBlock::STATUS_COL ].getString() << " "
                    << left << setw(8) << it->second->getBase()->getUserName() << "(" << it->second->getBase()->peekBlockThreads() << ")" ;
                if (it->second->getBase()->isConnected())
                    reply << "\tconnected";
                if (it->second->getBase()->peekDisconnecting())
                    reply << "\tdisconnecting - " << it->second->getBase()->disconnectReason();
                if (it->second->getBase()->getRedirectSock() != 0)
                    reply << "\tredirecting";
                reply << ";";
            }
        }
    }
    reply << mmcs_client::DONE;
    mutex.Unlock();
    return;
}

std::vector<std::string>
ListBlocks::getBlockObjects(std::deque<std::string>& cmdString,
                                  DBConsoleController* pController) {
    PthreadMutexHolder holder;
    holder.Lock(&DBConsoleController::getBlockMapMutex());
    std::vector<std::string> retvec;
    for(BlockMap::iterator i = DBConsoleController::getBlockMap().begin();
        i != DBConsoleController::getBlockMap().end(); ++i) {
        retvec.push_back(i->first);
    }
    return retvec;
}

void
ListBlocks::help(deque<string> args,
        mmcs_client::CommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << mmcs_client::OK << description()
        << ";Lists allocated blocks."
        << ";Output includes user, number of consoles started, and if output is redirected to console."
        << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
