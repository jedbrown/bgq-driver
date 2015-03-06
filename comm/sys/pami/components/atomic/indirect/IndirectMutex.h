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
 * \file components/atomic/indirect/IndirectMutex.h
 * \brief ???
 */
#ifndef __components_atomic_indirect_IndirectMutex_h__
#define __components_atomic_indirect_IndirectMutex_h__

#include "components/atomic/MutexInterface.h"
#include "components/atomic/IndirectInterface.h"

namespace PAMI
{
  namespace Mutex
  {
    ///
    /// \brief Convert an "in place" mutex class into an "indirect" mutex.
    ///
    /// \tparam T Mutex implmentation class of PAMI::Mutex::Interface.
    ///
    template <class T>
    class Indirect : public PAMI::Mutex::Interface< PAMI::Mutex::Indirect<T> >,
                     public PAMI::Atomic::Indirect< PAMI::Mutex::Indirect<T> >
    {
      public:

        friend class PAMI::Mutex::Interface< PAMI::Mutex::Indirect<T> >;
        friend class PAMI::Atomic::Indirect< PAMI::Mutex::Indirect<T> >;

        static const bool indirect = true;

        inline Indirect () :
          PAMI::Mutex::Interface< PAMI::Mutex::Indirect<T> > (),
          PAMI::Atomic::Indirect< PAMI::Mutex::Indirect<T> > (),
          _mutex (NULL)
        {};

        inline ~Indirect () {};

      protected:

        // -------------------------------------------------------------------
        // PAMI::Atomic::Indirect<T> implementation
        // -------------------------------------------------------------------

        template <class T_MemoryManager>
        inline void init_impl (T_MemoryManager * mm, const char * key)
        {
          pami_result_t rc;
	  rc = mm->memalign ((void **) & _mutex,
                                           sizeof(*_mutex),
                                           sizeof(*_mutex),
                                           key,
                                           Indirect::mutex_initialize,
                                           NULL);

          PAMI_assertf (rc == PAMI_SUCCESS, "Failed to allocate memory from memory manager (%p) with key (\"%s\")", mm, key);
        };

        template <class T_MemoryManager, unsigned T_Num>
        static void init_impl (T_MemoryManager * mm,
                const char      * key,
                Indirect       (&atomic)[T_Num])
        {
          unsigned i;
          char mykey[PAMI::Memory::MMKEYSIZE];

           for (i=0; i<T_Num; i++)
           {
             sprintf(mykey, "%s-%u", key, i);
             atomic[i].init (mm, mykey);
           }

        }

        inline void clone_impl (Indirect & atomic)
        {
          _mutex = atomic._mutex;
        };

        // -------------------------------------------------------------------
        // PAMI::Mutex::Interface<T> implementation
        // -------------------------------------------------------------------

        inline void acquire_impl ()
        {
          _mutex->acquire ();
        };

        inline void release_impl ()
        {
          _mutex->release ();
        };

        inline bool tryAcquire_impl ()
        {
          return _mutex->tryAcquire ();
        };

        inline bool isLocked_impl ()
        {
          return _mutex->isLocked ();
        };

        // -------------------------------------------------------------------
        // Memory manager counter initialization function
        // -------------------------------------------------------------------

        ///
        /// \brief Initialize the mutex resources
        ///
        /// \see PAMI::Memory::MM_INIT_FN
        ///
        static void mutex_initialize (void       * memory,
                                      size_t       bytes,
                                      const char * key,
                                      unsigned     attributes,
                                      void       * cookie)
        {
          T * mutex = (T *) memory;
          new (mutex) T ();
        };

        T * _mutex;

    }; // PAMI::Mutex::Indirect<T> class
  };   // PAMI::Mutex namespace
};     // PAMI namespace

#endif // __components_atomic_indirect_IndirectMutex_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
