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

#include "DatabaseMonitorThread.h"

#include "BlockDBPollingGovernor.h"
#include "DatabaseBlockCommandThread.h"

#include "common/Properties.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/gensrc/DBTBlock.h>

#include <utility/include/Log.h>

using namespace std;

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.server" );

#define NORMAL_DB_POLLING_PERIOD 0
#define SLOW_DB_POLLING_PERIOD 3

namespace mmcs {
namespace server {

void*
DatabaseMonitorThread::threadStart()
{
    BGQDB::STATUS result;                                      // Database API return code
    BGQDB::BLOCK_ACTION blockAction = BGQDB::NO_BLOCK_ACTION;  // Requested block action from DB table
    unsigned pollingPeriod = NORMAL_DB_POLLING_PERIOD;         // How often to poll the database for work
    BlockDBPollingGovernor blockTransactions;                  // Block table poller/governor

    // Set the block governor limits
    // Number of block transactions that can be running at one time
    blockTransactions.setMaxConcurrentTran(atoi(Properties::getProperty(BLOCKGOVERNOR_MAX_CONCURRENT).c_str()));
    // Length of a measurement interval in seconds
    blockTransactions.setInterval(atoi(Properties::getProperty(BLOCKGOVERNOR_INTERVAL).c_str()));
    // Number of block transactions that can be started in one interval
    blockTransactions.setMaxTranRate(atoi(Properties::getProperty(BLOCKGOVERNOR_MAX_TRAN_RATE).c_str()));
    // Minimum seconds between repeat of a transaction
    blockTransactions.setMinTranRepeatInterval(atoi(Properties::getProperty(BLOCKGOVERNOR_REPEAT_INTERVAL).c_str()));

    // Poll the database for work to do
    LOG_TRACE_MSG("Database polling thread started.");

    while (!isThreadStopping()) {
        if (pollingPeriod > 0)        {
            sleep(pollingPeriod);    // polling period in seconds
        }

        pollingPeriod = SLOW_DB_POLLING_PERIOD;
        string blockId;
        string userId;

        // Poll the block table for a transaction
        result = blockTransactions.beginTransaction(blockId, userId, blockAction);
        if (result == BGQDB::OK) {
            if (blockAction != BGQDB::NO_BLOCK_ACTION) {
                DatabaseBlockCommandThread* const commandThread( new DatabaseBlockCommandThread );
                commandThread->setJoinable(false);
                commandThread->_name = blockId;
                commandThread->_userName = userId;
                commandThread->_action = blockAction;
                commandThread->_transactions = &blockTransactions;
                commandThread->_commands = commands;
                commandThread->setDeleteOnExit(true); // delete this object when the thread exits
                pollingPeriod = NORMAL_DB_POLLING_PERIOD;
                commandThread->start();
            }
        }
        else {
            LOG_ERROR_MSG("Error accessing the " << BGQDB::DBTBlock().getTableName() << " table, result = " << result);
        }
    }

    // return when done
    LOG_TRACE_MSG("Database polling thread stopped.");
    return NULL;
}


} } // namespace mmcs::server
