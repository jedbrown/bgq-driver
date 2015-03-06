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

/// \file components/devices/bgq/commthread/WakeupRegion.h
/// \brief

#ifndef __components_devices_bgq_commthread_WakeupRegion_h__
#define __components_devices_bgq_commthread_WakeupRegion_h__

#include <pami.h>
#include "spi/include/l2/atomic.h"
#include "spi/include/kernel/memory.h"

#ifndef L1_CACHELINE_SIZE
#define L1_CACHELINE_SIZE	64
#endif // !L1_CACHELINE_SIZE

#ifndef L2_CACHELINE_SIZE
#define L2_CACHELINE_SIZE	128
#endif // !L2_CACHELINE_SIZE

#define WU_CACHELINE_SIZE	L1_CACHELINE_SIZE

#define WU_BASELINE		256
#define WU_CONTEXT_COST		104
#define WU_PROC_DISCOUNT	8

////////////////////////////////////////////////////////////////////////////////
/// \env{bgq,PAMI_BGQ_WU_BASELINE}
/// Size, bytes, of baseline WAC region.
/// size = PAMI_BGQ_WU_BASELINE + PAMI_BGQ_WU_CONTEXT_COST * nctx -
///			PAMI_BGQ_WU_PROC_DISCOUNT * nproc;
/// Minimum: PAMI_BGQ_WU_BASELINE;
/// \default 256
///
/// \env{bgq,PAMI_BGQ_WU_CONTEXT_COST}
/// Size, bytes, per-context WAC memory.
/// size = PAMI_BGQ_WU_BASELINE + PAMI_BGQ_WU_CONTEXT_COST * nctx -
///			PAMI_BGQ_WU_PROC_DISCOUNT * nproc;
/// \default 104
///
/// \env{bgq,PAMI_BGQ_WU_PROC_DISCOUNT}
/// Size, bytes, per-proc WAC savings.
/// size = PAMI_BGQ_WU_BASELINE + PAMI_BGQ_WU_CONTEXT_COST * nctx -
///			PAMI_BGQ_WU_PROC_DISCOUNT * nproc;
/// \default 8
////////////////////////////////////////////////////////////////////////////////

namespace PAMI {
namespace Device {
namespace CommThread {

class BgqWakeupRegion {
private:
	inline size_t _nextPowerOfTwo(size_t esize) {
		size_t n = esize - 1;
		if (!(esize & n)) return esize;
		size_t s = 1;
		size_t x;
		while ((x = (n >> s)) != 0) {
			n |= x;
			s <<= 1;
		}
		return n + 1;
	}
public:
	// type (size) used for each context. Must be power-of-two.

	BgqWakeupRegion() :
	_wu_mm(),
	_wu_memreg(),
	_ref(0)
	{ }

	~BgqWakeupRegion() {
	}


	/// \brief Initialize Wakeup Region for client
	///
	/// \param[in] nctx	Number of contexts being created for client
	/// \param[in] me	My index in local topology
	/// \param[in] lsize	Local size (local topology size)
	/// \param[in] mm	L2Atomic/Shmem MemoryManager
	/// \return	Error code
	///
	inline pami_result_t init(size_t nctx, size_t me, size_t lsize, Memory::MemoryManager *mm) {
		size_t mctx = nctx;
		// in order for WAC base/mask values to work, need to ensure size is
		// of power-of-two with same address alignment. Assuming phy address
		// alignment will match virt address alignment.

		char *env;
		size_t wu_baseline = WU_BASELINE;
		if ((env = getenv("PAMI_BGQ_WU_BASELINE"))) {
			wu_baseline = strtoul(env, NULL, 0);
		}
		size_t wu_context_cost = WU_CONTEXT_COST;
		if ((env = getenv("PAMI_BGQ_WU_CONTEXT_COST"))) {
			wu_context_cost = strtoul(env, NULL, 0);
		}
		size_t wu_proc_discount = WU_PROC_DISCOUNT;
		if ((env = getenv("PAMI_BGQ_WU_PROC_DISCOUNT"))) {
			wu_proc_discount = strtoul(env, NULL, 0);
		}
		size_t esize = (wu_baseline + mctx * wu_context_cost -
					lsize * wu_proc_discount) * sizeof(uint64_t);
		esize = _nextPowerOfTwo(esize);
		if (lsize >= PAMI_MAX_PROC_PER_NODE) {
			esize = wu_baseline + 256; // never actually used for wakeup...
		}

		char key[PAMI::Memory::MMKEYSIZE];
		size_t i;
		for (i = 0; i < lsize; ++i) {
			sprintf(key, "/pami-wu-region-%zd", i);
			pami_result_t rc = _wu_mm[i].init(mm, esize, esize, 0,
					PAMI::Memory::PAMI_MM_WACREGION, key);
			if (rc != PAMI_SUCCESS) {
fprintf(stderr, "memalign failed for %zd %zd (avail=%zd)\n", esize, esize, mm->available());
				return PAMI_ERROR;
			}
			if (i == me) {
				uint32_t krc = Kernel_CreateMemoryRegion(&_wu_memreg,
						_wu_mm[i].base(), _wu_mm[i].size());
				if (krc != 0) {
					//mm->free(virt);
fprintf(stderr, "Kernel_CreateMemoryRegion failed for %p %zd (%d)\n",
				_wu_mm[i].base(), _wu_mm[i].size(), krc);
					return PAMI_ERROR;
				}
			}
		}
#if 0
		size_t size = lsize * esize;
		char *v = (char *)virt;
		size_t i;
		for (i = 0; i < lsize; ++i) {
			_wu_mm[i].init(v, esize);
			v += esize;
		}
#endif
		_wu_region_me = me;

		// PAMI_assert((size & (size - 1)) == 0); // power of 2
		// PAMI_assert((_wu_memreg.BasePa & (size - 1)) == 0); // aligned
		return PAMI_SUCCESS;
	}

	/// \brief Return base phy addr and mask for WAC given context(s)
	///
	/// \todo incorporate memregion into memory manager? also default alignment?
	///
	/// \param[in] ctx	Bitmap of contexts to get WAC range for (currently not used)
	/// \param[out] base	Physical base address of memory block
	/// \param[out] mask	Address bit mask of memory block
	///
	inline void getWURange(uint64_t ctx, uint64_t *base, uint64_t *mask) {
		*base = (uint64_t)_wu_memreg.BasePa +
			((char *)_wu_mm[_wu_region_me].base() - (char *)_wu_memreg.BaseVa);
		*mask = ~(_wu_mm[_wu_region_me].size() - 1);
	}

	inline PAMI::Memory::MemoryManager *getWUmm(size_t process = (size_t)-1) {
		if (process == (size_t)-1) process = _wu_region_me;
		return &_wu_mm[process];
	}

	inline PAMI::Memory::GenMemoryManager *getAllWUmm() {
		return &_wu_mm[0];
	}

	inline void addRef() {
		__sync_fetch_and_add(&_ref, 1);
	}

	inline size_t rmRef() {
		return __sync_fetch_and_add(&_ref, -1);
	}

private:
	typedef uint64_t BgqWakeupRegionBuffer[BGQ_WACREGION_SIZE];

	PAMI::Memory::GenMemoryManager _wu_mm[PAMI_MAX_PROC_PER_NODE];
	size_t _wu_region_me;	///< local process index into WAC regions
	Kernel_MemoryRegion_t _wu_memreg;	///< phy addr of WAC region
	size_t _ref;
}; // class BgqWakeupRegion

}; // namespace CommThread
}; // namespace Device
}; // namespace PAMI

#endif // __components_devices_bgq_commthread_WakeupRegion_h__
