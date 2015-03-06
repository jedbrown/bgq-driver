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
 * \file arch/ppc450d/Arch.h
 * \brief ???
 */

#ifndef __arch_ppc450d_Arch_h__
#define __arch_ppc450d_Arch_h__

#include "arch/ArchInterface.h"

// Here go things specific to this processor architecture

#define NUM_CORES 4
#define NUM_SMT   1

// These are based on what the CNK allows
#define PAMI_MAX_PROC_PER_NODE   (NUM_CORES * NUM_SMT)
#define PAMI_MAX_THREAD_PER_PROC (NUM_CORES * NUM_SMT)

#include <bpcore/ppc450_inlines.h>
#include <bpcore/bgp_atomic_ops.h>
#include <spi/kernel_interface.h>

#define LQU(x, ptr, incr) \
asm volatile ("lfpdux %0,%1,%2" : "=f"(x), "+Ob"(ptr) : "r"(incr) : "memory")
#define SQU(x, ptr, incr) \
asm volatile ("stfpdux %2,%0,%1": "+Ob" (ptr) : "r" (incr), "f" (x) : "memory")


#endif // __pami_arch_ppc450d_h__
