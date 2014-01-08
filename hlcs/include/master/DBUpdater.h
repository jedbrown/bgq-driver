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
#include <queue>
#include <boost/thread.hpp>
#include <boost/detail/atomic_count.hpp>
#include <utility/include/DropoffQueue.h>

#ifndef _DBUPDATER_H
#define _DBUPDATER_H

typedef std::string RasMsgString;

class DBUpdater {
public:
    DBUpdater();
    ~DBUpdater();
    void start();
    void end();
    void addMsg(RasMsgString msg);
private:
    boost::condition_variable _dropoff_notification;
    boost::mutex _notify_lock;
    DropoffQueue<RasMsgString> _doq;
    boost::thread _dbthread;
    bool _ending;
    bool _ended;
    pthread_t _my_tid;
    void runThread();
};

#endif
