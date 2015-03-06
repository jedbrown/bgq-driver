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
 * \file components/atomic/bgq/L2Mutex.h
 * \brief BGQ L2 Atomics implementation of "in place" and "indirect" mutexes
 */
#ifndef __components_atomic_bgq_L2Mutex_h__
#define __components_atomic_bgq_L2Mutex_h__

#include "Global.h"
#include "Memory.h"
#include "components/atomic/MutexInterface.h"
#include "components/atomic/IndirectInterface.h"

#include "common/bgq/L2AtomicFactory.h"
#include <spi/include/l2/atomic.h>

namespace PAMI
{
  namespace Mutex
  {
    namespace BGQ
    {
      ///
      /// \brief PAMI::Mutex::Interface implementation using bgq L2 atomics
      ///
      /// The PAMI::Mutex::BGQ::L2 class is considered an "in place" implementation
      /// because the storage for the actual atomic resource is embedded within
      /// the class instance.
      ///
      /// Any "in place" counter implementation may be converted to an "indirect"
      /// counter implementation, where the atomic resource is located outside
      /// of the class instance, by using the PAMI::Mutex::Indirect<T> class
      /// instead of the native "in place" implementation.
      ///
      /// Alternatively, the PAMI::Counter::BGQ::IndirectL2 may be used which
      /// will allocate the L2 resources using the L2 Atomic Factory
      ///
      /// \note This "in place" class is used internally ONLY.
      ///
      class L2 : public PAMI::Mutex::Interface<L2>
      {
        public:

          friend class PAMI::Mutex::Interface<L2>;

          inline L2() {};

          inline ~L2() {};

	static inline bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
	}

	static inline bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return true; // no mm used - no init.
	}

        protected:

          // -------------------------------------------------------------------
          // PAMI::Mutex::Interface<T> implementation
          // -------------------------------------------------------------------

          void acquire_impl()
          {
            while (L2_AtomicLoadIncrement(&_counter) != 0);
          }

          void release_impl()
          {
            Memory::sync();
            L2_AtomicLoadClear(&_counter);
          }

          bool tryAcquire_impl()
          {
            return (L2_AtomicLoadIncrement(&_counter) == 0);
          }

          bool isLocked_impl()
          {
            return (_counter > 0) ? true : false;
          }

          uint64_t _counter;

      }; // class PAMI::Mutex::BGQ::L2


      class IndirectL2 : public PAMI::Mutex::Interface<IndirectL2>,
                         public PAMI::Atomic::Indirect<IndirectL2>
      {
        public:

          friend class PAMI::Mutex::Interface<IndirectL2>;
          friend class PAMI::Atomic::Indirect<IndirectL2>;

          static const bool indirect = true;

          inline IndirectL2() {};

          inline ~IndirectL2() {};

	static inline bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		// must not be shared memory.
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
	}

	static inline bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
	}

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          ///
          /// \brief Initialize the indirect L2 atomic mutex
          ///
          /// Does not use the memory manager from the user, but instead uses
          /// the BGQ-specific L2 Atomic Factory to access the L2 Memory Manager
          /// to allocate the memory.
          ///
          template <class T_MemoryManager>
          inline void init_impl(T_MemoryManager *mm, const char *key)
          {
            pami_result_t rc;
	    if ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0) {
            	rc = mm->memalign((void **)&_counter, sizeof(*_counter),
                                   sizeof(*_counter), key);
	    } else {
		//fprintf(stderr, "PAMI::Mutex::BGQ::IndirectL2: WARNING: using __global.l2atomicFactory.__nodescoped_mm\n");
            	rc = __global.l2atomicFactory.__nodescoped_mm.memalign((void **)&_counter,
                                                                    sizeof(*_counter),
                                                                    sizeof(*_counter),
                                                                    key);
	    }

            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate memory from mm %p with key \"%s\"", mm, key);
          };

          template <class T_MemoryManager, unsigned T_Num>
          inline static void init_impl(T_MemoryManager *mm, const char *key, IndirectL2 (&atomic)[T_Num])
          {
          
            volatile uint64_t * array;

              pami_result_t rc;
	    if ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0) {
            	rc = mm->memalign((void **)&array, sizeof(volatile uint64_t),
                                   sizeof(volatile uint64_t)*T_Num, key);
	    } else {
              rc = __global.l2atomicFactory.__nodescoped_mm.memalign ((void **) & array,
                  sizeof(volatile uint64_t),
                  sizeof(volatile uint64_t)*T_Num,
                  key);
              }

              PAMI_assertf (rc == PAMI_SUCCESS, "Failed to allocate memory from l2 atomic node-scoped memory manager with key (\"%s\")", key);

              unsigned i;
              for (i=0; i<T_Num; i++)
              {
                atomic[i]._counter = (volatile uint64_t *) &array[i];
              }


          }
          
          inline void clone_impl (IndirectL2 & atomic)
          {
            _counter = atomic._counter;
          };

          // -------------------------------------------------------------------
          // PAMI::Mutex::Interface<T> implementation
          // -------------------------------------------------------------------

          void acquire_impl()
          {
            while (L2_AtomicLoadIncrement(_counter) != 0);
          }

          void release_impl()
          {
            Memory::sync();
            L2_AtomicLoadClear(_counter);
          }

          bool tryAcquire_impl()
          {
            return (L2_AtomicLoadIncrement(_counter) == 0);
          }

          bool isLocked_impl()
          {
            return (*_counter > 0) ? true : false;
          }

          // -------------------------------------------------------------------
          // Memory manager counter initialization function
          // -------------------------------------------------------------------

          ///
          /// \brief Initialize the counter resources
          ///
          /// \see PAMI::Memory::MM_INIT_FN
          ///
          static void counter_initialize (void       * memory,
                                          size_t       bytes,
                                          const char * key,
                                          unsigned     attributes,
                                          void       * cookie)
          {
            volatile uint64_t * counter = (volatile uint64_t *) memory;
            L2_AtomicLoadClear(counter);
          };

          uint64_t * _counter;

      }; // class PAMI::Mutex::BGQ::IndirectL2
    }; // BGQ namespace
  }; // Mutex namespace
}; // PAMI namespace

#endif // __components_atomic_bgq_L2Mutex_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
