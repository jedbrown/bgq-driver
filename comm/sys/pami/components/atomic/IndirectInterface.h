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
#ifndef __components_atomic_IndirectInterface_h__
#define __components_atomic_IndirectInterface_h__

///  \file components/atomic/IndirectInterface.h
///  \brief ???

#include "components/memory/MemoryManager.h"

namespace PAMI
{
  namespace Atomic
  {
    ///
    /// \brief Indirect atomic object interface
    ///
    /// This is an interface class and may not be directly instantiated.
    ///
    /// \param T  Atomic interface implementation class
    ///
    template <class T>
    class Indirect
    {
      protected:

        inline Indirect () {};

        inline ~Indirect () {};

      public:

        static const bool indirect = true;

        template <class T_MemoryManager>
        inline void init (T_MemoryManager * mm, const char * key);

        template <class T_MemoryManager, unsigned T_Num>
        static void init(T_MemoryManager * mm, const char* key, T (&atomic)[T_Num]);

        inline void clone (T & atomic);

    }; // class PAMI::Atomic::Indirect

    template <class T>
    template <class T_MemoryManager>
    inline void Indirect<T>::init(T_MemoryManager * mm, const char * key)
    {
      static_cast<T*>(this)->init_impl(mm, key);
    }

    template <class T>
    template <class T_MemoryManager, unsigned T_Num>
     void Indirect<T>::init(T_MemoryManager * mm, const char* key,
                T (&atomic)[T_Num])
    {
      T::init_impl(mm, key, atomic);
    }

    template <class T>
    inline void Indirect<T>::clone(T & atomic)
    {
      static_cast<T*>(this)->clone_impl(atomic);
    }
  }; // namespace Atomic
}; // namespace PAMI

#endif // __components_atomic_IndirectInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
