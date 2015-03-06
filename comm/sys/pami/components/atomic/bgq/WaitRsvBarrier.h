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

#ifndef __components_atomic_bgq_WaitRsvBarrier_h__
#define __components_atomic_bgq_WaitRsvBarrier_h__

/**
 * \file components/atomic/bgq/WaitRsvBarrier.h
 *
 * \brief BG/Q WaitRsv Atomics implementation of a Barriers
 *
 * These objects must NOT be instantiated in shared memory!
 * Each process may get a different virtual address, so *_counters
 * would get stomped.
 */
#include "components/atomic/Barrier.h"
#include "Global.h"
#include "Memory.h"
#include "hwi/include/bqc/A2_inlines.h"

namespace PAMI {
namespace Barrier {
namespace BGQ {
        /**
         * \brief Base structure for proc-scoped WaitRsv atomics barrier
         *
         * This houses 5 local memory counters which are used to implement
         * a barrier.
         */
        struct WaitRsv_Barrier_ctrs {
		union {
                	uint64_t ctrl_lock;
			uint8_t _cacheline[L1D_CACHE_LINE_SIZE];
		} _u_ct;
		union {
			struct {
                		uint64_t lock;
                		uint64_t status;
			} _s;
			uint8_t _cacheline[L1D_CACHE_LINE_SIZE];
		} _u_lk[2];
        };

        class _WaitRsv_Barrier_s {
        public:
                _WaitRsv_Barrier_s() :
		_counters(NULL)
		{ }

		pami_result_t __init(PAMI::Memory::MemoryManager *mm, const char *key) {
			PAMI_assert_debugf(!_counters, "Re-init or object is in shmem");
			return mm->memalign((void **)&_counters, sizeof(uint64_t),
							sizeof(*_counters), key);
		}

                inline uint64_t *controlPtr() { return &_counters->_u_ct.ctrl_lock; }
                inline uint64_t *lockPtr(int n) { return &_counters->_u_lk[n]._s.lock; }
                inline uint64_t *statusPtr(int n) { return &_counters->_u_lk[n]._s.status; }

        private:
                WaitRsv_Barrier_ctrs *_counters
			__attribute__((__aligned__(L1D_CACHE_LINE_SIZE)));
        public:
                uint8_t _master;    /**< master participant */
                uint8_t _coreshift; /**< convert core to process for comparing to master */
                uint8_t _nparties;  /**< number of participants */
        };

/*
 * This class cannot be used directly. The super class must allocate the
 * particular type of lockbox based on desired scope.
 */
class _WaitRsvBarrier {
private:
	template <bool T_Wait>
        inline PAMI::Atomic::Interface::barrierPollStatus __poll() {
                PAMI_assert(_status == PAMI::Atomic::Interface::Entered);
                uint64_t lockup, value;
                lockup = (uint64_t)_data;
                if (LoadReserved(_barrier.lockPtr(lockup)) < _barrier._nparties) {
			if (T_Wait) ppc_waitrsv();
                        return PAMI::Atomic::Interface::Entered;
                }

                // All cores have participated in the barrier
                // We need all cores to block until checkin
                // to clear the lock atomically
                Fetch_and_Add(_barrier.lockPtr(lockup), 1);
		while ((value = LoadReserved(_barrier.lockPtr(lockup))) > 0 &&
				value < (unsigned)(2 * _barrier._nparties)) {
			ppc_waitrsv();
		}

                if ((Kernel_PhysicalProcessorID() >> _barrier._coreshift) == _barrier._master) {
                        if (lockup) {
                                Fetch_and_Add(_barrier.controlPtr(), -1);
                        } else {
                                Fetch_and_Add(_barrier.controlPtr(), 1);
                        }
                        *(_barrier.statusPtr(lockup)) = 0;
                        *(_barrier.lockPtr(lockup)) = 0;
                } else {
                        // wait until master releases the barrier by clearing the lock
                        while (LoadReserved(_barrier.lockPtr(lockup)) > 0) {
				ppc_waitrsv();
			}
                }
                _status = PAMI::Atomic::Interface::Initialized;
                return PAMI::Atomic::Interface::Done;
        }
public:
        _WaitRsvBarrier() { }
        ~_WaitRsvBarrier() { }

        inline void init_impl(Memory::MemoryManager *mm, const char *key, size_t z, bool m) {
                PAMI_abortf("_WaitRsvBarrier class must be subclass");
        }

        inline pami_result_t enter_impl() {
                pollInit_impl();
                while (__poll<true>() != PAMI::Atomic::Interface::Done);
                return PAMI_SUCCESS;
        }

        inline void enterPoll_impl(PAMI::Atomic::Interface::pollFcn fcn, void *arg) {
                pollInit_impl();
                while (__poll<false>() != PAMI::Atomic::Interface::Done) {
			// semantic choice: if fcn() can wait until "something happens",
			// could use __poll<true>() and lighten the load.
                        fcn(arg);
                }
        }

        inline void pollInit_impl() {
                uint64_t lockup;
                Memory::sync();
                lockup = *(_barrier.controlPtr());
                Fetch_and_Add(_barrier.lockPtr(lockup), 1);
                _data = (void*)lockup;
                _status = PAMI::Atomic::Interface::Entered;
        }

        inline PAMI::Atomic::Interface::barrierPollStatus poll_impl() {
                return __poll<false>();
	}
        // With 5 lockboxes used... which one should be returned?
        inline void *returnBarrier_impl() { return _barrier.controlPtr(); }
protected:
        _WaitRsv_Barrier_s _barrier;
        void *_data;
        PAMI::Atomic::Interface::barrierPollStatus _status;
}; // class _WaitRsvBarrier

class WaitRsvNodeCoreBarrier :
                public PAMI::Atomic::Interface::Barrier<WaitRsvNodeCoreBarrier>,
                public _WaitRsvBarrier {
public:
        WaitRsvNodeCoreBarrier() {}
        ~WaitRsvNodeCoreBarrier() {}
        inline void init_impl(PAMI::Memory::MemoryManager *mm, const char *key, size_t z, bool m) {
                // For core-granularity, everything is
                // a core number. Assume the master core
                // is the lowest-numbered core in the
                // process.

		pami_result_t rc = _barrier.__init(mm, key);
		PAMI_assertf(rc == PAMI_SUCCESS, "Failed to get WaitRsv Atomic NodeCoreBarrier");

                // PAMI_assert(m .iff. me == masterProc());
                _barrier._master = __global.l2atomicFactory.masterProc() << __global.l2atomicFactory.coreShift();
                _barrier._coreshift = 0;
                _barrier._nparties = __global.l2atomicFactory.numCore();
                _status = PAMI::Atomic::Interface::Initialized;
        }
	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		// This is an indirect object, cannot instantiate in shared memory.
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}
	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		// This is an WaitRsv Atomic object, must have WaitRsv Atomic-mapped memory
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}
}; // class WaitRsvNodeCoreBarrier

class WaitRsvNodeProcBarrier :
                public PAMI::Atomic::Interface::Barrier<WaitRsvNodeProcBarrier>,
                public _WaitRsvBarrier {
public:
        WaitRsvNodeProcBarrier() {}
        ~WaitRsvNodeProcBarrier() {}
        inline void init_impl(PAMI::Memory::MemoryManager *mm, const char *key, size_t z, bool m) {
                // For proc-granularity, must convert
                // between core id and process id,
                // and only one core per process will
                // participate.

		pami_result_t rc = _barrier.__init(mm, key);
		PAMI_assertf(rc == PAMI_SUCCESS, "Failed to get WaitRsv Atomic NodeProcBarrier");

                // PAMI_assert(m .iff. me == masterProc());
                _barrier._master = __global.l2atomicFactory.coreXlat(__global.l2atomicFactory.masterProc()) >> __global.l2atomicFactory.coreShift();
                _barrier._coreshift = __global.l2atomicFactory.coreShift();
                _barrier._nparties = __global.l2atomicFactory.numProc();
                _status = PAMI::Atomic::Interface::Initialized;
        }
	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		// This is an indirect object, cannot instantiate in shared memory.
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}
	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		// This is an WaitRsv Atomic object, must have WaitRsv Atomic-mapped memory
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}
}; // class WaitRsvNodeProcBarrier

}; // BGQ namespace
}; // Barrier namespace
}; // PAMI namespace

#endif // __components_atomic_bgq_WaitRsvBarrier_h__
