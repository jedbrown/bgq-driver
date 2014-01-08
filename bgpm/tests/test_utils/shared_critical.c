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

#include "spi/include/kernel/location.h"
#include "spi/include/upci/upc_atomic.h"

#define SHARED_CRITICAL_C
#include "shared_critical.h"



void InitCriticalLock()
{
    char* pShm = NULL;
    shmCritFd = shm_open(SHM_CRITICAL_FILE, O_RDWR | O_CREAT, 0600);
    ftruncate(shmCritFd, sizeof(ShmCritLock_t));
    pShm = (char*) mmap(NULL, sizeof(ShmCritLock_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmCritFd, 0);
    if (pShm == MAP_FAILED) {
        DIE("mmap() failed, errno=%d\n", errno);
    }
    pShmCritLock = (ShmCritLock_t *)pShm;

    // Let only process thread on core 0:0 init the shared mem.  Others wait.
    if (Kernel_ProcessorID() == 0) {
        pthread_mutexattr_t mutexattr_shared;
        pthread_mutexattr_init(&mutexattr_shared);
        pthread_mutexattr_setpshared (&mutexattr_shared, 1);
        pthread_mutex_init( &(pShmCritLock->mutex), &mutexattr_shared );
        int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
        if (rc) {
            DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        pShmCritLock->procCount++;
        pthread_mutex_unlock(&(pShmCritLock->mutex));
    }
    else {
        if (!pShmCritLock->procCount) {
           uint64_t pri = Set_ThreadPriority_Low();
           while (!(pShmCritLock->procCount)) { }
           Restore_ThreadPriority(pri);
        }
        int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
        if (rc) {
            DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        pShmCritLock->procCount++;
        pthread_mutex_unlock(&(pShmCritLock->mutex));
    }
}


void InitShmBarrier1(int threads) {
    if (Kernel_ProcessorID() == 0) {
        pthread_barrierattr_t barrattr_shared;
        pthread_barrierattr_init(&barrattr_shared);
        pthread_barrierattr_setpshared (&barrattr_shared, 1);
        if (pthread_barrier_init(&(pShmCritLock->barr1), &barrattr_shared, threads)) {
            DIE("pthread_barrier_init() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
        if (rc) {
            DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        pShmCritLock->barr1Count++;
        pthread_mutex_unlock(&(pShmCritLock->mutex));
    }
    else {
        if (!pShmCritLock->barr1Count) {
           uint64_t pri = Set_ThreadPriority_Low();
           while (!(pShmCritLock->barr1Count)) { }
           Restore_ThreadPriority(pri);
        }
        int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
        if (rc) {
            DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        pShmCritLock->barr1Count++;
        pthread_mutex_unlock(&(pShmCritLock->mutex));
    }
}


void InitShmBarrier2(int threads) {
    if (Kernel_ProcessorID() == 0) {
        pthread_barrierattr_t barrattr_shared;
        pthread_barrierattr_init(&barrattr_shared);
        pthread_barrierattr_setpshared (&barrattr_shared, 1);
        if (pthread_barrier_init(&(pShmCritLock->barr2), &barrattr_shared, threads)) {
            DIE("pthread_barrier_init() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
        if (rc) {
            DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        pShmCritLock->barr2Count++;
        pthread_mutex_unlock(&(pShmCritLock->mutex));
    }
    else {
        if (!pShmCritLock->barr2Count) {
           uint64_t pri = Set_ThreadPriority_Low();
           while (!(pShmCritLock->barr2Count)) { }
           Restore_ThreadPriority(pri);
        }
        int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
        if (rc) {
            DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, strerror(errno));
        }
        pShmCritLock->barr2Count++;
        pthread_mutex_unlock(&(pShmCritLock->mutex));
    }
}

