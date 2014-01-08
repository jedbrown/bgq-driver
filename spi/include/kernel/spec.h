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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef	_KERNEL_SPEC_H_ /* Prevent multiple inclusion */
#define	_KERNEL_SPEC_H_

/*!
 * \file memory.h
 *
 * \brief C Header File containing SPI Addressing Inline Functions
 *
 */


#include "kernel_impl.h" 

#include "hwi/include/common/compiler_support.h"
#include "hwi/include/bqc/l2_central_mmio.h"
#include "hwi/include/bqc/l2_dcr.h"

__BEGIN_DECLS

#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

typedef struct SpecConflictSliceInfo_s
{
    uint64_t was_conflictCaptured     : 1;
    uint64_t was_writeAfterRead       : 1;
    uint64_t was_nonSpeculativeAccess : 1;
    uint64_t conflict_vaddress_valid  : 1;
    uint64_t specid_original          : 7;
    uint64_t specid_violating         : 7;
    uint64_t conflict_vaddress;
    uint64_t conflict_paddress;
}
SpecConflictSliceInfo_t;

typedef struct SpecConflictInfo_t
{
    SpecConflictSliceInfo_t l2slice[L2_DCR_num];
}
SpecConflictInfo_t;

typedef struct SpecContext_s
{
      uint64_t priority_key;
      uint64_t atn;
      int32_t  *at;
      uint64_t *lctn;
      uint64_t hwthreadid;
      uint64_t padding[11];
}
SpecContext_t;

typedef struct SpecThread_s
{
      uint64_t specid;
      uint64_t ip;
      uint64_t gpr1;
      uint64_t gpr2;
      uint64_t padding[12];
}
SpecThread_t;

typedef struct SpecState_s
{
      SpecThread_t  hwt_state[68];
      SpecContext_t id_state[128];
}
SpecState_t;

// Conflict register value is not included in the return code if INVALID is set
#define Kernel_SpecReturnCode_INVALID           _BN(50)
#define Kernel_SpecReturnCode_SINGLE            L2C_CONFLICT_SINGLE
#define Kernel_SpecReturnCode_NONSPEC           L2C_CONFLICT_NONSPEC
#define Kernel_SpecReturnCode_MULTI             L2C_CONFLICT_MULTI
#define Kernel_SpecReturnCode_RESOURCE          L2C_CONFLICT_RESOURCE
// TODO@mschaal: what is KEY?
#define Kernel_SpecReturnCode_KEY               _BN(47)
#define Kernel_SpecReturnCode_JMV               _BN(48)
#define Kernel_SpecReturnCode_SEConflict        _BN(51)
#define Kernel_SpecReturnCode_SEConflictVictim  _BN(49)

__INLINE__
uint32_t Kernel_SetSpecContext(size_t specContextSize, const SpecContext_t* specContextBasePtr);

__INLINE__
uint32_t Kernel_SetSpecThreadMap(size_t specThreadSize, const SpecThread_t* specThreadBasePtr);

__INLINE__
uint32_t Kernel_SetSpecState(size_t specStateSize, const SpecState_t* specStatePtr);

__INLINE__
uint32_t Kernel_EnterSpecJailMode(int isLongRunningSpeculation);

__INLINE__
uint32_t Kernel_ExitSpecJailMode(void);

__INLINE__
uint32_t Kernel_AllocateSpecDomain(unsigned int* domain);

__INLINE__
uint32_t Kernel_SetNumSpecDomains(uint32_t new_domain_count);

__INLINE__
uint32_t Kernel_GetNumSpecDomains(uint32_t* current_domain_count);

__INLINE__
uint32_t Kernel_GetNumKernelSpecDomains(uint32_t* kernel_domain_usage);

__INLINE__
uint32_t Kernel_SetSpecDomainMode(unsigned int domain, uint64_t domainmode);

__INLINE__
uint32_t Kernel_SetL2SpecScrubRate(uint64_t scrub_rate);

__INLINE__
uint32_t Kernel_GetSpecConflictInfo(size_t specConflictSize, SpecConflictInfo_t* info);

__INLINE__
uint32_t Kernel_ClearSpecConflictInfo(uint32_t l2slice);

__INLINE__
uint32_t Kernel_EnableFastSpeculationPaths();

__INLINE__
uint32_t Kernel_GetSpeculationIDSelf(uint64_t* specid);

/**
 * \brief Include implementations of the above functions.
 */
#include "spec_impl.h"

__END_DECLS

#endif /* _KERNEL_SPEC_H_ */
