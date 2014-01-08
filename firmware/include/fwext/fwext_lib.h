/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef FWEXT_LIB_HEADER
#define FWEXT_LIB_HEADER

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/bqc/PhysicalMap.h>
#include <firmware/include/Firmware.h>
#include <firmware/include/personality.h>
#include <firmware/include/fwext/fwext_consts.h>
#include <firmware/include/fwext/fwext.h>

// inline functions for using the DCR-to-PIB/PCB interface
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/testint_tdr.h>
#include <hwi/include/bqc/testint_inlines.h>

/*!
// back off delay for barriers
*/
__INLINE__ void _Delay_dbnz( uint64_t pclks )
{
    do
    {
        asm volatile("nop;");
    }
    while ( --pclks );
}


/**
 * @brief Puts the host hardware thread to sleep.  There is no way for this thread
 *        to wake itself back up.
 */

__INLINE__ __NORETURN void _Bg_Sleep_Forever()
{
    // if this did not kill the current thread, kill it with this.
    // currently, the kernels sleep forever kill the thread
    #if 1
        // both the bit positions and the bit polarity are reversed from the
        // a2 spec, so this is subject to change when the a2 changess
        uint64_t mask = (1<<(ProcessorThreadID()));
        mtspr(SPRN_TENC,((mask) & 0xf));
        while (1);
    #else
    	Sleep_Forever();
    #endif

}

#define FWEXT_LOCK_COUNT                  (BeDRAM_LOCKNUM_FWEXT_LAST - BeDRAM_LOCKNUM_FWEXT_FIRST + 1)  //!< The number of locks for FW Extensions
#define FWEXT_LOCKNUM_FIRST               (0)                                                           //!< The first (logical) FW Extension lock number
#define FWEXT_LOCKNUM_LAST                (FWEXT_LOCK_COUNT - 3)                                        //!< The last (logical) FW Extension lock number
#define FWEXT_LOCKNUM_NUM_THRD_DONE       (FWEXT_LOCK_COUNT - 2)                                        //!< Interlock for BgGetNumThreads()
#define FWEXT_LOCKNUM_TEST_EXIT_CNT       (FWEXT_LOCK_COUNT - 1)                                        //!< Count of threads exiting via test_exit()

/**
 * @brief Obtains the actual lock index given a logical FW Extension lock index.
 */

extern size_t BgGetRealLockIndex( size_t virtual_index );

/**
 * @brief Barriers using the given lock and number of participants.
 * @param index_value is the logical lock number used for the barrier.
 * @param targetValue is the number of participants in the barrier.  
 */
extern void BgBarrier( size_t index_value, uint64_t targetValue);

/**
 * @brief Gets the number of hardware threads that available on the node (typically
 *        68).
 */
extern unsigned BgGetNumThreads ();


__INLINE__ void BgSetDeratEntry(unsigned entry, uint64_t e0, uint64_t e1)
{
  asm volatile("mtspr %0,%1; eratwe %2,%3,1; eratwe %4,%3,0; isync" : : 
	       "i" (SPRN_MMUCR0), 
	       "r" (MMUCR0_ECL(3) | MMUCR0_TLBSEL_DERAT),
	       "r" (e1),
	       "r" (entry),
	       "r" (e0) :"memory");
}

void Bgmemset(void* addr, char val, int size);

void test_detach(void);
void __NORETURN test_exit(int status);

//! @brief Given a general MMIO address, returns the corresponding user-space MMIO address.
__INLINE__ void* MMIO2Usr(void* addr) {
  return (void*)((unsigned long)addr & ~PHYMAP_PRIVILEGEDOFFSET);
}

//! @brief Given an general MMIO address, returns the corresponding supervisor-space MMIO address.
__INLINE__ void* MMIO2Sup(void* addr) {
  return (void*)((unsigned long)addr | PHYMAP_PRIVILEGEDOFFSET);
}

extern unsigned long fwext_strtoul(const char* s, char** endp, int base);
extern long          fwext_strtol(const char* s, char** endp, int base);
extern int           fwext_strcmp(const char* s1, const char* s2);
extern char*         fwext_getenv(const char*);
extern void          fwext_udelay( uint64_t );

/**
 * @brief Reads stdin (aka "write_con") and copies it into the buffer.
 * @return The number of characters copied or a negative number (error).
 */
extern int           fwext_get(const char* buffer, const unsigned bufferLength );

extern int fwext_termCheck();

extern void Bg_init_mc_ddr();





#endif
