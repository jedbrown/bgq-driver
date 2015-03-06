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
 * \file components/atomic/bgq/L2CounterBounded.h
 * \brief BG/Q L2 Atomics implementation of node- and process-scoped \b bounded atomic counters
 */
#ifndef __components_atomic_bgq_L2CounterBounded_h__
#define __components_atomic_bgq_L2CounterBounded_h__

#include "Global.h"
#include "components/atomic/BoundedCounterInterface.h"
#include "components/atomic/IndirectInterface.h"
#include "components/atomic/bgq/L2Counter.h"
#include "util/common.h"

#include "common/bgq/L2AtomicFactory.h"
#include <spi/include/l2/atomic.h>

namespace PAMI {
namespace BoundedCounter {
namespace BGQ {

class IndirectL2Bounded : public PAMI::BoundedCounter::Interface<IndirectL2Bounded>,
	public PAMI::Atomic::Indirect<IndirectL2Bounded> {
public:

	friend class PAMI::BoundedCounter::Interface<IndirectL2Bounded>;
	friend class PAMI::Atomic::Indirect<IndirectL2Bounded>;

	static const bool indirect = true;
	static const size_t bound_error = 0x8000000000000000;

	inline IndirectL2Bounded() :
		PAMI::BoundedCounter::Interface<IndirectL2Bounded> (),
		PAMI::Atomic::Indirect<IndirectL2Bounded> (),
		_counter(NULL)
	{};

	inline ~IndirectL2Bounded() {};

	static inline bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		// must not be shared memory.
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}

	static inline bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
	}

	inline void bound_upper_store_and_add() {
		L2_AtomicStoreAdd(&_counter->upper, 1);
	}

protected:

	struct L2BoundedCounter_t {
		volatile uint64_t lower;
		volatile uint64_t count;
		volatile uint64_t upper;
	};

	// -------------------------------------------------------------------
	// PAMI::Atomic::Indirect<T> implementation
	// -------------------------------------------------------------------

	///
	/// \brief Initialize the indirect L2 atomic counter
	///
	/// Does not use the memory manager from the user, but instead uses
	/// the BGQ-specific L2 Atomic Factory to access the L2 Memory Manager
	/// to allocate the memory.
	///
        template <class T_MemoryManager>
          inline void init_impl(T_MemoryManager *mm, const char *key) {
            pami_result_t rc;

            if ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0) {
              rc = mm->memalign((void **)&_counter, 32, sizeof(*_counter), key,
                  IndirectL2Bounded::counter_initialize, NULL);
            } else {
              void * __counter = NULL;
              rc = __global.l2atomicFactory.__nodescoped_mm.memalign(&__counter,
                                                                     32,
                                                                     sizeof(*_counter),
                                                                     key,
                                                                     IndirectL2Bounded::counter_initialize,
                                                                     NULL);
              _counter = (L2BoundedCounter_t*)__counter;
            }

            PAMI_assertf(rc == PAMI_SUCCESS,
                "Failed to allocate memory from mm %p with key \"%s\"", mm, key);
          }

        template <class T_MemoryManager, unsigned T_Num>
          static void init_impl (T_MemoryManager * mm,
              const char      * key,
              IndirectL2Bounded        (&atomic)[T_Num])
          {
            volatile uint64_t * array;
            pami_result_t rc;

            if ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0) {
              rc = mm->memalign((void **)&array, 32, sizeof(L2BoundedCounter_t)*T_Num, key,
                  IndirectL2Bounded::counter_initialize, NULL);
            } else {
              rc = __global.l2atomicFactory.__nodescoped_mm.memalign(
                  (void **)&array, 32, sizeof(L2BoundedCounter_t)*T_Num, key,
                  IndirectL2Bounded::counter_initialize, NULL);
            }

            PAMI_assertf (rc == PAMI_SUCCESS, "Failed to allocate memory from l2 atomic node-scoped memory manager with key (\"%s\")", key);

            unsigned i;
            for (i=0; i<T_Num; i++)
            {
              atomic[i]._counter =  (L2BoundedCounter_t *)&array[i];
            }
          };



	inline void clone_impl(IndirectL2Bounded &atomic) {
		_counter = atomic._counter;
	}
#if 0
	// -------------------------------------------------------------------
	// PAMI::Counter::Interface<T> implementation
	// -------------------------------------------------------------------

	inline size_t fetch_impl() {
		return _counter->count;
	}

	inline void clear_impl() {
		L2_AtomicLoadClear(&_counter->count);
	}
#endif
	// -------------------------------------------------------------------
	// PAMI::BoundedCounter::Interface<T> implementation
	// -------------------------------------------------------------------

	inline size_t fetch_and_inc_bounded_impl() {
		return L2_AtomicLoadIncrementBounded(&_counter->count);
	}

	inline size_t fetch_and_dec_bounded_impl() {
		return L2_AtomicLoadDecrementBounded(&_counter->count);
	}

	inline size_t bound_upper_fetch_impl() {
		return _counter->upper;
	}

	inline size_t bound_upper_fetch_and_inc_impl() {
		return L2_AtomicLoadIncrement(&_counter->upper);
	}

	inline void bound_upper_clear_impl() {
		L2_AtomicLoadClear(&_counter->upper);
	}

	inline void bound_upper_set_impl(size_t value) {
		L2_AtomicStore(&_counter->upper, value);
	}

	inline size_t bound_lower_fetch_impl() {
		return _counter->lower;
	}

	inline size_t bound_lower_fetch_and_dec_impl() {
		return L2_AtomicLoadDecrement(&_counter->lower);
	}

	inline void bound_lower_clear_impl() {
		L2_AtomicLoadClear(&_counter->lower);
	}

	inline void bound_lower_set_impl(size_t value) {
		L2_AtomicStore(&_counter->lower, value);
	}

	// -------------------------------------------------------------------
	// Memory manager counter initialization function
	// -------------------------------------------------------------------

	///
	/// \brief Initialize the counter resources
	///
	/// \see PAMI::Memory::MM_INIT_FN
	///
	/* Assuming that memory returned has three consecutive uint64_t locations */
	static void counter_initialize(void *memory, size_t bytes, const char *key,
						unsigned attributes, void *cookie) {
		L2BoundedCounter_t *counter = (L2BoundedCounter_t *) memory;
		L2_AtomicLoadClear(&counter->lower);
		L2_AtomicLoadClear(&counter->count);
		L2_AtomicLoadClear(&counter->upper);
	}

	L2BoundedCounter_t *_counter;

}; // class     PAMI::BoundedCounter::BGQ::IndirectL2Bounded
}; // namespace PAMI::BoundedCounter::BGQ
}; // namespace PAMI::BoundedCounter
}; // namespace PAMI

#endif // __components_atomic_bgq_L2CounterBounded_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments
// astyle options --max-instatement-indent=79 --min-conditional-indent=2
// astyle options --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements
// astyle options --align-pointer=name --lineend=linux
//
