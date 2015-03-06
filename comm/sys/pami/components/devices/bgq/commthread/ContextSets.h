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

/// \file components/devices/bgq/commthread/ContextSets.h
/// \brief

#ifndef __components_devices_bgq_commthread_ContextSets_h__
#define __components_devices_bgq_commthread_ContextSets_h__

#include <pami.h>
#include "components/atomic/bgq/L2Mutex.h"
#include "components/atomic/indirect/IndirectMutex.h"
#include "Context.h"

namespace PAMI {
namespace Device {
namespace CommThread {

class BgqContextPool {
	typedef PAMI::Mutex::Indirect<PAMI::Mutex::BGQ::L2> ContextSetMutex;

	// lock is held by caller...
	inline uint64_t __getOneContext(uint64_t &ref) {
		uint64_t k = ref;
		uint64_t m;
		size_t x = ffs(k);
		if (x > 0 && x < 64) {
			--x;
			m = (1UL << x);
			k &= ~m;
			ref = k;
			return m;
		}
		// should never happen...
		return 0;
	}

	/// \brief utility function to count number of '1' bits in a word
	///
	/// \param[in] k	Bitmap in which to count bits
	/// \return	Number of '1' bits found in 'k'
	///
	inline size_t num_bits(uint64_t k) {
		static int __num_bits[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
		size_t n = 0;
		while (k) {
			n += __num_bits[k & 0x0f];
			k >>= 4;
		}
		return n;
	}

	/// \brief assign contexts to commthreads considering balanced and affinity
	///
	/// lock is held by caller...
	///
	/// a commthread is giving up all it's contexts, or this is the second
	/// phase of a general re-balancing.
	///
	/// either way, re-distribute contexts across the (remaining) commthreads.
	///
	/// \param[in] ctxs	Bitmap of contexts that need commthreads
	///
	inline void __giveupContexts(uint64_t ctxs) {
		uint64_t m = ctxs;
		if (!m) return;
		// now try to assign remaining contexts to alternate commthreads.
		size_t x = 0;
		while (m) {
			if (m & 1) {
				size_t miny = _nsets;
				size_t minc = _ncontexts;
				size_t aminy = _nsets;
				size_t aminc = _ncontexts;
				size_t y;
				// look for comthreads with the lowest number of
				// contexts, both having affinity and not.
				// if a commthread with affinity is found, use it.
				// otherwise just pick the commthread with least
				// number of contexts assigned.
				for (y = 0; y < _nsets; ++y) {
					if (!(_actm & (1UL << y))) continue;
					if (_contexts[x] && (_contexts[x]->coreAffinity() == _coreids[y])) {
						if (_numinsets[y] < aminc) {
							aminy = y;
							aminc = _numinsets[y];
						}
					} else {
						if (_numinsets[y] < minc) {
							miny = y;
							minc = _numinsets[y];
						}
					}
				}
				if (aminy < _nsets) {
					_sets[aminy] |= (1UL << x);
					++_numinsets[aminy];
				} else {
					_sets[miny] |= (1UL << x);
					++_numinsets[miny];
				}
			}
			m >>= 1;
			++x;
		}
	}

	// lock is held by caller...
	//
	// do a complete redistribution of contexts. start by assigning
	// contexts to "home" commthreads, if they are active. Then
	// distribute any remaining contexts over the commthreads.
	//
	inline void __rebalanceContexts() {
		uint64_t m;
		size_t n;

		// first, assign contexts to "home" commthreads, if possible.
		m = (1UL << _ncontexts) - 1; // mask of all contexts
		for (n = 0; n < _nsets; ++n) {
			if (_actm & (1UL << n)) {
				uint64_t k = _coresets[n];
				_sets[n] = k;
				m &= ~k;
				_numinsets[n] = num_bits(k);
			}
		}
		__giveupContexts(m);
	}

	/// \todo #warning HACK workaround until mm support alloc by keys
	inline void privateFromShared(Memory::MemoryManager *setmm,
				void **ptr, size_t align, size_t bytes) {
		uint32_t np = __global.topology_local.size();
		uint8_t *v = NULL;
		pami_result_t rc;
                rc = setmm->memalign((void **)&v, align, bytes * np);
		PAMI_assertf(rc == PAMI_SUCCESS && v,
				"Out of memory for BgqContextPool::_sets");
		v += bytes * __global.mapping.t();
		memset(v, 0, bytes);
		*ptr = v;
	}

	inline void do_event_callbacks(uint64_t m, const int start) {
		PAMI::ProgressExtension::pamix_async_function progress, suspend, resume;
		void *cookie;
		size_t x = 0;
		for (; m; m >>= 1, ++x) {
			if (!(m & 1)) continue;
      if(!_contexts[x]) continue;
			_contexts[x]->getAsyncRegs(&progress,
					&suspend, &resume, &cookie);
			if (start && resume) {
				resume((pami_context_t)_contexts[x], cookie);
			}
			if (!start && suspend) {
				suspend((pami_context_t)_contexts[x], cookie);
			}
		}
	}

public:
	BgqContextPool() :
	_contexts(NULL),
	_ncontexts_total(0),
	_ncontexts(0),
	_mutex(),
	_actm(0),
	_sets(NULL),
	_nsets(0),
	_nactive(0),
	_lastset(0)
	{
	}

	inline void init(size_t nsets, size_t nctx,
				Memory::MemoryManager *mm,
				Memory::MemoryManager *setmm) {
		_mutex.init(mm, NULL); /// \todo note: mutex is (always?) process-private
		__global.heap_mm->memalign((void **)&_contexts, 16, nctx * sizeof(*_contexts));
		PAMI_assertf(_contexts, "Out of memory for BgqContextPool::_contexts");
		memset((void *)_contexts, 0, nctx * sizeof(*_contexts));

		// only _sets[] needs to be in WAC region... but are process private
		setmm->memalign((void **)&_sets, 16, sizeof(*_sets) * (nsets + 1));

		__global.heap_mm->memalign((void **)&_coresets, 16,
							nsets * sizeof(*_coresets));
		PAMI_assertf(_coresets, "Out of memory for BgqContextPool::_coresets");
		memset((void *)_coresets, 0, nsets * sizeof(*_coresets));

		__global.heap_mm->memalign((void **)&_coreids, 16,
							nsets * sizeof(*_coreids));
		PAMI_assertf(_coreids, "Out of memory for BgqContextPool::_coreids");
		memset((void *)_coreids, 0, nsets * sizeof(*_coreids));

		__global.heap_mm->memalign((void **)&_numinsets, 16,
							nsets * sizeof(*_numinsets));
		PAMI_assertf(_numinsets, "Out of memory for BgqContextPool::_numinsets");
		memset((void *)_numinsets, 0, nsets * sizeof(*_numinsets));

		_ncontexts_total = nctx;
		_nsets = nsets;
	}

	// caller promises to be single-threaded (?)
	inline uint64_t addContext(pami_context_t ctx) {
		_mutex.acquire();
		size_t x;
		for (x = 0; x < _ncontexts_total; ++x) {
			if (_contexts[x] == NULL) {
				break;
			}
		}
		if (x >= _ncontexts_total) {
			// should never happen
			_mutex.release();
			return 0UL;
		}
		_contexts[x] = (PAMI::Context *)ctx;
		++_ncontexts;
		_sets[_nsets] |= (1UL << x);
		_mutex.release();
		// presumably, a new comm thread is about to call joinContextSet(),
		// but can we guarantee timing? May need a way to ensure that
		// "_sets[_nsets]" will be checked.
		return (1UL << x);
	}

	// first make these contexts "invisible" to comm threads.
	inline uint64_t disableContexts(PAMI::Context *ctxs, size_t nctx) {
		size_t x, y;
		uint64_t mask = 0;
		_mutex.acquire();
		for (y = 0; y < nctx; ++y) {
			for (x = 0; x < _ncontexts_total; ++x) {
			        if ((_contexts[x]) && (_contexts[x] == (void *)&ctxs[y])) {
					mask |= (1UL << x);
				}
			}
		}
		_sets[_nsets] &= ~mask;
		for (x = 0; x < _nsets; ++x) {
			_sets[x] &= ~mask;
			_coresets[x] &= ~mask;
		}
		// maybe should rebalance?
		// also, caller probably needs to wait until all the contexts
		// are released (i.e. lock released).
		_mutex.release();
		return mask;
	}

	// now, remove (all references to) contexts entirely
	inline void rmContexts(PAMI::Context *ctxs, size_t nctx) {
		size_t x, y;
		_mutex.acquire();
		for (y = 0; y < nctx; ++y) {
			for (x = 0; x < _ncontexts_total; ++x) {
			        if ((_contexts[x]) && (_contexts[x] == (void *)&ctxs[y])) {
					_contexts[x] = NULL;
					--_ncontexts;
				}
			}
		}
		_mutex.release();
	}

	PAMI::Context *getContext(size_t contextix) {
		return _contexts[contextix];
	}

	inline uint64_t getContextSet(size_t threadid) {
//		_mutex.acquire();  Not needed...removed because it slows performance.
		uint64_t m = _sets[threadid];
#if 0
		// should we check "_sets[_nsets]" and possibly pick up more contexts?
		// or just trigger a "rejoin"?
#endif
//		_mutex.release();  Not needed...removed because it slows performance.
		return m;
	}

	inline void joinContextSet(size_t &threadid,
						uint64_t initial = 0UL) {
		uint64_t m = 0, n = 0;
		_mutex.acquire();
		// find free slot...
		while (n < 64 && (_actm & (1UL << n)) != 0) ++n;
		threadid = n;
		_coresets[threadid] = initial;
		_coreids[threadid] = Kernel_ProcessorCoreID();
		_actm |= (1UL << threadid);
		++_nactive;
		if (_nactive == 1) { /* first active thread gets all available */
			// take all? or just a few...?
			m = _sets[_nsets]; /* | initial */
			_sets[_nsets] = 0;
			/* This thread gets all available (_sets/_numinsets), no rebalance needed */
			_sets[threadid] = m;
			_numinsets[threadid] = num_bits(m);
			// might not need lock for this, but safer this way.
			do_event_callbacks(m, 1);
		} else { 
			/* Not first active thread but contexts may have been added/stopped since last callbacks */
			if(_sets[_nsets])
			{ /* so do callbacks before rebalance */
				m = _sets[_nsets];
				_sets[_nsets] = 0;
				/* later, rebalance will figure _sets & _numinsets for all threads */
				do_event_callbacks(m, 1);
			}
			__rebalanceContexts(); // recalc _sets[], _numinsets[]
		}
		_mutex.release();
	}

	inline void leaveContextSet(size_t &threadid) {
		size_t na;
		_mutex.acquire();
		na = --_nactive;
		uint64_t m = _sets[threadid];
		// PAMI_assert((_actm & (1UL << threadid)) != 0);
		_sets[threadid] = 0;      // not valid unless _actm bit set
		_coresets[threadid] = 0;  // not valid unless _actm bit set
		_coreids[threadid] = 0;   // not valid unless _actm bit set
		_numinsets[threadid] = 0; // not valid unless _actm bit set
		// must clear bit before calling __giveupContexts()
		_actm &= ~(1UL << threadid);
		// Race: another commthread could start picking up contexts
		// before we finish... so have to do suspend callbacks under
		// the lock.
		if (na == 0) {
			_sets[_nsets] |= m;
			do_event_callbacks(m, 0);
		} else {
			__giveupContexts(m);
		}
		_mutex.release();
		threadid = -1; // only valid while we're joined.
	}
private:
	PAMI::Context **_contexts;
	size_t _ncontexts_total;
	size_t _ncontexts;

	ContextSetMutex _mutex;
	// the following must be protected by the _mutex
	volatile uint64_t _actm;  
	volatile uint64_t *_sets;			// _sets[_nsets] has the mask of contexts added & suspended
	                              // _sets[threadid] ....
	volatile uint64_t *_coresets;
	uint64_t *_coreids;
	volatile uint64_t *_numinsets;
	size_t _nsets;
	volatile size_t _nactive;
	volatile size_t _lastset;	
}; // class BgqContextPool

}; // namespace CommThread
}; // namespace Device
}; // namespace PAMI

#endif // __components_devices_bgq_commthread_ContextSets_h__
