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
#ifndef _L1P_STREAM_H_ // Prevent multiple inclusion.
#define _L1P_STREAM_H_

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#include <sys/types.h>
#include <stdint.h>
#include <spi/include/kernel/location.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/l1p/types.h>
#include <hwi/include/bqc/l1p_mmio.h>

#define L1P_CFG_PF_USR_ADJUST (L1P_CFG_PF_USR - PHYMAP_MINADDR_L1P - PHYMAP_PRIVILEGEDOFFSET)

/*!
 \brief Returns enable/disable status of the linear stream prefetcher's adaptation mode
 */
__INLINE__ int L1P_GetStreamAdaptiveMode(int* adaptiveState)
{
    *adaptiveState = _G1(59, *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)));
    return 0;
}

/*!
 \brief Enables or disables the linear stream prefetcher's depth adaptation mode
 */
__INLINE__ int L1P_SetStreamAdaptiveMode(int enable)
{
    if(enable)
       *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) |= L1P_CFG_PF_USR_pf_adaptive_enable;
    else
       *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) &= ~L1P_CFG_PF_USR_pf_adaptive_enable;
    return 0;
}

/*!
 \brief Returns the linear stream prefetch policy in the specified pointer.  
 The policy controls when a stream is established
 */
__INLINE__ int L1P_GetStreamPolicy(L1P_StreamPolicy_t* policy)
{
    uint64_t tmp = *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST));
    if((tmp & L1P_CFG_PF_USR_pf_stream_prefetch_enable) == 0)
        *policy = L1P_stream_disable;
    else if(tmp & L1P_CFG_PF_USR_pf_stream_optimistic)
        *policy = L1P_stream_optimistic;
    else if(tmp & L1P_CFG_PF_USR_pf_stream_est_on_dcbt)
        *policy = L1P_confirmed_or_dcbt;
    else 
        *policy = L1P_stream_confirmed;
    return 0;
}

/*!
 \brief Changes the linear stream prefetch policy.  
 The policy controls when a stream is established
 */
__INLINE__ int L1P_SetStreamPolicy(L1P_StreamPolicy_t policy)
{
    // \todo Check with L1p team to verify that setting multiple bits is a programming error.
    switch(policy)
    {
        case L1P_stream_disable:
            *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) = (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & ~(L1P_CFG_PF_USR_pf_stream_prefetch_enable | L1P_CFG_PF_USR_pf_stream_establish_enable));
            break;
        case L1P_stream_optimistic:
            *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) = (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & ~(L1P_CFG_PF_USR_pf_stream_optimistic | L1P_CFG_PF_USR_pf_stream_est_on_dcbt)) | L1P_CFG_PF_USR_pf_stream_optimistic | L1P_CFG_PF_USR_pf_stream_prefetch_enable | L1P_CFG_PF_USR_pf_stream_establish_enable;
            break;
        case L1P_stream_confirmed:
            *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) = (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & ~(L1P_CFG_PF_USR_pf_stream_optimistic | L1P_CFG_PF_USR_pf_stream_est_on_dcbt)) | L1P_CFG_PF_USR_pf_stream_prefetch_enable | L1P_CFG_PF_USR_pf_stream_establish_enable;
            break;
        case L1P_confirmed_or_dcbt:
            *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) = (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & ~(L1P_CFG_PF_USR_pf_stream_optimistic | L1P_CFG_PF_USR_pf_stream_est_on_dcbt)) | L1P_CFG_PF_USR_pf_stream_est_on_dcbt | L1P_CFG_PF_USR_pf_stream_prefetch_enable | L1P_CFG_PF_USR_pf_stream_establish_enable;
            break;
        default:
            return L1P_PARMRANGE;
    }
    return 0;
}

/*!
 \brief Returns the default stream depth when a new stream has been created.  
 This default depth may be modified on a per stream basis via the adaptive mode (if enabled).
 */
__INLINE__ int L1P_GetStreamDepth(uint32_t* depth)
{
    *depth = ((*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & L1P_CFG_PF_USR_dfetch_depth(-1)) >> (63-42)) + 1;
    return 0;
}

/*!
 \brief Set the initial target prefetch depth
 When a new stream is established, the stream is set to the initial target prefetch depth specified by L1P_SetStreamDepth().  A streams prefetch depth may subsequently vary if 
 the adaptive prefetch mode is enabled.
 */
__INLINE__ int L1P_SetStreamDepth(uint32_t depth)
{
    if(depth > 8) 
        return L1P_PARMRANGE;
    if(depth == 0)
        return L1P_PARMRANGE;
    
    depth -= 1;
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) = (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & ~L1P_CFG_PF_USR_dfetch_depth(-1)) | L1P_CFG_PF_USR_dfetch_depth(depth);
    return 0;
}

/*!
 \brief Gets the number of 128-byte cachelines that can be used by the linear stream prefetcher. 
 Unallocated lines will be used by the perfect prefetcher.  This will help avoid thrashing between the prefetch algorithms
 */
__INLINE__ int L1P_GetStreamTotalDepth(uint32_t* depth)
{
    *depth = ((*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & L1P_CFG_PF_USR_dfetch_max_footprint(-1)) >> (63-46)) + 1;
    return 0;
}

/*!
 \brief Sets the number of 128-byte cachelines that can be used by the linear stream prefetcher.  
 The unallocated lines will continue to be used by the perfect prefetcher, this will help avoid thrashing between the prefetch algorithms
 */
__INLINE__ int L1P_SetStreamTotalDepth(uint32_t depth)
{
    if(depth > 16)
        return L1P_PARMRANGE;
    if(depth < 1)
        return L1P_PARMRANGE;
    depth--;
    *((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) = (*((uint64_t*)(Kernel_L1pBaseAddress() + L1P_CFG_PF_USR_ADJUST)) & ~L1P_CFG_PF_USR_dfetch_max_footprint(-1)) | L1P_CFG_PF_USR_dfetch_max_footprint(depth);
    return 0;
}

__END_DECLS

#endif // Add nothing below this line.
