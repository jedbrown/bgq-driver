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

#ifndef	_MUDM_LOCK_H_ /* Prevent multiple inclusion */
#define	_MUDM_LOCK_H_


#include <hwi/include/common/bgq_alignment.h>


typedef struct T_BGQ_Atomic
{
    volatile uint32_t atom;
}
ALIGN_L1D_CACHE Lock_Atomic_t;

// Exclusive lock. When obtained by a hwthread, other hwthreads will spin until Kernel_Unlock is called
// by the hwthread that obtained the lock.
//  parm1: doubleword initialized to zero on first use.
void lock(Lock_Atomic_t *pLock);

// Unlock. Should only be issued after a successful Kernel_Lock operation
//  parm1: doubleword used in a previous lock operation
void unlock(Lock_Atomic_t *pLock);


#define SPIN_LOCK(pvar) lock(&pvar)
#define SPIN_UNLOCK(pvar) unlock(&pvar)

#define MSPIN MPRINT("TOLOCK %s():%d\n", __FUNCTION__, __LINE__);
#define MLOCK MPRINT("LOCKED %s():%d\n", __FUNCTION__, __LINE__);
#define MRELS MPRINT("UNLOCK %s():%d\n", __FUNCTION__, __LINE__);
//#define MSPIN_LOCK(pvar) MSPIN; lock(&pvar); MLOCK
//#define MSPIN_UNLOCK(pvar) unlock(&pvar); MRELS
#define MSPIN_LOCK(pvar) SPIN_LOCK(pvar)
#define MSPIN_UNLOCK(pvar) SPIN_UNLOCK(pvar)

__INLINE__ void my_ppc_msync(void) { do { asm volatile ("msync" : : : "memory"); } while(0); }


// Load Reserved: 32bit atom
__INLINE__ uint32_t myLoadReserved32( volatile uint32_t *pVar )
{
   do {
      register uint32_t Val;

      asm volatile ("lwarx   %[rc],0,%[pVar];"
                    : [rc] "=&b" (Val)
                    : [pVar] "b" (pVar)
                    : "memory" );

      return( Val );
      }
      while( 0 );
}

// Store Conditional: 32bit atom
//   Returns: 0 = Conditional Store Failed,
//            1 = Conditional Store Succeeded.
__INLINE__ int myStoreConditional32( volatile uint32_t *pVar, uint32_t Val )
{
   do {
      register int rc = 1; // assume success

      asm volatile ("  stwcx.  %2,0,%1;"
                    "  beq     1f;"       // conditional store succeeded
                    "  li      %0,0;"
                    "1:;"
                    : "=b" (rc)
                    : "b"  (pVar),
                      "b"  (Val),
                      "0"  (rc)
                    : "cc", "memory" );

      return( rc );
      }
      while(0);
}

// Compare and Swap:
//   returns:
//      1 = var equals old value, new value atomically stored in var
//      0 = var does not equal old value, and *pOldValue is set to current value.
__INLINE__ int comp_and_swap( Lock_Atomic_t *var,
                                      uint32_t  *pOldValue,
                                      uint32_t  NewValue )
{
    uint32_t tmp_val;

    do
    {
        tmp_val = myLoadReserved32( &(var->atom) );
        if ( *pOldValue != tmp_val  )
        {
            *pOldValue = tmp_val;
            return( 0 );
        }
    }
    while ( !myStoreConditional32( &(var->atom), NewValue ) );

    return( 1 );
}
#endif //_MUDM_LOCK_H_
