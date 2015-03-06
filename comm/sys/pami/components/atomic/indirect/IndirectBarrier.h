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
 * \file components/atomic/indirect/IndirectBarrier.h
 * \brief ???
 */
#ifndef __components_atomic_indirect_IndirectBarrier_h__
#define __components_atomic_indirect_IndirectBarrier_h__

#include "components/atomic/BarrierInterface.h"
#include "components/atomic/IndirectInterface.h"

namespace PAMI
{
  namespace Barrier
  {
    ///
    /// \brief Convert an "in place" barrier class into an "indirect" barrier.
    ///
    /// \tparam T PAMI::Barrier::Interface<T> implmentation class.
    ///
    template <class T>
    class Indirect : public PAMI::Barrier::Interface< PAMI::Barrier::Indirect<T> >,
                     public PAMI::Atomic::Indirect< PAMI::Barrier::Indirect<T> >
    {
      private:

        inline Indirect () :
          PAMI::Barrier::Interface< PAMI::Barrier::Indirect<T> > (0, false),
          PAMI::Atomic::Indirect< PAMI::Barrier::Indirect<T> > (),
          _barrier (NULL),
          _participants (0),
          _master (false)
        {};

      public:

        friend class PAMI::Barrier::Interface< PAMI::Barrier::Indirect<T> >;
        friend class PAMI::Atomic::Indirect< PAMI::Barrier::Indirect<T> >;

        static const bool indirect = true;

        inline Indirect (size_t participants, bool master) :
          PAMI::Barrier::Interface< PAMI::Barrier::Indirect<T> > (participants, master),
          PAMI::Atomic::Indirect< PAMI::Barrier::Indirect<T> > (),
          _barrier (NULL),
          _participants (participants),
          _master (master)
        {};

        inline ~Indirect () {};

      protected:

        // -------------------------------------------------------------------
        // PAMI::Atomic::Indirect<T> implementation
        // -------------------------------------------------------------------
  
        template <class T_MemoryManager>
        inline void init_impl (T_MemoryManager * mm, const char * key)
        {
          pami_result_t rc = mm->memalign ((void **) & _barrier,
                                           sizeof(*_barrier),
                                           sizeof(*_barrier),
                                           key,
                                           Indirect<T>::barrier_initialize,
                                           (void *) this);

          PAMI_assertf (rc == PAMI_SUCCESS, "Failed to allocate memory from memory manager (%p) with key (\"%s\")", mm, key);
        };

        template <class T_MemoryManager, unsigned T_Num>
        static void init_impl (T_MemoryManager * mm, const char * key, Indirect (&atomic)[T_Num])
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
          _barrier = atomic._barrier;
        };

        // -------------------------------------------------------------------
        // PAMI::Barrier::Interface<T> implementation
        // -------------------------------------------------------------------

        inline void enter_impl ()
        {
          _barrier->enter ();
        };

        template <class T_Functor>
        inline void enter_impl (T_Functor & functor)
        {
          _barrier->enter (functor);
        };

        inline void begin_impl ()
        {
          _barrier->begin ();
        };

        inline bool poll_impl ()
        {
          return _barrier->poll ();
        };

        inline void end_impl ()
        {
          _barrier->end ();
        };

        inline void dump_impl (const char * string = NULL)
        {
          _barrier->dump (string);
        };

        // -------------------------------------------------------------------
        // Memory manager barrier initialization function
        // -------------------------------------------------------------------

        ///
        /// \brief Initialize the counter resources
        ///
        /// \see PAMI::Memory::MM_INIT_FN
        ///
        static void barrier_initialize (void       * memory,
                                        size_t       bytes,
                                        const char * key,
                                        unsigned     attributes,
                                        void       * cookie)
        {
          T * barrier = (T *) memory;
          Indirect<T> * indirect = (Indirect<T> *) cookie;
          new (barrier) T (indirect->_participants, indirect->_master);
        };

        T      * _barrier;
        size_t   _participants;
        bool     _master;

    }; // PAMI::Barrier::Indirect<T> class
  };   // PAMI::Barrier namespace
};     // PAMI namespace

#endif // __components_atomic_indirect_IndirectBarrier_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
