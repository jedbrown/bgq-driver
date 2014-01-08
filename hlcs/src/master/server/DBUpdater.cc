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

#include "DBUpdater.h"

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/TxObject.h>

#include <utility/include/Log.h>


LOG_DECLARE_FILE("master");

DBUpdater::DBUpdater(
        ) :
    _ending(false),
    _ended(false)
{
    // Nothing to do
}

DBUpdater::~DBUpdater()
{
    end();
}

void
DBUpdater::start()
{
    _dbthread = boost::thread(&DBUpdater::runThread, this);
}

void
DBUpdater::runThread()
{
    LOG_TRACE_MSG(__FUNCTION__);
    while (!_ending) {
        std::string rasMessage;
        if (_doq.pickUp(rasMessage)) {
            LOG_DEBUG_MSG("Sending RAS message to database: " << rasMessage);
            BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
            if (!tx.getConnection()) {
                LOG_ERROR_MSG("Unable to connect to database. Cannot put RAS message \"" << rasMessage << "\"");
                return;
            }

            tx.setAutoCommit(true);
            (void)tx.execStmt(rasMessage.c_str());
        }
        if (_doq.get_unread() == 0) { // No more to read...
            boost::unique_lock<boost::mutex> ulock(_notify_lock);
            while (!_ending && _doq.get_unread() == 0) {
                _dropoff_notification.wait(ulock); // so wait for something to be dropped off.
            }
        }
    }
}

void
DBUpdater::addMsg(
        const std::string& msg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    if (_ending) {
        return;
    }
    _doq.dropOff(msg);
    boost::lock_guard<boost::mutex> lg(_notify_lock);
    _dropoff_notification.notify_all();
    LOG_TRACE_MSG("Added following message to drop off queue: " << msg);
}

void
DBUpdater::end()
{
    LOG_TRACE_MSG(__FUNCTION__);
    _ending = true;
    if (!_ended) {
        _dropoff_notification.notify_all();
        _dbthread.join();
    }
    _ended = true;
}
