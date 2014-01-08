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

#ifndef _MMCSEnvMonitor_H
#define _MMCSEnvMonitor_H

#include "MMCSThread.h"
#include "bgq_util/include/pthreadmutex.h"
//******************************************************************************
// Class for the database monitor thread
// This thread monitors the database for 'allocate' and 'free' requests
//******************************************************************************
class EnvMonitorThread : public MMCSThread
{
public:
    unsigned ncseconds;
    unsigned scseconds;
    unsigned ioseconds;
    unsigned healthseconds;
    unsigned bulkseconds;
    unsigned perfseconds;
    unsigned coolantsecs;
    unsigned opticalsecs;
    EnvMonitorThread() : MMCSThread(), ncseconds(300), scseconds(1800), ioseconds(300),
                         healthseconds(300), bulkseconds(300),
                         perfseconds(300), coolantsecs(300),opticalsecs(3600)  {};
    void* threadStart();
};


//******************************************************************************
// Class for a specific location
//******************************************************************************
class LocationThread : public MMCSThread
{
public:
    LocationThread()
        :  seconds(300)
    {};
    void* threadStart();
    unsigned seconds;
    std::string location;
    std::string type;
};

class LocationThreadList
{
public:
    LocationThreadList() : _locThreadListMutex(PTHREAD_MUTEX_ERRORCHECK_NP) {}
    bool add(LocationThread* loc_thread);
    bool remove(const std::string& location);
    void list(MMCSCommandReply& reply);
private:
    PthreadMutex   _locThreadListMutex;    // to serialize access to the list
    std::vector<LocationThread*>    _locThreads;
};

#endif
