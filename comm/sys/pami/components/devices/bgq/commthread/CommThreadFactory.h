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

/// \file components/devices/bgq/commthread/CommThreadFactory.h
/// \brief

#ifndef __components_devices_bgq_commthread_CommThreadFactory_h__
#define __components_devices_bgq_commthread_CommThreadFactory_h__

#if 0 // doesn't work because of circular dependencies anyway
#include "common/bgq/ResourceManager.h"
extern PAMI::ResourceManager __pamiRM;
#endif

#include "api/extension/c/async_progress/ProgressExtension.h"

namespace PAMI {
namespace Device {
namespace CommThread {

class BgqCommThread;
class Factory {
	static const int MAX_NCTXS = 64;   // Why not platform MAX_CONTEXTS?
public:
	Factory() { }
        Factory(PAMI::Memory::MemoryManager *genmm, PAMI::Memory::MemoryManager *l2xmm);
        ~Factory();
#ifdef COMMTHREAD_LAYOUT_TESTING
	void * operator new(size_t x, void *v) { return v; }
#endif // COMMTHREAD_LAYOUT_TESTING
	static pami_result_t addContext(pami_context_t context);
	static pami_result_t rmContexts(pami_context_t ctxs, size_t nctx);
	static pami_result_t registerAsync(pami_context_t ctx,
				PAMI::ProgressExtension::pamix_async_function progress_fn,
				PAMI::ProgressExtension::pamix_async_function suspend_fn,
				PAMI::ProgressExtension::pamix_async_function resume_fn,
				void *cookie);
	static void initContext(size_t contextid, pami_context_t context);

#if 0 // doesn't work because of circular dependencies anyway
	inline size_t getPerProcessMaxPamiResources(size_t RmClientId) {
		size_t x, y = 0;
		size_t n = MAX_NCTXS;
		// this could be done only once, in ctor...
		// or better yet, there should be common routine since everyone needs it.
		size_t t = __pamiRM.getNumClients();
		for (x = 0; x < t; ++x) {
			size_t z;
			if (x + 1 == t) {
				z = n;
			} else {
				z = MAX_NCTXS;
				z *= __pamiRM.getClientWeight(x);
				z /= 100;
			}
			n -= z;
			if (x == RmClientId) y = z;
		}
		return y;
	}
#endif

	inline size_t getPerProcessMaxPamiResources() {
		return MAX_NCTXS; // this is because of uint64_t for context sets bitmaps.
	}

	/// \brief Return reference to the entire commthreads array
	///
	/// \return	pointer to the global _commThreads array
	///
        inline BgqCommThread *getCommThreads() { return _commThreads; }

	/// \brief return the next SMT to use on the given core
	///
	/// Implements a round-robin algorithm starting at the highest
	/// hwthread id a going downward. This is for finding the
	/// "home" commthread for a new context, so the SMT returned
	/// may already have a running commthread on it. It is also
	/// up to the caller to decide if a commthread has too many
	/// "core" contexts and whether to put the context on a core
	/// that might be sub-optimal.
	///
	/// Note we also don't have an "undo", so if the caller fails to
	/// actually assign this to the core/thread returned, it may end
	/// up out of balance. The caller should be taking steps to ensure
	/// that the context will be handled, but there could be an imbalance
	/// of assigned "home" threads.
	///
	/// \param[in,out] core	The core on which to place another commthread
	/// \param[out] thread	The hwthread on 'core' to use
	///
	inline void getNextThread(uint32_t &core, uint32_t &thread) {

		if (!_num_avail[core]) {
			// special case: desired core not avail
			// probably never happens, the MU RM avoids this.
			int j, k;
			// Find the first core available to this process
			for (j = NUM_CORES - 1; j >= 0 && !_num_avail[j]; --j);
			PAMI_assert_debugf(j >= 0, "Internal error: no available cores");

			// Now find the core with the least number of contexts assigned
			for (k = j--; j >= 0; --j) {
				if (!_num_avail[j]) continue;
				if (_num_used[j] < _num_used[k]) k = j;
			}
			core = k;
		}
		int x = _num_used[core]++;
		x %= _num_avail[core];
		thread = _smt_xlat[core][x];
	}

	// This might someday be a CNK SPI (should be?)
	// For now, assumes being called from main thread of process.
	// Also, assumes CNK is operating in the BREADTH layout...
	//
	inline uint32_t getRecommendedThread(uint32_t index) {
		uint32_t a;
		if (Kernel_ProcessCount() > NUM_CORES) {
			a = index;
		} else {
			uint32_t cpp = Kernel_ProcessorCount() / NUM_SMT;
			a = index * NUM_SMT;
			a += ((index / cpp) * 2) % NUM_SMT;
			a += (index / (2 * cpp));
			a %= Kernel_ProcessorCount();
		}
		return _core0 * NUM_SMT + _smt0 + a;
	}

private:
        BgqCommThread *_commThreads;
	uint64_t _avail_threads;
	int _num_used[NUM_CORES];
	int _num_avail[NUM_CORES];
	int _smt_xlat[NUM_CORES][NUM_SMT];
	uint32_t _core0;
	uint32_t _smt0;
}; // class Factory

}; // namespace CommThread
}; // namespace Device
}; // namespace PAMI

#endif // __components_devices_bgq_commthread_CommThreadFactory_h__
