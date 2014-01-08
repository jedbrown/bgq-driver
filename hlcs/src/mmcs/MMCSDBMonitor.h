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

#ifndef MMCS_DB_MONITOR_H
#define MMCS_DB_MONITOR_H

#include "MMCSThread.h"
#include <db/include/api/BGQDBlib.h>

class BlockDBPollingGovernor;

/*!
 * \class DatabaseMonitorThread
 * \brief This thread monitors the database for 'allocate' and 'free' requests
 */
class DatabaseMonitorThread : public MMCSThread
{
public:
    DatabaseMonitorThread() :
        MMCSThread(),
        monitorBlockTable(false)
    {

    }

    bool monitorBlockTable;
    void* threadStart();
};

/*!
 * \class DatabaseBlockCommandThread
 * \brief This thread gets started by the database monitor to process block transactions
 */
class DatabaseBlockCommandThread : public MMCSThread
{
public:
    DatabaseBlockCommandThread() :
        MMCSThread(),
        blockAction(BGQDB::NO_BLOCK_ACTION),
        blockTransactions(NULL)
    {

    }

    void* threadStart();
    std::string blockName;
    std::string userName;
    unsigned int creationId;
    BGQDB::BLOCK_ACTION blockAction;
    BlockDBPollingGovernor* blockTransactions;
};

#endif
