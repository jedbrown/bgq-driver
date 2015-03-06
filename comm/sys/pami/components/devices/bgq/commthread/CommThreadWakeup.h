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

/// \file components/devices/bgq/commthread/CommThreadWakeup.h
/// \brief

#ifndef __components_devices_bgq_commthread_CommThreadWakeup_h__
#define __components_devices_bgq_commthread_CommThreadWakeup_h__

#include "Memory.h"

#ifndef COMMTHREAD_LAYOUT_TESTING
#include <pami.h>
#include "components/devices/bgq/commthread/WakeupRegion.h"
#include "components/devices/bgq/commthread/ContextSets.h"
#include "components/devices/bgq/commthread/CommThreadFactory.h"
#include "common/bgq/Context.h"
#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "hwi/include/bqc/A2_inlines.h"
#include "spi/include/kernel/thread.h"

#undef DEBUG_COMMTHREADS // Enable debug messages here

// limit/default for PAMI_COMMTHREAD_SLEEP
#define COMMTHREAD_SLEEP_MIN	2000	// absolute minimum
#define COMMTHREAD_SLEEP_REC	100000	// recommended

// Defaults for "sleeping commthreads" mode.
// #undef these to keep "standard" defaults.
#define DEFAULT_SLEEP_MAXTHRDS	1
#define DEFAULT_SLEEP_MINLOOPS	1
#define DEFAULT_SLEEP_MAXLOOPS	100

#include "spi/include/wu/wait.h"

#ifndef SCHED_COMM
/// \todo #warning No SCHED_COMM from CNK yet
#define SCHED_COMM SCHED_RR
#endif // !SCHED_COMM

#define COMMTHREAD_SCHED	SCHED_COMM
//#define COMMTHREAD_SCHED	SCHED_FIFO
//#define COMMTHREAD_SCHED	SCHED_OTHER

#ifdef DEBUG_COMMTHREADS
#define DEBUG_INIT()						\
		char __dbgbuf[64];				\
		int __dbgbufl = sprintf(__dbgbuf, "xx %ld %d", pthread_self(), Kernel_ProcessorID());	\
		int __dbgbufi = 0;

#define DEBUG_WRITE(a,b)				\
		__dbgbuf[0] = a; __dbgbuf[1] = b;	\
		__dbgbuf[__dbgbufl] = '\n';		\
		__dbgbufi = 1;				\
		write(2, __dbgbuf, __dbgbufl + __dbgbufi);

#define DEBUG_WRITE_T(a,b,t)				\
		__dbgbuf[0] = a; __dbgbuf[1] = b;	\
		__dbgbufi = sprintf(__dbgbuf + __dbgbufl, " %ld\n", t); \
		write(2, __dbgbuf, __dbgbufl + __dbgbufi);

#else // ! DEBUG_COMMTHREADS
#define DEBUG_INIT()
#define DEBUG_WRITE(a,b)
#define DEBUG_WRITE_T(a,b,t)
#endif // ! DEBUG_COMMTHREADS

extern PAMI::Device::CommThread::Factory __commThreads;
#endif // !COMMTHREAD_LAYOUT_TESTING

////////////////////////////////////////////////////////////////////////////////
/// \envs{pami,commthread,Commthread}
/// This is some general documentation about the commthread
/// environment variables.
///
/// \env{commthread,PAMI_COMMTHREAD_SLEEP}
/// Use sleeping-preemtion for over-subscribed commthreads.
/// Value is the interval (nanoseconds) at which a commthread wakes up
/// and advances, when having been preempted by a user thread.
/// Use suffix of 'u' for microseconds, 'm' for milliseconds,
/// or 's' for seconds. Absolute minimum 2000ns, recommended minimum
/// 100000ns. Set to "d(efault)" for recommended value.
/// \default (0) Standard preemption - until user threads sleeps or exits
///
/// \env{commthread,PAMI_MAX_COMMTHREADS}
/// Maximum number of commthreads to create.
/// This can be used to avoid hardware thread oversubscription.
/// \default (64 / Ranks-per-Node) - 1
///
/// \env{commthread,PAMI_COMMTHREAD_MAX_LOOPS}
/// Maximum number of loops through advance done by
/// a commthread before checking status for possible sleep or preemption.
/// Must be >= PAMI_COMMTHREAD_MIN_LOOPS.
/// \default 100
///
/// \env{commthread,PAMI_COMMTHREAD_MIN_LOOPS}
/// Minimum number of loops through advance done by
/// a commthread before checking status for possible sleep or preemption.
/// Must be <= PAMI_COMMTHREAD_MAX_LOOPS.
/// \default 10
////////////////////////////////////////////////////////////////////////////////

namespace PAMI {
namespace Device {
namespace CommThread {

class BgqCommThread {
private:

#ifdef COMMTHREAD_LAYOUT_TESTING

public:
        BgqCommThread() { }
        static size_t _maxActive;
        static size_t _maxloops;
        static size_t _minloops;

#else // !COMMTHREAD_LAYOUT_TESTING

	/// \brief Convenience code to lock and/or unlock contexts in set
	///
	/// \param[in] old	Currently locked contexts
	/// \param[in] new	New set of contexts that should be locked
	///
	inline size_t __lockContextSet(uint64_t &old, uint64_t new_) {
		uint64_t m, l = old;
		size_t x, e = 0;
		pami_result_t r;
		PAMI::Context * ctx;

		m = (old ^ new_) & old; // must unlock these
		x = 0;
		while (m) {
			if (m & 1) {
				ctx = _ctxset->getContext(x);
				if(ctx) 
				{
					ctx->cleanupAffinity(false);
					r = ctx->unlock();
				}
				l &= ~(1ULL << x);
			}
			m >>= 1;
			++x;
		}

		m = (old ^ new_) & new_; // must lock these
		x = 0;
		while (m) {
			if (m & 1) {
				++e; // if we need, or get, new conetxt
				ctx = _ctxset->getContext(x);
				if(ctx) {
					r = ctx->trylock();
					if (r == PAMI_SUCCESS) {
						l |= (1ULL << x);
						ctx->cleanupAffinity(true);
					}
				}
			}
			m >>= 1;
			++x;
		}
		old = l;
		return e;
	}

	/// \brief Convenience code to advance all contexts in set
	///
	/// \param[in] ctx	Bitmap of contexts that are locked
	/// \return	Number of work events
	///
	inline size_t __advanceContextSet(uint64_t ctxset) {
		size_t x, e = 0;
		uint64_t m = ctxset;
		pami_result_t r;
		PAMI::Context * ctx;
		x = 0;
		while (m) {
			if (m & 1) {
				ctx = _ctxset->getContext(x);
				if(ctx) 
				{
					e += ctx->advance(1, r);
					r = r; // avoid warning until we figure out what to do with result
				}
			}
			m >>= 1;
			++x;
		}
		return e;
	}

	/// \brief Arm the MU interrupt-through-Wakeup Unit
	///
	inline void __armMU_WU() {
		WU_ArmMU(0x0f);
	}

	/// \brief Disarm the MU interrupt-through-Wakeup Unit
	///
	inline void __disarmMU_WU() {
		WU_DisarmMU(0x0f);
	}

public:
	BgqCommThread(BgqWakeupRegion *wu, BgqContextPool *pool, size_t num_ctx) :
	_wakeup_region(wu),
	_ctxset(pool),
	_thread(0),
	_falseWU(0),
	_shutdown(false)
	{
		_wakeup_region->addRef();
	}

	~BgqCommThread() {
		if (_wakeup_region->rmRef() == 1) {
			_wakeup_region->~BgqWakeupRegion();
		}
	}


	static void *commThread(void *cookie) {
		BgqCommThread *thus = (BgqCommThread *)cookie;
		pami_result_t r = thus->__commThread();
		r = r; // avoid warning until we decide how to use result
		return NULL;
	}

	static void *sleepingCommThread(void *cookie) {
		BgqCommThread *thus = (BgqCommThread *)cookie;
		pami_result_t r = thus->__sleepingCommThread();
		r = r; // avoid warning until we decide how to use result
		return NULL;
	}

	static void commThreadSig(int sig) {
		// BgqCommThread *thus = (BgqCommThread *)???;
		// thus->__commThreadSig(sig);
		// force us to highest priority so we can exit...
		pthread_t self = pthread_self();
		int max_pri = sched_get_priority_max(COMMTHREAD_SCHED);
//fprintf(stderr, "Bop %ld\n", self);
		pthread_setschedprio(self, max_pri);
	}

#if 0
	static void balanceThreads(uint32_t core, uint32_t &newcore, uint32_t &newthread) {

		uint64_t mask = __commThreads.availThreads(core);



		uint32_t c = core;
		uint32_t m = (c > _ptCore ? _ptThread + 1 : _ptThread);
		if (!m) {
			uint32_t j;
			for (c = j = 16 - 1; j > _ptCore; --j) {
				if (_core_iter[j] < _core_iter[c]) c = j;
			}
			m = _ptThread + 1;
		}
		newthread = (NUM_SMT - 1) - (_core_iter[c] % m);
		newcore = c;
		++_core_iter[c];
	}
#endif

	static inline void initContext(size_t contextid, pami_context_t context) {
		// might need hook later, to do per-context initialization?
		// PAMI::Context *ctx = (PAMI::Context *)context;
		// size_t client_id = ctx->getClientId();
	}

	static inline pami_result_t registerAsync(pami_context_t context,
				PAMI::ProgressExtension::pamix_async_function progress_fn,
				PAMI::ProgressExtension::pamix_async_function suspend_fn,
				PAMI::ProgressExtension::pamix_async_function resume_fn,
				void *cookie) {
		PAMI::Context *ctx = (PAMI::Context *)context;
		return ctx->registerAsync(progress_fn, suspend_fn, resume_fn, cookie);
	}

	// arrange for the new comm-thread to hunt-down this context and
	// take it, even if some other thread already picked it up.
	// This helps ensure a more-balanced startup, and prevents some
	// complexities of trying to re-balance later. Then, only when
	// a comm-thread leaves the set does there have to be a re-balance.
	static inline pami_result_t addContext(pami_context_t context) {
		int status;
		uint32_t c, t, core;
		PAMI::Context *ctx = (PAMI::Context *)context;
		// size_t client_id = ctx->getClientId();
		core = ctx->coreAffinity();
		BgqCommThread *thus;

		//balanceThreads(core, c, t);
		c = core;
		__commThreads.getNextThread(c, t);
		// any error checking?
		thus = _comm_xlat[c][t];

		// all BgqCommThread objects have the same ContextSet object.
		BgqCommThread *devs = __commThreads.getCommThreads();
		// this will wakeup existing commthreads...
		// note, unsolvable race condition: we want to add 'm' to
		// the existing commthread's _initCtxs, but can't do that
		// until getting 'm' from call to addContext() which will
		// wakeup commthreads. This means any commthread might get
		// the new context, and the true "home" commthread will
		// have to force it away later. A more-complex two-phased
		// approach would help. But, this is really not much different
		// than a newly created commthread which will have to force
		// its core context(s) back to itself.
		uint64_t m = devs[0]._ctxset->addContext(context);
		if (thus) {
			if (m == 0) {
				return PAMI_EAGAIN; // closest thing to ENOSPC ?
			}
			thus->_initCtxs |= m;
			Memory::sync();
			return PAMI_SUCCESS;
		}

		// we assume this is single-threaded so no locking required...
		size_t x = _numActive++;
		thus = &devs[x];
		thus->_initCtxs = m;	// this should never be zero
		pthread_attr_t attr;

		status = pthread_attr_init(&attr);
		if (status) {
			--_numActive;
			errno = status;
			return PAMI_CHECK_ERRNO;
		}
		status = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
		if (status) {
			pthread_attr_destroy(&attr);
			--_numActive;
			errno = status;
			return PAMI_CHECK_ERRNO;
		}
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
		if (status) {
			pthread_attr_destroy(&attr);
			--_numActive;
			errno = status;
			return PAMI_CHECK_ERRNO;
		}
		status = pthread_attr_setschedpolicy(&attr, COMMTHREAD_SCHED);
		if (status) {
			pthread_attr_destroy(&attr);
			--_numActive;
			errno = status;
			return PAMI_CHECK_ERRNO;
		}
		cpu_set_t cpu_mask;
		CPU_ZERO(&cpu_mask);
		CPU_SET(c * NUM_SMT + t, &cpu_mask);
		status = pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpu_mask);
		if (status) {
			pthread_attr_destroy(&attr);
			--_numActive;
			errno = status;
			return PAMI_CHECK_ERRNO;
		}

		thus->_shutdown = false;
		Memory::sync();
		status = pthread_create(&thus->_thread, &attr, _func, thus);
		pthread_attr_destroy(&attr);
		if (status) {
			--_numActive;
			thus->_thread = 0; // may not always be legal assignment?
			errno = status;
			return PAMI_CHECK_ERRNO;
		}
		_comm_xlat[c][t] = thus;
		return PAMI_SUCCESS;
	}

	static inline pami_result_t rmContexts(pami_context_t _ctxs, size_t nctx) {
		PAMI::Context *ctxs = (PAMI::Context *)_ctxs;
		size_t x;
		if (_numActive == 0) {
			return PAMI_SUCCESS;
		}
		BgqCommThread *devs = __commThreads.getCommThreads();

		// all BgqCommThread objects have the same ContextSet object.

		// This should wakeup all commthreads, and any one holding
		// this context should release it...
		uint64_t mask = devs[0]._ctxset->disableContexts(ctxs, nctx);
		for (x = 0; x < _numActive; ++x) {
			devs[x]._initCtxs &= ~mask;
		}

		// wait here for all contexts to get released? must only
		// wait for all commthreads to release, not for other threads
		// that might have the context locked - how to tell?
		// Maybe the caller knows best?

		// wait for commthreads to unlock contexts we removed.
		// is this guaranteed to complete?
		// is this guaranteed not to race with locker?
		// do we need some sort of "generation counter" barrier?
		uint64_t lmask;
		do {
			lmask = 0;
			for (x = 0; x < _numActive; ++x) {
				lmask |= devs[x]._lockCtxs;
			}
		} while (lmask & mask);

		devs[0]._ctxset->rmContexts(ctxs, nctx);

		return PAMI_SUCCESS;
	}

private:
	inline pami_result_t __commThread() {
		// should/can this use the internal (C++) interface?
		uint64_t new_ctx, old_ctx, lkd_ctx;
		size_t n, events, ev_since_wu;
		size_t max_loop = BgqCommThread::_maxloops;
		size_t min_loop = BgqCommThread::_minloops;
		size_t id; // our current commthread id, among active ones.
		pthread_t self = pthread_self();
		uint64_t wu_start, wu_mask;
		int min_pri = sched_get_priority_min(COMMTHREAD_SCHED);
		int max_pri = sched_get_priority_max(COMMTHREAD_SCHED);
DEBUG_INIT();
		sigset_t sigset_usr1;
		sigemptyset(&sigset_usr1);
		sigaddset(&sigset_usr1, SIGUSR1);

		pthread_setschedprio(self, max_pri);
		struct sigaction sigact;
		sigact.sa_handler = commThreadSig;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = 0;
		sigaction(SIGUSR1, &sigact, NULL);
		_ctxset->joinContextSet(id, _initCtxs);
//fprintf(stderr, "comm thread %ld for context %04zx\n", self, _initCtxs);
DEBUG_WRITE('c','t');
		new_ctx = old_ctx = lkd_ctx = 0;
		ev_since_wu = 0;
		while (!_shutdown) {
			//
new_context_assignment:	// These are the same now, assuming the re-locking is
more_work:		// lightweight enough.
			//

			__armMU_WU();

			// doing this without 'new_ctx' depends on it being ignored...
			_wakeup_region->getWURange(0, &wu_start, &wu_mask);

			n = 0;
			events = 0;
			do {
				WU_ArmWithAddress(wu_start, wu_mask);
				new_ctx = _ctxset->getContextSet(id);

				// this only locks/unlocks what changed...
				events += __lockContextSet(lkd_ctx, new_ctx);
				_lockCtxs = lkd_ctx;
				if (old_ctx != new_ctx) ev_since_wu += 1;
				old_ctx = new_ctx;
				Memory::sync();
				events += __advanceContextSet(lkd_ctx);
				ev_since_wu += events;
				++n;
			} while (!_shutdown && lkd_ctx &&
				(events != 0 || n < min_loop) && n < max_loop);
			if (_shutdown) break;

			// Snoop the scheduler to see if other threads are competing.
			// This should also include total number of threads on
			// the core, and some heuristic by which we decide to
			// back-off more. This gets complicated if we consider
			// whether those other sw threads are truly active, or even
			// running in some syncopated "tag team" mode.
			// TBD
//re_evaluate:
			n = Kernel_SnoopRunnable();

			if (n <= 1) {
				// we are alone
				if (events == 0) {
					// The wait can only detect new work.
					// Only do the wait if we know the
					// contexts have no work. otherwise
					// we could wait forever for new work
					// while existing work waits for us to
					// advance it.
					if (ev_since_wu == 0 && lkd_ctx) ++_falseWU;
DEBUG_WRITE('g','i');
					ppc_waitimpl();
DEBUG_WRITE('w','u');
					ev_since_wu = 0;
					if (_shutdown) break;
				}
				// need to re-evaluate things here?
				// goto re_evaluate;
				// ... or just go back and do work?
				goto more_work;
			} else {
				__disarmMU_WU();

				// this only locks/unlocks what changed...
				(void)__lockContextSet(lkd_ctx, 0);
				_lockCtxs = lkd_ctx;
				Memory::sync();

				_ctxset->leaveContextSet(id); // id invalid now
DEBUG_WRITE('s','a');
				// The signal won't be handled until *after*
				// we regain control anyway, so we need to block
				// it to prevent deadlocks in pthread_setschedprio.
				sigprocmask(SIG_BLOCK, &sigset_usr1, NULL);
				pthread_setschedprio(self, min_pri);
				//=== we get preempted here ===//
				pthread_setschedprio(self, max_pri);
				sigprocmask(SIG_UNBLOCK, &sigset_usr1, NULL);
DEBUG_WRITE('s','b');

				if (_shutdown) break;
				_ctxset->joinContextSet(id, _initCtxs); // got id
				// always assume context set changed... just simpler.
				goto new_context_assignment;
			}
		}

		if (lkd_ctx) {
			(void)__lockContextSet(lkd_ctx, 0);
			_lockCtxs = lkd_ctx;
			Memory::sync();
		}
		if (id != (size_t)-1) {
			_ctxset->leaveContextSet(id); // id invalid now
		}
DEBUG_WRITE('t','t');
		return PAMI_SUCCESS;
	}

	inline pami_result_t __sleepingCommThread() {
		// should/can this use the internal (C++) interface?
		uint64_t new_ctx, old_ctx, lkd_ctx;
		size_t n, events, ev_since_wu;
		size_t max_loop = BgqCommThread::_maxloops;
		size_t min_loop = BgqCommThread::_minloops;
		size_t id; // our current commthread id, among active ones.
		pthread_t self = pthread_self();
		uint64_t wu_start, wu_mask;
		int max_pri = sched_get_priority_max(COMMTHREAD_SCHED);
DEBUG_INIT();
		sigset_t sigset_usr1;
		sigemptyset(&sigset_usr1);
		sigaddset(&sigset_usr1, SIGUSR1);

		pthread_setschedprio(self, max_pri);
		struct sigaction sigact;
		sigact.sa_handler = commThreadSig;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = 0;
		sigaction(SIGUSR1, &sigact, NULL);
		_ctxset->joinContextSet(id, _initCtxs);
DEBUG_WRITE('c','t');
		new_ctx = old_ctx = lkd_ctx = 0;
		ev_since_wu = 0;
		while (!_shutdown) {
			//
new_context_assignment:	// These are the same now, assuming the re-locking is
more_work:		// lightweight enough.
			//

			__armMU_WU();

			// doing this without 'new_ctx' depends on it being ignored...
			_wakeup_region->getWURange(0, &wu_start, &wu_mask);

			n = 0;
			events = 0;
			do {
				WU_ArmWithAddress(wu_start, wu_mask);
				new_ctx = _ctxset->getContextSet(id);

				// this only locks/unlocks what changed...
				events += __lockContextSet(lkd_ctx, new_ctx);
				_lockCtxs = lkd_ctx;
				if (old_ctx != new_ctx) ev_since_wu += 1;
				old_ctx = new_ctx;
				Memory::sync();
				events += __advanceContextSet(lkd_ctx);
				ev_since_wu += events;
				++n;
			} while (!_shutdown && lkd_ctx &&
				(events != 0 || n < min_loop) && n < max_loop);
			if (_shutdown) break;

			// Snoop the scheduler to see if other threads are competing.
			// This should also include total number of threads on
			// the core, and some heuristic by which we decide to
			// back-off more. This gets complicated if we consider
			// whether those other sw threads are truly active, or even
			// running in some syncopated "tag team" mode.
			// TBD
//re_evaluate:
			n = Kernel_SnoopRunnable();

			if (n <= 1) {
				// we are alone
				if (events == 0) {
					// The wait can only detect new work.
					// Only do the wait if we know the
					// contexts have no work. otherwise
					// we could wait forever for new work
					// while existing work waits for us to
					// advance it.
					if (ev_since_wu == 0 && lkd_ctx) ++_falseWU;
DEBUG_WRITE('g','i');
					ppc_waitimpl();
DEBUG_WRITE('w','u');
					ev_since_wu = 0;
					if (_shutdown) break;
				}
				// need to re-evaluate things here?
				// goto re_evaluate;
				// ... or just go back and do work?
				goto more_work;
			} else {
				struct timespec rem;
				__disarmMU_WU();

				// Should we release the contexts here?
				// We could wake-up faster if we keep
				// them locked but if there is ever a
				// chance we don't wake-up it could be
				// disasterous.

				// we don't care if we wake-up early, just
				// do our thing and go back to sleep.
//DEBUG_WRITE('s','a');
//uint64_t t0 = Kernel_GetTimeBase();
				nanosleep(&_sleep, &rem);
//t0 = Kernel_GetTimeBase() - t0;
//DEBUG_WRITE_T('s','b', t0);

				if (_shutdown) break;
				// always assume context set changed... just simpler.
				goto new_context_assignment;
			}
		}

		if (lkd_ctx) {
			(void)__lockContextSet(lkd_ctx, 0);
			_lockCtxs = lkd_ctx;
			Memory::sync();
		}
		if (id != (size_t)-1) {
			_ctxset->leaveContextSet(id); // id invalid now
		}
DEBUG_WRITE('t','t');
		return PAMI_SUCCESS;
	}

	friend class PAMI::Device::CommThread::Factory;
	BgqWakeupRegion *_wakeup_region;	///< WAC memory for contexts (common)
	PAMI::Device::CommThread::BgqContextPool *_ctxset; ///< context set (common)
	volatile uint64_t _initCtxs;	///< initial set of contexts to take
	volatile uint64_t _lockCtxs;	///< set of contexts we have locked
	pthread_t _thread;		///< pthread identifier
	volatile size_t _falseWU;	///< perf counter for false wakeups
	volatile bool _shutdown;	///< request commthread to exit
	static BgqCommThread *_comm_xlat[NUM_CORES][NUM_SMT];
	static struct timespec _sleep;
	static void *(*_func)(void *cookie);
	static size_t _numActive;
	static size_t _maxActive;
        static size_t _maxloops;
        static size_t _minloops;
	static size_t _ptCore;
	static size_t _ptThread;
#endif // !COMMTHREAD_LAYOUT_TESTING
}; // class BgqCommThread

void *(*PAMI::Device::CommThread::BgqCommThread::_func)(void *cookie) = BgqCommThread::commThread;
struct timespec PAMI::Device::CommThread::BgqCommThread::_sleep = { 0, 0 };
size_t PAMI::Device::CommThread::BgqCommThread::_maxActive = 0;
size_t PAMI::Device::CommThread::BgqCommThread::_maxloops = 100;
size_t PAMI::Device::CommThread::BgqCommThread::_minloops = 10;
#ifndef COMMTHREAD_LAYOUT_TESTING
size_t PAMI::Device::CommThread::BgqCommThread::_numActive = 0;
size_t PAMI::Device::CommThread::BgqCommThread::_ptCore = 0;
size_t PAMI::Device::CommThread::BgqCommThread::_ptThread = 0;
PAMI::Device::CommThread::BgqCommThread *PAMI::Device::CommThread::BgqCommThread::_comm_xlat[NUM_CORES][NUM_SMT] = {{NULL}};

#undef DEBUG_INIT
#undef DEBUG_WRITE
#endif // !COMMTHREAD_LAYOUT_TESTING

// Called from __global...
//
// This isn't really a device, only using this for convenience but
//
Factory::Factory(PAMI::Memory::MemoryManager *genmm,
			PAMI::Memory::MemoryManager *l2xmm) :
_commThreads(NULL)
{
	size_t x;
	BgqCommThread::_maxActive = Kernel_ProcessorCount() - 1;
#ifndef COMMTHREAD_LAYOUT_TESTING
	BgqCommThread *devs;
	BgqWakeupRegion *wu;
	BgqContextPool *pool;

#if 0
	size_t num_ctx = __MUGlobal.getMuRM().getPerProcessMaxPamiResources();
	// may need to factor in others such as shmem?
#else
	size_t num_ctx = 256 / Kernel_ProcessCount();    // Why not platform MAX_CONTEXTS?
#endif

	pami_result_t rc;
	size_t me = __global.topology_local.rank2Index(__global.mapping.task());
	size_t lsize = __global.topology_local.size();

	// config param may also affect this?

	char *env = getenv("PAMI_COMMTHREAD_SLEEP");
	if (env) {
		char *e = NULL;
		x = strtoul(env, &e, 0);
		if (x > 0 || *e) {
			if (*e == 'd' || *e == 'D') x = COMMTHREAD_SLEEP_REC;
			else if (*e == 'u' || *e == 'U') x *= 1000;
			else if (*e == 'm' || *e == 'M') x *= 1000 * 1000;
			else if (*e == 's' || *e == 'S') x *= 1000 * 1000 * 1000;
			if (x < COMMTHREAD_SLEEP_MIN) x = COMMTHREAD_SLEEP_MIN;
			if (x < COMMTHREAD_SLEEP_REC) {
				// print warning if < recommended?
			}
			BgqCommThread::_sleep.tv_sec = x / (1000 * 1000 * 1000);
			BgqCommThread::_sleep.tv_nsec = x % (1000 * 1000 * 1000);
			BgqCommThread::_func = BgqCommThread::sleepingCommThread;
			// Setup other defaults...
#ifdef DEFAULT_SLEEP_MAXTHRDS
			BgqCommThread::_maxActive = DEFAULT_SLEEP_MAXTHRDS;
#endif
#ifdef DEFAULT_SLEEP_MINLOOPS
			BgqCommThread::_minloops = DEFAULT_SLEEP_MINLOOPS;
#endif
#ifdef DEFAULT_SLEEP_MAXLOOPS
			BgqCommThread::_maxloops = DEFAULT_SLEEP_MAXLOOPS;
#endif
		}
	}

	env = getenv("PAMI_MAX_COMMTHREADS");
	if (env) {
		x = strtoul(env, NULL, 0);
		if (x < BgqCommThread::_maxActive) BgqCommThread::_maxActive = x;
	}
	env = getenv("PAMI_COMMTHREAD_MAX_LOOPS");
	if (env) {
		x = strtoul(env, NULL, 0);
		if (x < BgqCommThread::_minloops) x = BgqCommThread::_minloops;
		BgqCommThread::_maxloops = x;
	}
	env = getenv("PAMI_COMMTHREAD_MIN_LOOPS");
	if (env) {
		x = strtoul(env, NULL, 0);
		if (x > BgqCommThread::_maxloops) x = BgqCommThread::_maxloops;
		BgqCommThread::_minloops = x;
	}

	BgqCommThread::_ptCore = (NUM_CORES - 1) - (BgqCommThread::_maxActive % NUM_CORES);
	BgqCommThread::_ptThread = BgqCommThread::_maxActive / NUM_CORES;

	rc = __global.heap_mm->memalign((void **)&devs, 16, BgqCommThread::_maxActive * sizeof(*devs));
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc BgqCommThread");
	rc = __global.heap_mm->memalign((void **)&pool, 16, sizeof(*pool));
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc BgqContextPool");
	rc = __global.heap_mm->memalign((void **)&wu, 16, sizeof(*wu)); // one per client
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc BgqWakeupRegion");

	new (wu) BgqWakeupRegion();
	rc = wu->init(num_ctx, me, lsize, l2xmm);
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to init BgqWakeupRegion - not enough shared memory?");
	__global._wuRegion_mms = wu->getAllWUmm();
	__global._wuRegion_mm = wu->getWUmm();
	new (pool) BgqContextPool();
	pool->init(BgqCommThread::_maxActive, num_ctx, l2xmm, wu->getWUmm());

	for (x = 0; x < BgqCommThread::_maxActive; ++x) {
		new (&devs[x]) BgqCommThread(wu, pool, num_ctx);
	}
	_commThreads = devs;
	size_t tcoord = __global.mapping.t();
#endif // !COMMTHREAD_LAYOUT_TESTING

	// determine what range of hwthreads we have...
	// Try to generalize, and just use the bitmap as
	// a set of "available" threads, zero each one as we
	// start a commthread on it...  also take into account
	// the "main" thread, where we never start a commthread.
	uint64_t tmask = Kernel_ThreadMask(tcoord);
	memset(_num_used, 0, sizeof(_num_used));
	memset(_num_avail, 0, sizeof(_num_avail));

	// Assume the calling thread is main(), remove it from the mask.
	tmask &= ~(1UL << ((NUM_CORES * NUM_SMT - 1) - Kernel_ProcessorID()));
	_avail_threads = tmask;
	int k, c;
	uint64_t m;

	_core0 = Kernel_ProcessorID() / NUM_SMT;
	_smt0 = Kernel_ProcessorID() % NUM_SMT;
	c = Kernel_ProcessorCount() - 1; // highest thread index in process
	k = c - BgqCommThread::_maxActive;
	for (; c > k; --c) {
		x = getRecommendedThread(c);
		uint32_t core = x / NUM_SMT;
		uint32_t thread = x % NUM_SMT;

		m = (1UL << ((NUM_CORES * NUM_SMT - 1) - x));
		if (!(tmask & m)) {
			if (k > 0) --k;
			continue;
		}
		// could also assert that _num_avail[core] < NUM_SMT...
		_smt_xlat[core][_num_avail[core]++] = thread;
	}
#ifdef COMMTHREAD_LAYOUT_TESTING
{       int c, t, k;
        static char buf[128];
        k = (int)_core0 + ((int)Kernel_ProcessorCount() + 2) / NUM_SMT;
        for (c = _core0; c < k; ++c) {
                char *s = buf;
                s += sprintf(s, " %3d:", c);
                for (t = 0; t < _num_avail[c]; ++t) {
                        s += sprintf(s, " %3d", _smt_xlat[c][t]);
                }
                fprintf(stderr, "%s\n", buf);
        }
}
#endif // COMMTHREAD_LAYOUT_TESTING
}

Factory::~Factory() {
#ifndef COMMTHREAD_LAYOUT_TESTING
	size_t x;

	// assert _commThreads[0]._ctxset->_nactive == 0...
	// will that be true? commthreads with no contexts will
	// still be in the waitimpl loop. We really should confirm
	// that all commthreads have reached the "zero contexts"
	// state, though. In that case, the commthread needs to
	// give some feedback telling us how many contexts it
	// thinks it has.

	for (x = 0; x < BgqCommThread::_numActive; ++x) {
		_commThreads[x]._shutdown = true;
		// touching _shutdown will not directly wakeup commthreads,
		// so must do something... tbd

		// If the target commthread is in waitimpl, we can
		// easily wake it.

		// If the thread is in pthread_setschedprio(min)
		// then it has released all locks and may be safely
		// terminated.

		// However, we can't tell which case applies.
		// If we can just get both cases to wake up, the
		// thread will notice the shutdown and exit.
		// But if it is in setsched the problem is that
		// most likely some other thread has preempted it
		// and so the pthread_join below may hang(?)
		// Maybe we don't really care about the pthread join?
		// We do need it for statistics gathering, though.

		// Note, should not get here unless all clients/contexts
		// have been destroyed, so in that case the commthreads
		// can just be terminated - they are all totally inactive.
//fprintf(stderr, "pthread_kill(%ld, SIGUSR1);\n", _commThreads[x]._thread);
		pthread_kill(_commThreads[x]._thread, SIGUSR1);
	}
//fprintf(stderr, "all signalled\n");

	// need to pthread_join() here? or is it too risky (might hang)?
	size_t fwu = 0;
	size_t na = BgqCommThread::_numActive;
	BgqCommThread::_numActive = 0;
	for (x = 0; x < na; ++x) {
		void *status;
//fprintf(stderr, "pthread_join(%ld, &status);\n", _commThreads[x]._thread);
		pthread_join(_commThreads[x]._thread, &status);
		fwu += _commThreads[x]._falseWU;
		_commThreads[x].~BgqCommThread();
	}
// if (fwu > 2) fprintf(stderr, "Commthreads saw %zd false wakeups\n", fwu);
#endif // !COMMTHREAD_LAYOUT_TESTING
	for (;x < BgqCommThread::_maxActive; ++x) {
		_commThreads[x].~BgqCommThread();
	}
}

pami_result_t Factory::addContext(pami_context_t ctx) {
	return BgqCommThread::addContext(ctx);
}
pami_result_t Factory::rmContexts(pami_context_t ctxs, size_t nctx) {
	return BgqCommThread::rmContexts(ctxs, nctx);
}
void Factory::initContext(size_t contextid, pami_context_t context) {
	BgqCommThread::initContext(contextid, context);
}
pami_result_t Factory::registerAsync(pami_context_t ctx,
			PAMI::ProgressExtension::pamix_async_function progress_fn,
			PAMI::ProgressExtension::pamix_async_function suspend_fn,
			PAMI::ProgressExtension::pamix_async_function resume_fn,
			void *cookie) {
	return BgqCommThread::registerAsync(ctx,
			progress_fn, suspend_fn, resume_fn, cookie);
}

}; // namespace CommThread
}; // namespace Device
}; // namespace PAMI

#endif // __components_devices_bgq_commthread_CommThreadWakeup_h__
