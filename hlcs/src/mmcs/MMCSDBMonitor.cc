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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "DBConsoleController.h"
#include "MMCSCommandProcessor.h"
#include "MMCSDBMonitor.h"
#include "MMCSExcludeList.h"
#include "MMCSProperties.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/TxObject.h>

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>

#include <string>

using std::string;
using std::ostringstream;


LOG_DECLARE_FILE( "mmcs" );


#define NORMAL_DB_POLLING_PERIOD 0
#define SLOW_DB_POLLING_PERIOD 3

BlockDBPollingGovernor* blockTransactions = NULL;// Block table poller/governor
extern MMCSCommandMap* mmcsCommands;           // map of mmcs commands to MMCSCommand objects

template<typename Resource, typename Action>
class DBPollingGovernor
{
public:
    DBPollingGovernor() :
        _mutex(PTHREAD_MUTEX_ERRORCHECK_NP),
        _num_current_tran(0),
        _interval_length(0),
        _max_tran_rate(0),
        _max_concurrent_tran(0),
        _tran_repeat_interval(0)
    {

    }

    virtual ~DBPollingGovernor() {}

    virtual BGQDB::STATUS beginTransaction(Resource& resource, Action& action) {
        PthreadMutexHolder mutex;
        mutex.Lock(&_mutex);
        LOG_TRACE_MSG( governorType() << " beginning transaction (" << resource << "," << action << ")" );
        time_t now; time(&now);
        removeOldIntervals(now);    // remove obsolete transaction rate measurements
        removeOldTrans(now);

        // if we exceed the transaction rate, don't allow the transaction
        if (maxConcurrentTranExceeded() || // check the limit on the number of concurrent transactions
                maxIntervalTranRateExceeded() || // check the limit of the number of transactions in each interval
                tranTooFrequent(resource, action)) // check for too frequent repetition of the same transaction
            return BGQDB::NOT_FOUND;    // the mutex is automatically freed on return

        // transaction is allowed: increment counters for the new transaction
        if (_tran_repeat_interval > 0)
            addNewTransaction(now, resource, action); // keep track of the current transaction
        if (_max_tran_rate > 0)
        {
            addNewInterval(now);        // add a new interval for the current transaction
            incrIntervalTran();            // increment the number of transactions in each interval period
        }
        ++_num_current_tran;            // increment the number of transactions currently running
        mutex.Unlock();
        return BGQDB::OK;
    }

    virtual void endTransaction(Resource& resource, Action& action, bool exclude = true) {
        PthreadMutexHolder mutex;
        mutex.Lock(&_mutex);
        --_num_current_tran;        // decrement the number of transactions currently running
        mutex.Unlock();
    }

    // get the number of transactions currently executing
    unsigned getNumCurrentTran() {
        unsigned returnVal;
        PthreadMutexHolder mutex;
        mutex.Lock(&_mutex);
        returnVal = _num_current_tran;
        mutex.Unlock();
        return returnVal;
    }

    // interval for transaction rate measurement, in seconds
    void setInterval(unsigned newInterval) { _interval_length = newInterval; }
    unsigned getInterval() { return _interval_length; }

    // minimum interval between repeats of the same transaction
    void setMinTranRepeatInterval(unsigned tranRepeatInterval) { _tran_repeat_interval = tranRepeatInterval; }
    unsigned getMinTranRepeatInterval() { return _tran_repeat_interval; }

    // maximum rate of all transactions, in transactions per interval. 0 = unlimited
    void setMaxTranRate(unsigned newMaxTranRate) { _max_tran_rate = newMaxTranRate; }
    unsigned getMaxTranRate() { return _max_tran_rate; }

    // maximum number of transactions of any type that can be in execution at any time. 0 = unlimited
    void setMaxConcurrentTran(unsigned newMaxConcurrentTran) { _max_concurrent_tran = newMaxConcurrentTran; }
    unsigned getMaxConcurrentTran() { return _max_concurrent_tran; }

protected:
    struct TranRate {        // keeps transaction rates for a given time period
        time_t   _interval_start; // beginning of the time period
        unsigned _num_tran;    // number of transactions started during this time period
    };
    struct Tran {
        time_t   _interval_start; // beginning of the time period
        Resource _resource;     // Transaction resource (blockid, jobid) starting this period
        Action   _action;    // Transaction action starting this period
    };
    std::deque<TranRate> _intervals;    // transaction rate information for different time intervals
    std::deque<Tran> _transactions;  // transactions executed within the last _tran_repeat_interval seconds
    PthreadMutex  _mutex;    // for serializing operations on the class
    unsigned _num_current_tran; // number of transactions currently executing
    unsigned _interval_length;    // interval for transaction rate measurement
    unsigned _max_tran_rate;    // maximum allowed transactions per interval
    unsigned _max_concurrent_tran;  // maximum concurrent transactions
    unsigned _tran_repeat_interval; // minimum seconds required between repeat transactions
    virtual const string& governorType() { static const string thisType(""); return thisType; }

private:
    // remove entries from _intervals more than _interval seconds old
    // must be called while holding _mutex lock
    void removeOldIntervals(time_t now) {
        while (1)
        {
            if (_intervals.size() == 0 || difftime(now, _intervals[0]._interval_start) < (double) _interval_length)
                break;
            _intervals.erase(_intervals.begin());
        }
    }

    // remove entries from _transactions more than _tran_repeat_interval seconds old
    // must be called while holding _mutex lock
    void removeOldTrans(time_t now) {
        while (1)
        {
            if (_transactions.size() == 0 || difftime(now, _transactions[0]._interval_start) < (double) _tran_repeat_interval)
                break;
            LOG_DEBUG_MSG( governorType() << " removing old transaction (" << _transactions.begin()->_resource << "," << _transactions.begin()->_action << ")" );
            _transactions.erase(_transactions.begin());
        }
    }

    // check whether the maximum simultaneous transaction limit would be exceeded
    // must be called while holding _mutex lock
    bool maxConcurrentTranExceeded() {
        if (getMaxConcurrentTran() > 0 &&
                _num_current_tran >= getMaxConcurrentTran()) // note: use of getNumCurrentTran() here causes mutex abort
        {
            LOG_DEBUG_MSG(governorType() << " -- max concurrent transactions");
            return true;
        }
        else
            return false;
    }

    // check for any intervals in which the maximum transaction rate was exceeded
    // must be called while holding _mutex lock
    bool maxIntervalTranRateExceeded() {
        for (unsigned i = 0; i < _intervals.size(); ++i)
        {
            if (_intervals[i]._num_tran >= _max_tran_rate)
            {
                LOG_DEBUG_MSG(governorType() << " -- max interval transaction rate");
                return true;
            }
        }
        return false;
    }

    // check for repeat transactions occurring too frequently
    // must be called while holding mutex lock
    bool tranTooFrequent(Resource resource, Action action) {
        for (unsigned i = 0; i < _transactions.size(); ++i)
        {
            if (_transactions[i]._resource == resource && _transactions[i]._action == action)
            {
                LOG_DEBUG_MSG(governorType() << " -- transaction(" << resource << "," << action << ") already in progress");
                return true;
            }
        }
        return false;
    }

    // add a new interval for a time period beginning now
    // must be called while holding _mutex lock
    void addNewInterval(time_t now) {
        // avoid duplicate intervals
        if (_intervals.size() > 0 && _intervals[_intervals.size() - 1]._interval_start == now)
            return;
        // add new interval
        TranRate newInterval;
        newInterval._interval_start = now;
        newInterval._num_tran = 0;
        _intervals.push_back(newInterval);
    }

    // increment the number of transactions executed in each interval
    // must be called while holding _mutex lock
    void incrIntervalTran() {
        for (unsigned i = 0; i < _intervals.size(); ++i) {
            ++(_intervals[i]._num_tran);
        }
    }

    // add a new transaction for a time period beginning now
    // must be called while holding _mutex lock
    void addNewTransaction(time_t now, Resource& resource, Action& action) {
        // add new transaction
        Tran newTran;
        newTran._interval_start = now;
        newTran._resource = resource;
        newTran._action = action;
        _transactions.push_back(newTran);
    }
};

class BlockDBPollingGovernor: public DBPollingGovernor<string, BGQDB::BLOCK_ACTION>
{
public:
    BlockDBPollingGovernor() : dbExcludedBlockList(new ExcludeList) {}
    virtual ~BlockDBPollingGovernor() { delete dbExcludedBlockList; }
    BGQDB::STATUS beginTransaction(string& blockName, string& userName, BGQDB::BLOCK_ACTION& action, unsigned int& creationId);
    void endTransaction(string& blockName, BGQDB::BLOCK_ACTION& action, bool exclude);
protected:
    const string& governorType() { static const string thisType("BlockDBPollingGovernor"); return thisType; }
private:
    ExcludeList* dbExcludedBlockList; // blocks to be excluded from DB polling
};

BGQDB::STATUS
BlockDBPollingGovernor::beginTransaction(string& blockName, string& userName, BGQDB::BLOCK_ACTION& action, unsigned int& creationId)
{
    BGQDB::STATUS result;
    string excludedBlocks = dbExcludedBlockList->getSqlListQuoted();

    // get the next block transaction from the database
    result = BGQDB::getBlockAction(
            blockName,
            action,
            creationId,
            excludedBlocks
    );
    if (result != BGQDB::OK || action == BGQDB::NO_BLOCK_ACTION)
        return result;

    // get the user name
    int notused;
    result = BGQDB::getBlockUser(blockName, userName, notused);
    if (result != BGQDB::OK)
        return result;

    // check whether transactions are being limited
    result = DBPollingGovernor<string, BGQDB::BLOCK_ACTION>::beginTransaction(blockName, action);
    if (result == BGQDB::NOT_FOUND) {
        result = BGQDB::OK;
        action = BGQDB::NO_BLOCK_ACTION;
    } else {
        // exclude the block from polling until the transaction is done
        dbExcludedBlockList->add(blockName);
    }

    // move the block to the end of the polling list to keep from repolling immediately
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection");
        return BGQDB::CONNECTION_ERROR;
    }
    string message("update bgqblock set statuslastmodified=CURRENT TIMESTAMP where blockid='");
    message.append(blockName).append("'");
    tx.execStmt(message.c_str());
    return result;
}

void
BlockDBPollingGovernor::endTransaction(string& blockName, BGQDB::BLOCK_ACTION& action, bool exclude = true)
{
    DBPollingGovernor<string, BGQDB::BLOCK_ACTION>::endTransaction(blockName, action);
    dbExcludedBlockList->remove(blockName);
}

void*
DatabaseMonitorThread::threadStart()
{
    BGQDB::STATUS result;                                    // database API return code
    BGQDB::BLOCK_ACTION blockAction = BGQDB::NO_BLOCK_ACTION;    // requested block action from DB table
    unsigned pollingPeriod = NORMAL_DB_POLLING_PERIOD;         // how often to poll the database for work
    unsigned int creationId = 0;                            // specific value for this instance of the block name
    blockTransactions = new BlockDBPollingGovernor;        // Block table poller/governor

    // set the block governor limits
    // number of block transactions that can be running at one time
    blockTransactions->setMaxConcurrentTran(atoi(MMCSProperties::getProperty(BLOCKGOVERNOR_MAX_CONCURRENT).c_str()));
    // length of a measurement interval in seconds
    blockTransactions->setInterval(atoi(MMCSProperties::getProperty(BLOCKGOVERNOR_INTERVAL).c_str()));
    // number of block transactions that can be started in one interval
    blockTransactions->setMaxTranRate(atoi(MMCSProperties::getProperty(BLOCKGOVERNOR_MAX_TRAN_RATE).c_str()));
    // minimum seconds between repeat of a transaction
    blockTransactions->setMinTranRepeatInterval(atoi(MMCSProperties::getProperty(BLOCKGOVERNOR_REPEAT_INTERVAL).c_str()));

    // Poll the database for work to do
    LOG_INFO_MSG( "MMCSDBMonitor started" );

    while (isThreadStopping() == false)
    {
        if (pollingPeriod > 0)
        {
            sleep(pollingPeriod);    // polling period in seconds
        }

        pollingPeriod = SLOW_DB_POLLING_PERIOD;

        //
        // Monitor the DB Block table
        // get the block name and requested action
        //
        if (monitorBlockTable)        // startup parameters: monitor Block Table?
        {
            string blockId;                // block id for requested block action
            string userId;                // user id for requested block action

            // poll the block table for a transaction
            result = blockTransactions->beginTransaction(blockId, userId, blockAction, creationId);
            if (result == BGQDB::OK)
            {
                if (blockAction != BGQDB::NO_BLOCK_ACTION)
                {
                    // Create a thread to
                    // perform the requested action on the block
                    DatabaseBlockCommandThread* commandThread =
                        new DatabaseBlockCommandThread;
                    commandThread->setJoinable(false);
                    commandThread->blockName = blockId;
                    commandThread->userName = userId;
                    commandThread->blockAction = blockAction;
                    commandThread->creationId = creationId;
                    commandThread->blockTransactions = blockTransactions;
                    commandThread->setDeleteOnExit(true); // delete this object when the thread exits
                    pollingPeriod = NORMAL_DB_POLLING_PERIOD;
                    commandThread->start();
                }
            }
            else
            {
                LOG_ERROR_MSG("MMCSDBMonitor, beginTransaction: error accessing the " << BGQDB::DBTBlock().getTableName() << " table, result = " << result);
            }
        }
    }

    // return when done
    LOG_INFO_MSG("MMCSDBMonitor stopped");
    return NULL;
}

void*
DatabaseBlockCommandThread::threadStart()
{
    std::deque<string> args;         // arguments to MMCS command
    MMCSCommandReply reply;             // holds reply from MMCS command
    MMCSServerCommandProcessor mmcsCommandProcessor(mmcsCommands); // MMCS command processor

    // Create a midplane controller object
    DBConsoleController* midplaneController = 0;
    try {
        bgq::utility::UserId uid( userName );
        midplaneController = new DBConsoleController(&mmcsCommandProcessor, uid);
        setThreadName("DBBlockCmd"); // note: setusername command bypasses username checking for this thread
        midplaneController->setMMCSThread(this);
    } catch(std::runtime_error& e) {
        LOG_ERROR_MSG("Invalid user id " << userName);
        return 0;
    }

    LOG_INFO_MSG(
            "DatabaseBlockCommandThread started"
             << ": block " << blockName
             << ", user " << userName
             << ", action " << blockAction
             << ", creationId " << creationId
        );

    switch(blockAction)
    {
        case BGQDB::NO_BLOCK_ACTION:
            break;
        case BGQDB::CONFIGURE_BLOCK:  // allocate and boot the block
            BGQDB::clearBlockAction(blockName, creationId);
            args.clear(); args.push_back(blockName);
            mmcsCommandProcessor.logCommand("db_allocate", args);
            mmcsCommandProcessor.execute("allocate", args, reply, midplaneController);
            midplaneController->deselectBlock();
            break;
        case BGQDB::DEALLOCATE_BLOCK:  // deallocate the block
            args.clear(); args.push_back(blockName);
            mmcsCommandProcessor.logCommand("db_free", args);
            mmcsCommandProcessor.execute("free", args, reply, midplaneController);
            midplaneController->deselectBlock();
            break;
        default:
            LOG_ERROR_MSG( "DatabaseBlockCommandThread unexpected blockAction " << blockAction);
    }

    // end the transaction
    blockTransactions->endTransaction(blockName, blockAction);

    // return when done
    LOG_INFO_MSG("DatabaseBlockCommandThread stopped");
    delete midplaneController;
    return NULL;
}
