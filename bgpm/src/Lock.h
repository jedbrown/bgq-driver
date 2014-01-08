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

#ifndef _BGPM_LOCK_H_  // Prevent multiple inclusion
#define _BGPM_LOCK_H_

#include <vector>

#include "spi/include/upci/upc_atomic.h"
#include "spi/include/kernel/upc_hw.h"


namespace bgpm {

//! Lock Class
/*!
  A constructor / destructor framework class for maintaining an active lock.
*/
class Lock
{
    public:
        inline Lock(UPC_Lock_t *ipLock)
        : pLock(ipLock) {
            UPC_Lock(pLock);
        }
        inline ~Lock() { UPC_Unlock(pLock); mbar(); }  // mbar - make sure protected operations stored.
    private:
        UPC_Lock_t *pLock;

        // hide these
        Lock();
        Lock(const Lock &);
        Lock & operator=(const Lock &);
};




//! ShMemLock Class
/*!
  A constructor / destructor framework class for maintaining an active lock within shared memory.
  A shared memory lock is distinquished from the normal memory lock in that
  atexit will be used to free the lock if the caller exits before the lock has been freed.

  This should prevent other processes from hanging if they are waiting on the lock.

  If a segfault or other error like that were to occur, there is no cleanup, and other processes
  might still hang.

*/
class ShMemLock; // forward decl for vector below

class ShMemLock
{
    public:
        ShMemLock(UPC_Lock_t *ipLock) : pLock(ipLock) {
            //Lock guard(&listLock);
            UPC_Lock(pLock);
            //AddToCleanup();
            mbar();
        }
        ~ShMemLock() {
            //Lock guard(&listLock);
            //RmvFromCleanup();
            UPC_Unlock(pLock);
            mbar(); // mbar - make sure protected operations stored.
        }
    private:
        UPC_Lock_t *pLock;

#if 0
       // \todo atexit handling is not working properly. Atexit is being called multiple times on different
       //       threads than the one who registered the handler.  That doesn't sound appropriate, but I'm
       //       unsure.  I'm not sure there is a good reason to free shared locks, as an non-zero exit
       //       is supposed to kill all processes on the node.  Thus, I'll just comment out this support
       //       for now and come back to it.


        // cleanupList must be process global (can't do thread initialization)
        // but each thread should only clean up it's own locks.
        // Keep track of tid with lock.
        typedef struct {
            int tid;
            ShMemLock *pLock;
        } ThdLockRec;

        static UPC_Lock_t listLock;
        static __thread bool atExitRegistered;
        static std::vector<ThdLockRec> cleanupList;
        int cleanupIdx;  // this lock's entry in the cleanup list

        static void CleanupOutstandingLocks();
        void AddToCleanup();
        void RmvFromCleanup();
#endif
        // hide these
        ShMemLock();
        ShMemLock(const ShMemLock &);
        ShMemLock & operator=(const ShMemLock &);
};

}

#endif
