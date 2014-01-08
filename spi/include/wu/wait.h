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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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
#ifndef _WU_WAIT_H_ // Prevent multiple inclusion.
#define _WU_WAIT_H_

/*!
 * \file wakeup.h
 *
 * \brief C Header File containing interfaces for wakeup-unit operations
 *
 */

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#include <sys/types.h>
#include <stdint.h>
#include <spi/include/kernel/location.h>
#include <spi/include/kernel/memory.h>
#include <hwi/include/bqc/wu_mmio.h>
#include <hwi/include/bqc/wu_dcr.h>

/*!
 * \brief Arm the wakeup unit with a specified address and mask
 *
 * \param[in]  physical_address  Address to be monitored.
 * \param[in]  mask              Bitmask specifying the address bits that are
 *                               to participate in the comparison. The
 *                               low-order 6 bits of the address never
 *                               participate, so the minimum address range
 *                               that can be specified is 64 bytes.
 */
__INLINE__ void WU_ArmWithAddress(uint64_t physical_address, uint64_t mask)
{
    // We use the "fast" (cache-inhibited but not guarded) L1P MMIO mapping.
    volatile uint64_t *base = (uint64_t *) (Kernel_FastL1pBaseAddress() +
						WAKEUP_OFFSET_FROM_L1P);
    int thr = Kernel_ProcessorThreadID();
    int reg = thr + Kernel_FirstThreadWakeupWAC();
    base[WAC_BASE(reg)] = physical_address;
    base[WAC_ENABLE(reg)] = mask;
    base[CLEAR_THREAD(thr)] = _BN(reg);
    // Now that we've programmed the unit we need to wait for the programming
    // to take effect.  We do that by reading back one of the unit registers
    // and artificially using the returned value.
    uint64_t tmp = base[CLEAR_THREAD(thr)];	// read back the register
    asm volatile("cmpldi %0,0" : : "r" (tmp));	// "use" the returned value
}

/*!
 * \brief Arm the wakeup unit to fire in response to MU interrupts
 *
 * \param[in]  bits  A 4-bit vector specifying which of the 4 MU subgroups
 *                   are affected.
 */
__INLINE__ void WU_ArmMU(uint64_t bits)
{
    // We use the "fast" (cache-inhibited but not guarded) L1P MMIO mapping.
    volatile uint64_t *base = (uint64_t *) (Kernel_FastL1pBaseAddress() +
						WAKEUP_OFFSET_FROM_L1P);
    int thr = Kernel_PhysicalHWThreadID();
    base[SET_THREAD(thr)] = WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_set(bits);
    // Now that we've programmed the unit we need to wait for the programming
    // to take effect.  We do that by reading back one of the unit registers
    // and artificially using the returned value.
    uint64_t tmp = base[SET_THREAD(thr)];	// read back the register
    asm volatile("cmpldi %0,0" : : "r" (tmp));	// "use" the returned value
}

/*!
 * \brief Disarm the wakeup unit with respect to MU interrupts
 *
 * \param[in]  bits  A 4-bit vector specifying which of the 4 MU subgroups
 *                   are affected.
 */
__INLINE__ void WU_DisarmMU(uint64_t bits)
{
    // We use the "fast" (cache-inhibited but not guarded) L1P MMIO mapping.
    volatile uint64_t *base = (uint64_t *) (Kernel_FastL1pBaseAddress() +
						WAKEUP_OFFSET_FROM_L1P);
    int thr = Kernel_PhysicalHWThreadID();
    base[CLEAR_THREAD(thr)] = WU_DCR__THREAD0_WU_EVENT_CLR__MU_WU_EN_set(bits);
    // Now that we've programmed the unit we need to wait for the programming
    // to take effect.  We do that by reading back one of the unit registers
    // and artificially using the returned value.
    uint64_t tmp = base[CLEAR_THREAD(thr)];	// read back the register
    asm volatile("cmpldi %0,0" : : "r" (tmp));	// "use" the returned value
}

/*!
 * \brief Arm the wakeup unit to fire in response to MU interrupts (system)
 *
 * \param[in]  bits  A 4-bit vector specifying which of the 4 MU subgroups
 *                   are affected. wxyz where w is subgroup 0; x,1;y,2; and
 *                   z,3.
 *             
 */
__INLINE__  void WU_ArmMU_system(uint64_t bits)
{
    // We use the "fast" (cache-inhibited but not guarded) L1P MMIO mapping.
    volatile uint64_t *base = (uint64_t *) WAKEUP_BASE;  //system privileged 
    int thr = Kernel_PhysicalHWThreadID();
    base[SET_THREAD(thr)] = WU_DCR__THREAD0_WU_EVENT_SET__MU_WU_EN_set(bits);
    // Now that we've programmed the unit we need to wait for the programming
    // to take effect.  We do that by reading back one of the unit registers
    // and artificially using the returned value.
    uint64_t tmp = base[SET_THREAD(thr)];	// read back the register
    asm volatile("cmpldi %0,0" : : "r" (tmp));	// "use" the returned value
}
__END_DECLS

#endif // Add nothing below this line.
