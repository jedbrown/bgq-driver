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

#ifndef MASTER_DB_UPDATER_H_
#define MASTER_DB_UPDATER_H_


#include "DropoffQueue.h"

#include <boost/thread.hpp>

#include <string>


class DBUpdater
{
public:
    DBUpdater();
    ~DBUpdater();
    void start();
    void end();
    void addMsg(const std::string& msg);
private:
    boost::condition_variable _dropoff_notification;
    boost::mutex _notify_lock;
    DropoffQueue<std::string> _doq;
    boost::thread _dbthread;
    bool _ending;
    bool _ended;
    void runThread();
};

#endif
