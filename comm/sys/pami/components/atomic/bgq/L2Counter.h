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
 * \file components/atomic/bgq/L2Counter.h
 * \brief BG/Q L2 Atomics implementation of node- and process-scoped atomic counters
 */
#ifndef __components_atomic_bgq_L2Counter_h__
#define __components_atomic_bgq_L2Counter_h__

#include "Global.h"
#include "components/atomic/CounterInterface.h"
#include "components/atomic/IndirectInterface.h"
#include "util/common.h"

#include "common/bgq/L2AtomicFactory.h"
#include <spi/include/l2/atomic.h>

namespace PAMI
{
  namespace Counter
  {
    namespace BGQ
    {
      ///
      /// \brief PAMI::Counter::Interface implementation using bgq L2 atomics
      ///
      /// The PAMI::Counter::BGQ::L2 class is considered an "in place" implementation
      /// because the storage for the actual atomic resource is embedded within
      /// the class instance.
      ///
      /// Any "in place" counter implementation may be converted to an "indirect"
      /// counter implementation, where the atomic resource is located outside
      /// of the class instance, by using the PAMI::Counter::Indirect<T> class
      /// instead of the native "in place" implementation.
      ///
      /// Alternatively, the PAMI::Counter::BGQ::IndirectL2 may be used which
      /// will allocate the L2 resources using the L2 Atomic Factory
      ///
      /// \note This "in place" class is used internally ONLY.
      ///
      class L2 : public PAMI::Counter::Interface<L2>
      {
        public:

          friend class PAMI::Counter::Interface<L2>;

          inline L2 () :
              PAMI::Counter::Interface<L2> (),
              _counter()
          {};

          inline ~L2() {};

          static inline bool checkCtorMm(PAMI::Memory::MemoryManager *mm)
          {
            return ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
          }

          static inline bool checkDataMm(PAMI::Memory::MemoryManager *mm)
          {
            return true; // no mm used - no init.
          }

        protected:

          // -------------------------------------------------------------------
          // PAMI::Counter::Interface<T> implementation
          // -------------------------------------------------------------------

          inline size_t fetch_impl()
          {
            return _counter;
          }

          inline size_t fetch_and_inc_impl()
          {
            return L2_AtomicLoadIncrement(&_counter);
          }

          inline size_t fetch_and_dec_impl()
          {
            return L2_AtomicLoadDecrement(&_counter);
          }

          inline size_t fetch_and_clear_impl()
          {
            return L2_AtomicLoadClear(&_counter);
          }

          inline void clear_impl()
          {
            L2_AtomicLoadClear(&_counter);
          }

          volatile uint64_t _counter;

      }; // class PAMI::Counter::BGQ::L2

      // 
      // Use of this class in a wakeup address compare (WAC) range
      // is supported.  It uses normal loads to primt the L1 cache
      // for the wakeup unit.  If this counter is not in the
      // WAC range, use class IndirectL2NoWakeup, which uses L2
      // atomic loads and should be faster.
      //
      class IndirectL2 : public PAMI::Counter::Interface<IndirectL2>,
          public PAMI::Atomic::Indirect<IndirectL2>
      {
        public:

          friend class PAMI::Counter::Interface<IndirectL2>;
          friend class PAMI::Atomic::Indirect<IndirectL2>;

          static const bool indirect = true;

          inline IndirectL2 () :
              PAMI::Counter::Interface<IndirectL2> (),
              _counter(NULL)
          {};

          inline ~IndirectL2() {};

          static inline bool checkCtorMm(PAMI::Memory::MemoryManager *mm)
          {
            // must not be shared memory.
            return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
          }

          static inline bool checkDataMm(PAMI::Memory::MemoryManager *mm)
          {
            return ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
          }

          inline void set (volatile uint64_t * addr)
          {
            _counter = addr;
          };

        protected:

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
          inline void init_impl(T_MemoryManager *mm, const char *key)
          {
            pami_result_t rc;

            if ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0)
              {
                rc = mm->memalign((void **) & _counter, sizeof(*_counter),
                                  sizeof(*_counter), key);
              }
            else
              {
                void * __counter = NULL;
                rc = __global.l2atomicFactory.__nodescoped_mm.memalign(&__counter,
                                                                       sizeof(*_counter),
                                                                       sizeof(*_counter),
                                                                       key);
                _counter = (volatile uint64_t *)__counter;
              }

            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate memory from mm %p with key \"%s\"", mm, key);
          };

          template <class T_MemoryManager, unsigned T_Num>
            static void init_impl (T_MemoryManager * mm,
                const char      * key,
                IndirectL2        (&atomic)[T_Num])
            {
              volatile uint64_t * array;

              pami_result_t rc;
              rc = __global.l2atomicFactory.__nodescoped_mm.memalign ((void **) & array,
                  sizeof(volatile uint64_t),
                  sizeof(volatile uint64_t)*T_Num,
                  key);

              PAMI_assertf (rc == PAMI_SUCCESS, "Failed to allocate memory from l2 atomic node-scoped memory manager with key (\"%s\")", key);


              unsigned i;
              for (i=0; i<T_Num; i++)
              {
                atomic[i]._counter = (volatile uint64_t *) &array[i];
              }
            };


          inline void clone_impl (IndirectL2 & atomic)
          {
            _counter = atomic._counter;
          };

          // -------------------------------------------------------------------
          // PAMI::Counter::Interface<T> implementation
          // -------------------------------------------------------------------

          inline size_t fetch_impl()
          {
            // A normal load of this counter performs slower than an 
            // L2_AtomicLoad.  But, a normal load is needed to prime the L1
            // cache line for the wakeup unit when this counter is in
            // the Wakeup Address Compare (WAC) range.
            return *_counter;
          }

          inline size_t fetch_and_inc_impl()
          {
            return L2_AtomicLoadIncrement(_counter);
          }

          inline size_t fetch_and_dec_impl()
          {
            return L2_AtomicLoadDecrement(_counter);
          }

          inline size_t fetch_and_clear_impl()
          {
            return L2_AtomicLoadClear(_counter);
          }

          inline void clear_impl()
          {
            L2_AtomicLoadClear(_counter);
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

          volatile uint64_t * _counter;

      }; // class     PAMI::Counter::BGQ::IndirectL2

      // CAUTION: Use of this class in a wakeup address compare (WAC) range
      // is not supported.  The fetch_impl() function does a L2 atomic
      // load which is faster than a normal load, but it doesn't 
      // prime the L1 cache with the counter, so it will not wake up.
      // If wakeup is needed, use the IndirectL2 class.
      class IndirectL2NoWakeup : public PAMI::Counter::Interface<IndirectL2NoWakeup>,
          public PAMI::Atomic::Indirect<IndirectL2NoWakeup>
      {
        public:

          friend class PAMI::Counter::Interface<IndirectL2NoWakeup>;
          friend class PAMI::Atomic::Indirect<IndirectL2NoWakeup>;

          static const bool indirect = true;

          inline IndirectL2NoWakeup () :
              PAMI::Counter::Interface<IndirectL2NoWakeup> (),
              _counter(NULL)
          {};

          inline ~IndirectL2NoWakeup() {};

          static inline bool checkCtorMm(PAMI::Memory::MemoryManager *mm)
          {
            // must not be shared memory.
            return ((mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) == 0);
          }

          static inline bool checkDataMm(PAMI::Memory::MemoryManager *mm)
          {
            return ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
          }

          inline void set (volatile uint64_t * addr)
          {
            _counter = addr;
          };

        protected:

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
          inline void init_impl(T_MemoryManager *mm, const char *key)
          {
            pami_result_t rc;

            if ((mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0)
              {
                rc = mm->memalign((void **) & _counter, sizeof(*_counter),
                                  sizeof(*_counter), key);
              }
            else
              {
                void * __counter = NULL;
                rc = __global.l2atomicFactory.__nodescoped_mm.memalign(&__counter,
                                                                       sizeof(*_counter),
                                                                       sizeof(*_counter),
                                                                       key);
                _counter = (volatile uint64_t *)__counter;
              }

            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate memory from mm %p with key \"%s\"", mm, key);
          };

          template <class T_MemoryManager, unsigned T_Num>
            static void init_impl (T_MemoryManager * mm,
                const char      * key,
                IndirectL2NoWakeup        (&atomic)[T_Num])
            {
              volatile uint64_t * array;

              pami_result_t rc;
              rc = __global.l2atomicFactory.__nodescoped_mm.memalign ((void **) & array,
                  sizeof(volatile uint64_t),
                  sizeof(volatile uint64_t)*T_Num,
                  key);

              PAMI_assertf (rc == PAMI_SUCCESS, "Failed to allocate memory from l2 atomic node-scoped memory manager with key (\"%s\")", key);


              unsigned i;
              for (i=0; i<T_Num; i++)
              {
                atomic[i]._counter = (volatile uint64_t *) &array[i];
              }
            };


          inline void clone_impl (IndirectL2NoWakeup & atomic)
          {
            _counter = atomic._counter;
          };

          // -------------------------------------------------------------------
          // PAMI::Counter::Interface<T> implementation
          // -------------------------------------------------------------------

          inline size_t fetch_impl()
          {
            // A normal load of this counter performs slower than an 
            // L2_AtomicLoad.  A normal load is needed to prime the L1
            // cache line for the wakeup unit when this counter is in
            // the Wakeup Address Compare (WAC) range.
            // This class is not for use with wakeup, so we can safely
            // use an L2 atomic load to improve performance.
            return L2_AtomicLoad(_counter);
            /* return *_counter; */
          }

          inline size_t fetch_and_inc_impl()
          {
            return L2_AtomicLoadIncrement(_counter);
          }

          inline size_t fetch_and_dec_impl()
          {
            return L2_AtomicLoadDecrement(_counter);
          }

          inline size_t fetch_and_clear_impl()
          {
            return L2_AtomicLoadClear(_counter);
          }

          inline void clear_impl()
          {
            L2_AtomicLoadClear(_counter);
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

          volatile uint64_t * _counter;

      }; // class     PAMI::Counter::BGQ::IndirectL2NoWakeup

    };   // namespace PAMI::Counter::BGQ
  };     // namespace PAMI::Counter
};       // namespace PAMI

#endif // __components_atomic_bgq_L2Counter_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
