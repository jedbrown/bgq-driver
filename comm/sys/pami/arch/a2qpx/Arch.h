/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file arch/a2qpx/Arch.h
 * \brief ???
 */

#ifndef __arch_a2qpx_Arch_h__
#define __arch_a2qpx_Arch_h__

#include "arch/ArchInterface.h"

#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/A2_core.h>




// Here go things specific to this processor architecture

#define NUM_CORES 16
#define NUM_SMT    4

// These are based on what the CNK allows (should be in an os file)
#define PAMI_MAX_PROC_PER_NODE   (NUM_CORES * NUM_SMT)
#define PAMI_MAX_THREAD_PER_PROC (NUM_CORES * NUM_SMT)

#ifdef __cplusplus
///
/// \brief Template specialization for size_t copies.
///
template <>
template <unsigned N>
void Type<size_t>::copy (size_t * dst, size_t * src)
{
  size_t i;
  for (i=0; i<(N/sizeof(size_t)); i++) dst[i] = src[i];

  if (N%(sizeof(size_t)))
  {
    uint8_t * const d = (uint8_t * const) dst[N%(sizeof(size_t))];
    uint8_t * const s = (uint8_t * const) src[N%(sizeof(size_t))];

    for (i=0; i<(N%(sizeof(size_t))); i++) d[i] = s[i];
  }
};
#endif // __cplusplus

#endif // __pami_arch_a2qpx_h__
