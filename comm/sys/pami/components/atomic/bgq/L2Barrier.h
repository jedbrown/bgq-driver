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

#ifndef __components_atomic_bgq_L2Barrier_h__
#define __components_atomic_bgq_L2Barrier_h__

/**
 * \file components/atomic/bgq/L2Barrier.h
 *
 * \brief BG/Q L2 Atomics implementation of a Barriers
 *
 * These objects must NOT be instantiated in shared memory!
 * Each process may get a different virtual address, so *_counters
 * would get stomped.
 */
#include "components/atomic/BarrierInterface.h"
#include "components/atomic/IndirectInterface.h"

#include "Global.h"
#include "Memory.h"
#include <spi/include/l2/atomic.h>

namespace PAMI
{
  namespace Barrier
  {
    namespace BGQ
    {
      /**
       * \brief Base structure for proc-scoped L2 atomics barrier
       *
       * This houses 5 local memory counters which are used to implement
       * a barrier.
       */
      struct L2_Barrier_ctrs
      {
        uint64_t ctrl_lock;
        uint64_t lock[2];
        uint64_t status[2];
      };

      class _L2_Barrier_s
      {
	private:
		static void static_counter_init(void *mem, size_t bytes, const char *key, unsigned attrs, void *cookie) {
			// L2 Atomic-capable memory... just ensure it is zero.
			memset(mem, 0, bytes);
		}
        public:
          _L2_Barrier_s() :
              _counters(NULL)
          { }

          template <class T_MemoryManager>
          pami_result_t __init(T_MemoryManager *mm, const char *key)
          {
            PAMI_assert_debugf(!_counters, "Re-init or object is in shmem");
            return mm->memalign((void **)&_counters, sizeof(uint64_t),
                                sizeof(*_counters), key,
				static_counter_init, NULL);
          }

          inline uint64_t *controlPtr() { return &_counters->ctrl_lock; }
          inline uint64_t *lockPtr(int n) { return &_counters->lock[n]; }
          inline uint64_t *statusPtr(int n) { return &_counters->status[n]; }

        private:
          L2_Barrier_ctrs *_counters;
        public:
          uint8_t _master;    /**< master participant */
          uint8_t _coreshift; /**< convert core to process for comparing to master */
          uint8_t _nparties;  /**< number of participants */
      };

      /*
       * This class cannot be used directly. The super class must allocate the
       * particular type of lockbox based on desired scope.
       */
      class _L2Barrier
      {
        protected:

          inline _L2Barrier () :
            _active (false),
            _initialized (false)
          {};

          inline ~_L2Barrier () {};

          // -------------------------------------------------------------------
          // PAMI::Barrier::Interface<T> implementation
          // -------------------------------------------------------------------

          inline void enter_impl ()
          {
            begin_impl();
            while (poll_impl());
            end_impl();
          };

          template <class T_Functor>
          inline void enter_impl (T_Functor & functor)
          {
            begin_impl();
            while (poll_impl())
              functor();
            end_impl();
          };

          inline void begin_impl()
          {
            uint64_t lockup;
            Memory::sync();
            lockup = *(_barrier.controlPtr());
            L2_AtomicLoadIncrement(_barrier.lockPtr(lockup));
            _data = (void*)lockup;
            _active = true;
          }

          inline bool poll_impl()
          {
            PAMI_assert(_active == true);
            uint64_t lockup, value;
            lockup = (uint64_t)_data;

            if (*(_barrier.lockPtr(lockup)) < _barrier._nparties)
              {
                return true; // barrier is still active ...
              }

            // All cores have participated in the barrier
            // We need all cores to block until checkin
            // to clear the lock atomically
            L2_AtomicLoadIncrement(_barrier.lockPtr(lockup));

            do
              {
                value = *(_barrier.lockPtr(lockup));
              }
            while (value > 0 && value < (unsigned)(2 * _barrier._nparties));

            if ((Kernel_PhysicalProcessorID() >> _barrier._coreshift) == _barrier._master)
              {
                if (lockup)
                  {
                    L2_AtomicLoadDecrement(_barrier.controlPtr());
                  }
                else
                  {
                    L2_AtomicLoadIncrement(_barrier.controlPtr());
                  }

                L2_AtomicLoadClear(_barrier.statusPtr(lockup));
                L2_AtomicLoadClear(_barrier.lockPtr(lockup));
              }
            else
              {
                // wait until master releases the barrier by clearing the lock
                while (*(_barrier.lockPtr(lockup)) > 0);
              }

            _active = false;
            return false;
          }

          inline void end_impl()
          {
            // noop
          }

          _L2_Barrier_s _barrier;
          void *_data;
          bool _active;
          bool _initialized;

      }; // class _L2Barrier

      class L2NodeCoreBarrier :
          public PAMI::Barrier::Interface<L2NodeCoreBarrier>,
          public PAMI::Atomic::Indirect<L2NodeCoreBarrier>,
          public _L2Barrier
      {
        public:

          friend class PAMI::Barrier::Interface<L2NodeCoreBarrier>;
          friend class PAMI::Atomic::Indirect<L2NodeCoreBarrier>;

          static const bool indirect = true;

          inline L2NodeCoreBarrier (size_t participants, bool master) :
            PAMI::Barrier::Interface<L2NodeCoreBarrier> (0, false),
            PAMI::Atomic::Indirect<L2NodeCoreBarrier> (),
            _L2Barrier ()
          {
          };

          inline ~L2NodeCoreBarrier() {};

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl (T_MemoryManager *mm, const char *key)
          {
            PAMI_assert(_initialized == false);
            PAMI_assert(_active == false);

            // For core-granularity, everything is
            // a core number. Assume the master core
            // is the lowest-numbered core in the
            // process.

            pami_result_t rc;
	    rc = _barrier.__init(mm, key);
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to get L2 Atomic NodeCoreBarrier");

            // PAMI_assert(m .iff. me == masterProc());
            _barrier._master = __global.l2atomicFactory.masterProc() << __global.l2atomicFactory.coreShift();
            _barrier._coreshift = 0;
            _barrier._nparties = __global.l2atomicFactory.numCore();
            _active = false;
            _initialized = true;
          }

          inline void clone_impl (L2NodeCoreBarrier & atomic)
          {
            PAMI_abortf("how do we clone the l2 objects?\n");

            //_barrier.master = atomic._barrier.master;
            //_barrier.coreshift = atomic._barrier.coreshift;
            //_barrier.nparties = atomic._barrier.nparties;
          };

      }; // class    PAMI::Barrier::BGQ::L2NodeCoreBarrier

      class L2NodeProcBarrier :
          public PAMI::Barrier::Interface<L2NodeProcBarrier>,
          public PAMI::Atomic::Indirect<L2NodeProcBarrier>,
          public _L2Barrier
      {
        public:

          friend class PAMI::Barrier::Interface<L2NodeProcBarrier>;
          friend class PAMI::Atomic::Indirect<L2NodeProcBarrier>;

          static const bool indirect = true;

          inline L2NodeProcBarrier(size_t participants, bool master) :
            PAMI::Barrier::Interface<L2NodeProcBarrier> (0, false),
            PAMI::Atomic::Indirect<L2NodeProcBarrier> (),
            _L2Barrier ()
          {
            	_barrier._coreshift = __global.l2atomicFactory.coreShift();
		_barrier._nparties = participants;
		_barrier._master = master ?
			Kernel_PhysicalProcessorID() >> __global.l2atomicFactory.coreShift() :
			(size_t)-1;
	  }

          inline ~L2NodeProcBarrier() {};

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl(T_MemoryManager *mm, const char *key)
          {
            PAMI_assert(_initialized == false);
            PAMI_assert(_active == false);

            // For proc-granularity, must convert
            // between core id and process id,
            // and only one core per process will
            // participate.

            pami_result_t rc = PAMI_SUCCESS;
	    rc = _barrier.__init(mm, key);
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to get L2 Atomic NodeProcBarrier");

            // PAMI_assert(m .iff. me == masterProc());
            _active = false;
            _initialized = true;
          }

          inline void clone_impl (L2NodeProcBarrier & atomic)
          {
            PAMI_abortf("how do we clone the l2 objects?\n");

            //_barrier.master = atomic._barrier.master;
            //_barrier.coreshift = atomic._barrier.coreshift;
            //_barrier.nparties = atomic._barrier.nparties;
          };

      }; // class     PAMI::Barrier::BGQ::L2NodeProcBarrier
    };   // namespace PAMI::Barrier::BGQ
  };     // namespace PAMI::Barrier
};       // namespace PAMI

#endif // __components_atomic_bgq_L2Barrier_h__
