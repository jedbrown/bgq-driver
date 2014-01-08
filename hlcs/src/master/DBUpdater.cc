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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include <utility/include/Log.h>
#include <api/tableapi/DBConnectionPool.h>
#include <api/tableapi/TxObject.h>
#include "DBUpdater.h"
#include "Thread.h"

LOG_DECLARE_FILE("master");

DBUpdater::DBUpdater() : _ending(false), _ended(false), _my_tid(0) {
}

DBUpdater::~DBUpdater() {
    end();
}

void DBUpdater::start() {
    _dbthread = boost::thread(&DBUpdater::runThread, this);
}

void DBUpdater::runThread() {
    LOG_INFO_MSG(__FUNCTION__);
    ThreadLog tl("DB Update Thread");
    _my_tid = pthread_self();
    while(!_ending) {
        RasMsgString rasMessage;
        if(_doq.pickUp(rasMessage) == true) {
            int tx_rc = 0;
            LOG_DEBUG_MSG("Sending RAS request to database: " << rasMessage);
            BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
            if (!tx.getConnection()) {
                LOG_ERROR_MSG("Unable to connect to database.  Cannot put RAS message \"" 
                              << rasMessage << "\"");
                return;
            }
            
            tx.setAutoCommit(true);
            tx_rc = tx.execStmt(rasMessage.c_str());
        }
        if(_doq.get_unread() == 0) { // No more to read...
            boost::unique_lock<boost::mutex> ulock(_notify_lock);
            while(!_ending && _doq.get_unread() == 0) {
                _dropoff_notification.wait(ulock); // so wait for something to be dropped off.
            }
        }
    }
}

void DBUpdater::addMsg(RasMsgString msg) {
    LOG_DEBUG_MSG(__FUNCTION__);
    if(_ending) return;
    _doq.dropOff(msg);
    boost::lock_guard<boost::mutex> lg(_notify_lock);
    _dropoff_notification.notify_all();
    LOG_DEBUG_MSG("Added " << msg << " to DoQ");
}

void DBUpdater::end() { 
    LOG_DEBUG_MSG(__FUNCTION__);
    _ending = true; 
    if(!_ended) {
        _dropoff_notification.notify_all();
        _dbthread.join();
    }
    _ended = true;
}
