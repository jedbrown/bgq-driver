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

#ifndef	__LOCK_H
#define	__LOCK_H

/*!
 * \file lock.h
 *
 * \brief C Header file containing SPI L2-atomics-based lock implementation
 *
 */

#include <stdint.h>
#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/bgq_alignment.h>
#include <spi/include/l2/atomic.h>


typedef struct {
  volatile uint64_t ticket;
  volatile uint64_t serving;
} L2_Lock_t ALIGN_QUADWORD;

__INLINE__ void L2_LockInit(L2_Lock_t *l)
{
  l->ticket=0;
  l->serving=0;
}

__INLINE__ void L2_LockAcquireNoSync(L2_Lock_t *l)
{
  uint64_t ticket=L2_AtomicLoadIncrement(&l->ticket);
  while(l->serving != ticket);
}

__INLINE__ void L2_LockAcquire(L2_Lock_t *l)
{
  L2_LockAcquireNoSync(l);
  isync();
}

__INLINE__ void L2_LockReleaseNoSync(L2_Lock_t *l)
{
  L2_AtomicStoreAdd(&l->serving, 1);
}

__INLINE__ void L2_LockRelease(L2_Lock_t *l)
{
  ppc_msync();
  L2_LockReleaseNoSync(l);
}

__INLINE__ uint64_t L2_LockTryAcquireNoSync(L2_Lock_t *l)
{
  int64_t ticket=L2_AtomicLoadIncrementIfEqual(&l->ticket);
  return (ticket >= 0) ? 1 : 0;
}

__INLINE__ uint64_t L2_LockTryAcquire(L2_Lock_t *l)
{
  uint64_t rc=L2_LockTryAcquireNoSync(l);
  isync();
  return rc;
}

__INLINE__ int64_t L2_LockThreadsInLine(L2_Lock_t *l)
{
  int64_t serving=l->serving;
  int64_t ticket=l->ticket;
  return ticket - serving;
}

__INLINE__ uint64_t L2_LockIsBusy(L2_Lock_t *l)
{
  return (L2_LockThreadsInLine(l) > 0) ? 1: 0;
}

#endif /* defined(__LOCK_H) */
