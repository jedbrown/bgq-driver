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

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief Atomic Operations
 */
//@}



#include <stdio.h>

#define UPC_ATOMIC_C
#include "spi/include/kernel/location.h"
#include "spi/include/kernel/upci.h"
#include "spi/include/upci/upc_atomic.h"


//! \todo Could we switch to use ppc_waitrsv in operations?
//!       check cnk threading doc further (wait on reservation)

void UPC_Lock(UPC_Lock_t *pLock)
{
    uint64_t lockIndex = Upci_GetLockID();
    uint32_t curValue;

    //mbar();

    do {
        do {
            curValue = LoadReserved32( pLock );
            if (curValue == 0 ) {
                break;
            }
            else if (curValue == lockIndex) {
                UPC_FATAL_ERR("Duplicate UPC_Lock() by thread lockIndex=%ld\n", lockIndex);
                // Terminate(lockIndex);       // terminate test
            }
            else {
                uint64_t savpri = Set_ThreadPriority_Low();
                while ( *pLock ) { // spin till free
                    asm volatile ("nop; nop; nop; nop;");
                }
                Restore_ThreadPriority(savpri);
            }
        } while(1);
    } while (!StoreConditional32(pLock, lockIndex));
    // isync();   // create import barrier (lock seen before subsequent storage accesses)
    ppc_msync();

}




int UPC_Lock_woBlock(UPC_Lock_t *pLock)
{
    uint64_t lockIndex = Upci_GetLockID();
    uint32_t curValue;
    int rc = -1;
    //ppc_msync();

    curValue = LoadReserved32( pLock );
    while ((curValue == 0) && (!StoreConditional32(pLock, lockIndex))) {
        curValue = LoadReserved32( pLock );
    }

    if (curValue == 0) {
        // got the lock
        rc = 0;
        ppc_msync();  // import mem sync - subsequent loads only occur after lock successful.
    }
    else if (curValue == lockIndex) {
        UPC_FATAL_ERR("Duplicate UPC_Lock() by thread lockIndex=%ld\n", lockIndex);
        // Terminate(lockIndex);       // terminate test
    }

    return rc;
}




void UPC_Unlock(UPC_Lock_t *pLock)
{
    ppc_msync();  // import sync - prior store must complete before lock dropped.

    uint32_t lockIndex = (uint32_t)Upci_GetLockID();
    uint32_t curValue = *pLock;
    if (curValue != lockIndex) {
        UPC_FATAL_ERR("Improper UPC_Unlock for lockIndex=%d; curIndex=%d\n", lockIndex, curValue);
        // Terminate(lockIndex);       // terminate test
    }
    else {
        *pLock = 0;
    }
   // mbar();
}







// Assumes target number of threads is consistent by all threads, so no one thread needs to be the master.
short UPC_Barrier(UPC_Barrier_t *pLock, short num_threads, uint64_t timeout)
{
    uint64_t targ_num_threads = num_threads;
    uint64_t barr_timeout;  // Timeout hardcoded in cycles
    if (timeout == 0) timeout = 200000;
    barr_timeout = GetTimeBase() + timeout;

    ppc_msync();  // export mem sync

    // store initial value
    uint64_t curValue;
    curValue = LoadReserved( pLock );
    while (curValue == 0) {
        curValue = targ_num_threads;
        StoreConditional(pLock, curValue);  // let 1st writer win - so don't care if fails.
        curValue = LoadReserved( pLock );
    }

    // now atomically subtract 1 for this thread.
    do {
        curValue = LoadReserved( pLock );
        curValue--;
    } while (!StoreConditional( pLock, curValue));

    // now wait till value reaches zero
    uint64_t savpri = Set_ThreadPriority_Low();
    while (*pLock > 0) {
        if (GetTimeBase() > barr_timeout) {
            UPC_CRITICAL_WARNING("Timeout(2) on barr_target of 0x%016lx; cur target=0x%016lx.\n", targ_num_threads, curValue);
            break;       // end barrier
        };
    }
    Restore_ThreadPriority(savpri);

    ppc_msync();  // import mem sync

    return (*pLock);
}






short LeaderLatch(UPC_Barrier_t *pLock, volatile uint64_t *pstatus, uint64_t mask)
{
    uint64_t curLock;

    // check if any other thread has the lock yet.
    curLock = LoadReserved( pLock );
    while (curLock == 0) {
        // quit if status is inactive
        if ((*pstatus & mask) == 0) {
            return LLATCH_CONTINUE;
        }

        // otherwise store threads index +1 as lock value.
        curLock = Kernel_PhysicalHWThreadIndex();
        curLock += 1;
        if ( StoreConditional( pLock, curLock )) {
            ppc_msync();  // export mem sync
            return LLATCH_LEADER;
        }

        // try again.
        curLock = LoadReserved( pLock );
    }

    // otherwise block on curLock
    uint64_t savpri = Set_ThreadPriority_Low();
    while ( *pLock ) {
        asm volatile ("nop; nop; nop; nop;");
    }
    Restore_ThreadPriority(savpri);
    ppc_msync();  // export mem sync for everyone else
    return LLATCH_CONTINUE;
}



short LeaderLatch2(UPC_Barrier_t *pLock, volatile uint64_t *pstatus, volatile uint64_t *pstatus2)
{
    uint64_t curLock;


    // check if any other thread has the lock yet.
    curLock = LoadReserved( pLock );
    while (curLock == 0) {
        // quit if status is inactive
        if ((*pstatus || *pstatus2) == 0) {
            return LLATCH_CONTINUE;
        }

        // otherwise store threads index +1 as lock value.
        curLock = Kernel_PhysicalHWThreadIndex();
        curLock += 1;
        if ( StoreConditional( pLock, curLock )) {
            ppc_msync();  // export mem sync
            return LLATCH_LEADER;
        }

        // try again.
        curLock = LoadReserved( pLock );
    }

    // otherwise block on curLock
    uint64_t savpri = Set_ThreadPriority_Low();
    while ( *pLock ) {
        asm volatile ("nop; nop; nop; nop;");
    }
    Restore_ThreadPriority(savpri);
    ppc_msync();  // export mem sync for everyone else
    return LLATCH_CONTINUE;
}




void LeaderUnLatch(UPC_Barrier_t *pLock)
{
    ppc_msync();  // import mem sync

    // check if any other thread has the lock yet.
    do {
        LoadReserved( pLock );
    } while (!StoreConditional( pLock, 0 ) );
}
