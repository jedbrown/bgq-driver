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
#ifndef SHARED_CRITICAL_H
#define SHARED_CRITICAL_H


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


#include "hwi/include/common/compiler_support.h"
#include "../test_utils.h"

__BEGIN_DECLS

//
// How to use....
//
// use to create cross process barriers and synchronization for testing.
// The methods look like this:  (tests/diags/collisions test uses these methods heavily if you want to see examples)
//
// In main to set it up:
//        InitCriticalLock();
//        InitShmBarrier1(numProcs);              // creates barrier usable across for all processes on the node
//        ShmBarrier1();                          // perform the process barrier
//        InitShmBarrier2(numProcs*numThreads);   // creates barrier for use across all threads on the node
//
//    or use
//        SHMC_Init_Barriers(numProcs, numThreads)
//
//
// In your test execution code
//        #pragma omp parallel default(none) num_threads(numThreads) shared(stderr,numFailures,numProcs,numThreads)
//        {
//            ShmBarrier2();   // use barrier across all processes and threads
//
//            // to cause operations across all processes and threads to serialize in a incremental fashion:
//            InitShmCount(0);   // tell it to start with Processor ID 0 (thread 0)
//            ShmBarrier2();     // barrier all threads.
//            ShmAtomicWait(Kernel_ProcessorID());  // wait until the current thread id comes up.
//            ... do your test action - very useful for printing information
//            ShmAtomicIncr();   // current thread increments value.
//            ShmBarrier2();     // each thread wait until all threads have performed the action.
//
//         or use
//
//            SHMC_ProcBarrier();   or
//            SHMC_ThreadBarrier();   or
//
//            SHMC_SERIAL_Start(numProcs,numThreads);   Assumes numProcs and numThreads values are always a power of 2
//            ... do your test action - very useful for printing information
//            SHMC_SERIAL_End();



#define SHMC_Init_Barriers(numProcs, numThreads)  \
        InitCriticalLock();        \
        InitShmBarrier1(numProcs); \
        ShmBarrier1();             \
        InitShmBarrier2(numProcs*numThreads);

#define SHMC_SERIAL_Start(numProcs,numThreads) \
        { \
        InitShmCount(0); \
        ShmBarrier2(); \
        int totThds = numProcs*numThreads; \
        int targ = Kernel_ProcessorID(); \
        if (totThds >= 16) { targ /= (64 / totThds); } \
        else { targ /= 4; } \
        ShmAtomicWait(Kernel_ProcessorID()/(64/(numProcs*numThreads))); \
        }

#define SHMC_SERIAL_End() \
        ShmAtomicIncr(); \
        ShmBarrier2();

#define SHMC_ProcBarrier()    ShmBarrier1()
#define SHMC_ThreadBarrier()  ShmBarrier2()





#define SHM_CRITICAL_FILE "shm_test_critical"
typedef struct SharedCriticalArea_t {
    pthread_mutex_t   mutex;
    volatile int      procCount;
    pthread_barrier_t barr1;
    volatile int      barr1Count;
    pthread_barrier_t barr2;
    volatile int      barr2Count;
    volatile int      val_count;
} ShmCritLock_t;

#define SHARED_CRITICAL_ERRBUF_LEN 256
#ifdef SHARED_CRITICAL_C
ShmCritLock_t *pShmCritLock;
int shmCritFd;
__thread char errBuf[SHARED_CRITICAL_ERRBUF_LEN];
#else
extern ShmCritLock_t *pShmCritLock;
extern int shmCritFd;
extern __thread char *errBuf;
#endif



__INLINE__ void ShmCriticalLock()
{
    int rc = pthread_mutex_lock(&(pShmCritLock->mutex));
    if (rc) {
        int err = errno;
        strerror_r(err, errBuf, SHARED_CRITICAL_ERRBUF_LEN);
        DIE("pthread_mutex_lock() failed, errno=%d; %s\n", errno, errBuf);
    }
}

__INLINE__ void ShmCriticalUnlock()
{
    int rc = pthread_mutex_unlock(&(pShmCritLock->mutex));
    if (rc) {
        int err = errno;
        strerror_r(err, errBuf, SHARED_CRITICAL_ERRBUF_LEN);
        DIE("pthread_mutex_unlock() failed, errno=%d; %s\n", err, errBuf);
    }
}


__INLINE__ void ShmBarrier1()
{
    int rc = pthread_barrier_wait(&(pShmCritLock->barr1));
    if ((rc != 0) && (rc != PTHREAD_BARRIER_SERIAL_THREAD)) {
        int err = errno;
        strerror_r(err, errBuf, SHARED_CRITICAL_ERRBUF_LEN);
        DIE("pthread_mutex_wait - could not wait on barrier, errno=%d; %s\n", err, errBuf);
    }
}

__INLINE__ void ShmBarrier2()
{
    int rc = pthread_barrier_wait(&(pShmCritLock->barr2));
    if ((rc != 0) && (rc != PTHREAD_BARRIER_SERIAL_THREAD)) {
        int err = errno;
        strerror_r(err, errBuf, SHARED_CRITICAL_ERRBUF_LEN);
        DIE("pthread_mutex_wait - could not wait on barrier, errno=%d; %s\n", err, errBuf);
    }
}




void InitCriticalLock();
void InitShmBarrier1(int threads);
void InitShmBarrier2(int threads);

__INLINE__ void InitShmCount(int val) {
    ShmCriticalLock();
    pShmCritLock->val_count = val;
    ShmCriticalUnlock();
}

__INLINE__ int ShmAtomicIncr() {
    ShmCriticalLock();
    pShmCritLock->val_count++;
    int val = pShmCritLock->val_count;
    ShmCriticalUnlock();
    return val;
}

__INLINE__ void ShmAtomicWait(int targ) {
    int curValue;
    uint64_t startPriority = mfspr(SPRN_PPR32);
    while ((curValue = pShmCritLock->val_count) < targ) {
        Upci_Delay(5);
    }
    Restore_ThreadPriority(startPriority);
}

__END_DECLS


#endif
