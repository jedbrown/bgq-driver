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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#include <stdlib.h>

#ifndef __HWSTUBS__
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(207, 0) // not defined by glibc
#endif

#include "Lock.h"

using namespace bgpm;

#if 0
UPC_Lock_t ShMemLock::listLock = 0;
__thread bool ShMemLock::atExitRegistered = 0;
std::vector<ShMemLock::ThdLockRec> ShMemLock::cleanupList;



void ShMemLock::CleanupOutstandingLocks()
{
    Lock guard(&listLock);

    int tid = gettid();
    fprintf(stderr, "%s:%d\n", __func__, tid);
    for (unsigned i=0; i<cleanupList.size(); ++i) {
        if ((cleanupList[i].pLock != NULL && cleanupList[i].tid == tid)) {
            cleanupList[i].pLock->~ShMemLock();
            cleanupList[i].pLock = NULL;
            cleanupList[i].tid = -1;
        }
    }
}



void ShMemLock::AddToCleanup()
{
    unsigned i = 0;
    // check for empty slots
    int tid = gettid();
    while ((i<cleanupList.size()) && (cleanupList[i].pLock != NULL)) { ++i; }
    if (i < cleanupList.size()) {
        cleanupList[i].pLock = this;
        cleanupList[i].tid = tid;
    }
    else {
        ThdLockRec rec;
        rec.tid = tid;
        rec.pLock = this;
        cleanupList.push_back(rec);
    }
    cleanupIdx = i;

    if (!atExitRegistered) {
        atExitRegistered = true;
        atexit(CleanupOutstandingLocks);
    }
}




void ShMemLock::RmvFromCleanup()
{
    cleanupList[cleanupIdx].pLock = NULL;
    cleanupList[cleanupIdx].tid = -1;
}

#endif

