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

#ifndef __L2_UTIL_H__
#define __L2_UTIL_H__

__BEGIN_DECLS

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/bgq_alignment.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/l1p_mmio.h>
#include <hwi/include/bqc/l2_dcr.h>

__INLINE__ void l2_lock_range(void *address, uint64_t bytes)
{
    uint64_t a;
    for (a = 0; a < bytes; a += L2_CACHE_LINE_SIZE) {
	DCBTLS_CT(((uint64_t) address) + a, 2);
    }
}

__INLINE__ void l2_unlock_range(void *address, uint64_t bytes)
{
    uint64_t a;
    for (a = 0; a < bytes; a += L2_CACHE_LINE_SIZE) {
	DCBLC_CT(((uint64_t) address) + a, 2);
    }
}

//
// NOTE: The supplied address must be mapped to a contiguous physical
//       region 2 megabytes in size and aligned on a 2-megabyte boundary.
//
__INLINE__ void l2_unlock_all_with_address(void *address)
{
    // We clear all locks by configuring the l1p to convert lock clears to a
    // ttype that clears the whole set, and then issuing a clear to each set.
    uint64_t old_cfg = in64((uint64_t *) L1P_CFG_PF_SYS);
    uint64_t new_cfg = (old_cfg & ~L1P_CFG_PF_SYS_lock_l1_only) |
					    L1P_CFG_PF_SYS_lock_l2_set_lc;
    out64_sync((uint64_t *) L1P_CFG_PF_SYS, new_cfg);

    // This loop will hit each L2 set as long as address is mapped to a
    // 2MB-sized and 2MB-aligned contiguous physical region.  We don't check.
    uint64_t a;
    for (a = 0; a < (2*1024*1024); a += L2_CACHE_LINE_SIZE) {
	DCBLC_CT(((uint64_t) address) + a, 2);
    }

    out64_sync((uint64_t *) L1P_CFG_PF_SYS, old_cfg);
}

__INLINE__ void l2_unlock_all(void)
{
    // Use address 0 by default.
    l2_unlock_all_with_address((void *) 0);
}

__INLINE__ void l2_set_overlock_threshold(uint64_t threshold)
{
    uint64_t i;
    for (i = 0; i < L2_DCR_num; i++) {
	L2_DCR__LRUCTRL1__OVERLOCK_THRESH_insert(L2_DCR_PRIV_PTR(i)->lructrl1,
						 threshold);
    }
}

__INLINE__ void l2_set_spec_threshold(uint64_t threshold)
{
    uint64_t i;
    for (i = 0; i < L2_DCR_num; i++) {
        L2_DCR__LRUCTRL1__SPEC_THRESH_insert(L2_DCR_PRIV_PTR(i)->lructrl1, 
                                             threshold);
    }
}

__INLINE__ void l2_set_prefetch_enables(uint64_t enable)
{
    uint64_t i;
    for (i=0; i<16; i++)
    {
        uint64_t data64 = DCRReadPriv(L2_DCR(i, CTRL));
        if(enable)
            data64 |= L2_DCR__CTRL__ENABLE_PRFT_set(1);
        else
            data64 &= ~(L2_DCR__CTRL__ENABLE_PRFT_set(1));
        DCRWritePriv(L2_DCR(i, CTRL), data64);
    }
}


__INLINE__ void l2_l1pcfg_lock_l1_only(uint64_t l1_only)
{
    uint64_t cfg_pf_sys = in64((uint64_t *) L1P_CFG_PF_SYS);
    cfg_pf_sys &= ~L1P_CFG_PF_SYS_lock_l1_only;
    cfg_pf_sys |= (l1_only ? L1P_CFG_PF_SYS_lock_l1_only : 0);
    out64_sync((uint64_t *) L1P_CFG_PF_SYS, cfg_pf_sys);
}

__INLINE__ void l2_l1pcfg_clear_locks_in_set(uint64_t l2_set_lc)
{
    uint64_t cfg_pf_sys = in64((uint64_t *) L1P_CFG_PF_SYS);
    cfg_pf_sys &= ~L1P_CFG_PF_SYS_lock_l2_set_lc;
    cfg_pf_sys |= (l2_set_lc ? L1P_CFG_PF_SYS_lock_l2_set_lc : 0);
    out64_sync((uint64_t *) L1P_CFG_PF_SYS, cfg_pf_sys);
}

__END_DECLS

#endif
