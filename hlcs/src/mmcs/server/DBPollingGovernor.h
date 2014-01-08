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


#ifndef MMCS_SERVER_DB_POLLING_GOVERNOR_H_
#define MMCS_SERVER_DB_POLLING_GOVERNOR_H_


#include <bgq_util/include/pthreadmutex.h>

#include <db/include/api/BGQDBlib.h>

#include <extlib/include/log4cxx/logger.h>

#include <string>


namespace mmcs {
namespace server {


extern log4cxx::LoggerPtr dbPollingGovernorLogger;


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

        LOG4CXX_TRACE( dbPollingGovernorLogger, governorType() << " beginning transaction (" << resource << "," << action << ")" );

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
    virtual const std::string& governorType() = 0;

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
            LOG4CXX_DEBUG( dbPollingGovernorLogger, governorType() << " removing old transaction (" << _transactions.begin()->_resource << "," << _transactions.begin()->_action << ")" );
            _transactions.erase(_transactions.begin());
        }
    }

    // check whether the maximum simultaneous transaction limit would be exceeded
    // must be called while holding _mutex lock
    bool maxConcurrentTranExceeded() {
        if (getMaxConcurrentTran() > 0 &&
                _num_current_tran >= getMaxConcurrentTran()) // note: use of getNumCurrentTran() here causes mutex abort
        {
            LOG4CXX_DEBUG( dbPollingGovernorLogger, governorType() << " -- max concurrent transactions");
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
                LOG4CXX_DEBUG( dbPollingGovernorLogger, governorType() << " -- max interval transaction rate");
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
                LOG4CXX_DEBUG( dbPollingGovernorLogger, governorType() << " -- transaction(" << resource << "," << action << ") already in progress");
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


} } // namespace mmcs::server

#endif
