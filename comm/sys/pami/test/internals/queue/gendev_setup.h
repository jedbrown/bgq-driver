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
 * \file test/internals/queue/gendev_setup.h
 * \brief ???
 */

#ifndef __test_internals_queue_gendev_setup_h__
#define __test_internals_queue_gendev_setup_h__

// A cheat to override GenericDeviceWorkQueue, etc...
#include "GenericDevicePlatform.h" // prevent later inclusion...
#include "Topology.h"
#include "Global.h"
#undef USE_COMMTHREADS

#if defined(GCCSAFE) && !defined(QUEUE_NAME)
#define QUEUE_NAME	"GccThreadSafeQueue<GccIndirCounter>"
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/native/NativeCounter.h"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue	PAMI::GccThreadSafePostingQueue<\
		PAMI::MutexedQueue<PAMI::Mutex::Counter<PAMI::Counter::Native> >\
		>
#endif // GCCSAFE

#if defined(CMPSAFE) && !defined(QUEUE_NAME)
#include "util/queue/GccCmpSwapQueue.h"
#define QUEUE_NAME	"GccThreadSafeQueue<GccCmpSwap>"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue	PAMI::GccThreadSafePostingQueue<PAMI::GccCmpSwapQueue> >
#endif // CMPSAFE

#if defined(L2MUTEX) && !defined(QUEUE_NAME)
#include "components/atomic/bgq/L2Mutex.h"
#define QUEUE_NAME	"MutexedQueue<L2IndirMutex>"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue	PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<PAMI::Mutex::BGQ::IndirectL2> >
#endif // L2MUTEX

#if defined(ARRAYBASED) && !defined(QUEUE_NAME)
#include "components/atomic/bgq/L2Mutex.h"
#include "util/queue/bgq/ArrayBasedQueue.h"
#define QUEUE_NAME	"ArrayBasedQueue<L2ProcMutex>"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue PAMI::ArrayBasedPostingQueue<PAMI::Mutex::BGQ::L2ProcMutex> >
#endif // ARRAYBASED

#if defined(LBXMUTEX) && !defined(QUEUE_NAME)
#include "components/atomic/bgp/LockBoxMutex.h"
#define QUEUE_NAME	"MutexedQueue<LockBoxMutex>"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue	PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<PAMI::Mutex::BGP::LockBoxMutex> >
#endif // LBXMUTEX

#if defined(GCCMUTEX) && !defined(QUEUE_NAME)
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/native/NativeCounter.h"
#define QUEUE_NAME	"MutexedQueue<Mutex::Counter<Counter::Native> >"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue	PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<\
		PAMI::Mutex::Counter<PAMI::Counter::Native>\
		>
#endif // GCCMUTEX

#ifdef __pami_target_bgq__

#include "components/atomic/bgq/L2Mutex.h"
typedef PAMI::Mutex::BGQ::IndirectL2 GenericDeviceMutex;
#define MUTEX_NAME	"IndirectL2"

#else // !__pami_target_bgq__
#ifdef __GNUC__

#include "components/atomic/native/NativeMutex.h"
typedef PAMI::Mutex::Native GenericDeviceMutex;
#define MUTEX_NAME	"GccNative"

#else /* !__GNUC__ */

#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/pthread/Pthread.h"
typedef PAMI::Mutex::CounterMutex<PAMI::Counter::Pthread> GenericDeviceMutex;
#define MUTEX_NAME	"PthreadCounter"

#endif /* !__GNUC__ */
#endif // !__pami_target_bgq__

#ifndef QUEUE_NAME
#define QUEUE_NAME	"MutexedQueue<" MUTEX_NAME ">"
#define GenericDeviceWorkQueue		PAMI::Queue
#define GenericDevicePostingQueue	PAMI::GccThreadSafePostingQueue<PAMI::MutexedQueue<GenericDeviceMutex> >
#endif

#include <pami.h>
#include "components/memory/MemoryManager.h"
#include "util/queue/GccThreadSafeMultiQueue.h"
#include "util/queue/GccThreadSafeQueue.h"
#include "util/queue/MutexedMultiQueue.h"
#include "util/queue/Queue.h"
#include "util/queue/MutexedQueue.h"
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/native/NativeCounter.h"

#include "components/devices/generic/Device.h"
#include "components/devices/generic/AdvanceThread.h"

static inline pid_t gettid() {
	return syscall(SYS_gettid);
}

#endif // __test_internals_queue_gendev_setup_h__
