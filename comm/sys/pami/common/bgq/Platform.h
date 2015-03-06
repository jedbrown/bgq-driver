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

///
/// \file common/bgq/Platform.h
/// \brief ???
///

#ifndef __common_bgq_Platform_h__
#define __common_bgq_Platform_h__

#include "Arch.h"
#include "hwi/include/bqc/A2_core.h"

/// \brief Constant used to specify alignment in declarations of lwarx/stwcx atomics.
#define PAMI_NATIVE_ATOMIC_ALIGN	L1D_CACHE_LINE_SIZE

// Enable multi-context/endpoint PGAS/CCMI collective registration.  BGQ Optimized collectives are context 0 only.
 #define PAMI_ENABLE_COLLECTIVE_MULTICONTEXT

// Enable BGQ shmem stack on sub-node geometries
//#define PAMI_ENABLE_SHMEM_SUBNODE

// Enable experimental (X0) collective protocols
//#define PAMI_ENABLE_X0_PROTOCOLS

#define PAMI_ALGOLISTS_MAX_NUM 32 // Maximum number of algorithms per collective per geometry

#define SHM_USE_COLLECTIVE_FIFO // Enable shared memory collectives

/** \todo set the client information in the endpoint opaque type */
#define PAMI_ENDPOINT_INIT(client,task,offset) ((offset << 23) | task)
#define PAMI_ENDPOINT_INFO(endpoint,task,offset) { task = endpoint & 0x007fffff; offset = (endpoint >> 23) & 0x03f; }

#define MAX_CONTEXTS        (64)

#define PAMI_PLATFORM_A2A_PACING_WINDOW __global._a2a_pacing_window

#define PLATFORM_TID Kernel_ProcessorID()

#endif // __common_bgq_platform_h__
