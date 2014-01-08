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


#include "BlockDBPollingGovernor.h"

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/TxObject.h>

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "mmcs.server" );


namespace mmcs {
namespace server {


BGQDB::STATUS
BlockDBPollingGovernor::beginTransaction(
        std::string& blockName,
        std::string& userName,
        BGQDB::BLOCK_ACTION& action
        )
{
    BGQDB::STATUS result;
    const std::string excludedBlocks = dbExcludedBlockList.getSqlListQuoted();

    // get the next block transaction from the database
    result = BGQDB::getBlockAction(
            blockName,
            action,
            excludedBlocks
    );
    if (result != BGQDB::OK || action == BGQDB::NO_BLOCK_ACTION) {
        return result;
    }

    // get the user name
    int notused;
    result = BGQDB::getBlockUser(blockName, userName, notused);
    if (result != BGQDB::OK) {
        return result;
    }

    // check whether transactions are being limited
    result = DBPollingGovernor<std::string, BGQDB::BLOCK_ACTION>::beginTransaction(blockName, action);
    if (result == BGQDB::NOT_FOUND) {
        result = BGQDB::OK;
        action = BGQDB::NO_BLOCK_ACTION;
    } else {
        // exclude the block from polling until the transaction is done
        dbExcludedBlockList.add(blockName);
    }

    // move the block to the end of the polling list to keep from repolling immediately
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection");
        return BGQDB::CONNECTION_ERROR;
    }
    const std::string message("update bgqblock set statuslastmodified=CURRENT TIMESTAMP where blockid='" + blockName + "'");
    tx.execStmt(message.c_str());
    return result;
}

void
BlockDBPollingGovernor::endTransaction(
        std::string& blockName,
        BGQDB::BLOCK_ACTION& action,
        bool exclude
        )
{
    DBPollingGovernor<std::string, BGQDB::BLOCK_ACTION>::endTransaction(blockName, action);
    dbExcludedBlockList.remove(blockName);
}


} } // namespace mmcs::server
