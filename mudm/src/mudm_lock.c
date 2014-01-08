/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

#include "mudm_lock.h"
//extracted code from hwi to use in cnk and linux
//#include <hwi/include/bqc/A2_inlines.h>
//#include <hwi/include/bqc/A2_core.h>


// Warning: Most SPRs are privileged.
// Note: Must be a define because SPRN is encoded in the instruction.
#ifndef mfspr
#define SPRN_PIR                  (0x11E)          // Processor ID Register (See GPIR)             Hypervisor Read-only
#define mfspr( SPRN )\
({\
   uint64_t tmp;\
   do {\
      asm volatile ("mfspr %0,%1" : "=&r" (tmp) : "i" (SPRN) : "memory" );\
      }\
      while(0);\
   tmp;\
})
#endif

// read my Thread Index (0 to 67) from SPRN_PIR
__INLINE__ int ProcessorID( void )
{
   uint64_t pir = mfspr( SPRN_PIR );

   return( (int)(pir & 0x3FF) );
}

void lock(Lock_Atomic_t *pLock) 
{
    uint64_t my_unique_index = ProcessorID() + 1; 
    //uint64_t my_unique_index = 1;
    uint32_t freeValue = 0;
    // try to obtain the lock
    while (1)
    {
        if (comp_and_swap(pLock,             // lockword
                             &freeValue,        // value is zero when lock is not held
                             my_unique_index )) // store our thread_index+1 to indicate we own the lock
        {
            break; // break out of the the while loop. We now own the lock
        }
        else
        {
            // Spin at a low priority until lock is release and then try again
            //ThreadPriority_Low();
            while (pLock->atom) { }
            //ThreadPriority_High(); // Use high priority while we are holding the kernel lock
            freeValue = 0;
        }
    }
}
void unlock(Lock_Atomic_t *pLock) 
{
    my_ppc_msync();
    // Consider checking that we own this lock before resetting it. This test may not be necessary if we can trust ourself!
    // reset the lock
    pLock->atom = 0;
    // Move our priority back down to normal. We were running with a high priority while holding the kernel lock
    //ThreadPriority_Medium(); // Use high priority while we are holding the kernel lock

}
