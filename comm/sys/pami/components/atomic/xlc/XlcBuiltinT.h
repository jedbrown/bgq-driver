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
 * \file components/atomic/xlc/XlcBuiltinT.h
 * \brief ???
 */

#ifndef __components_atomic_xlc_XlcBuiltinT_h__
#define __components_atomic_xlc_XlcBuiltinT_h__

#include "components/atomic/CounterInterface.h"
#include "util/common.h"

namespace PAMI
{
  namespace Counter
  {
    ///
    /// \brief PAMI::Counter::Interface implementation using xlc builtin atomics
    ///
    /// The PAMI::Counter::Xlc class is considered an "in place" implementation
    /// because the storage for the actual atomic resource is embedded within
    /// the class instance.
    ///
    /// Any "in place" counter implementation may be converted to an "indirect"
    /// counter implementation, where the atomic resource is located outside
    /// of the class instance, by using the PAMI::Counter::Indirect<T> class
    /// instead of the native "in place" implementation.
    ///
    template <class T>
    class XlcBuiltinT : public PAMI::Counter::Interface <XlcBuiltinT < T > >
    {
      public:

        friend class PAMI::Counter::Interface <XlcBuiltinT < T > >;

        XlcBuiltinT () :
            PAMI::Counter::Interface <XlcBuiltinT < T > > ()
        {
          COMPILE_TIME_ASSERT(sizeof(XlcBuiltinT) == sizeof(T));
        };

        ~XlcBuiltinT () {};

      protected:

        // -------------------------------------------------------------------
        // PAMI::Counter::Interface<T> implementation
        // -------------------------------------------------------------------

        inline T fetch_impl ()
        {
          return _atom;
        };

        inline T fetch_and_inc_impl ()
        {
          return __fetch_and_add((volatile int *)&_atom, 1);
        };

        inline T fetch_and_dec_impl ()
        {
          return __fetch_and_add ((volatile int *)&_atom, -1);
        };

        inline T fetch_and_clear_impl ()
        {
          return __fetch_and_and((unsigned int *)(&_atom), 0);
        };

        inline void clear_impl ()
        {
          _atom = 0;
        };

        // using  Interface::Counter<XlcBuiltinT < T > >::fetch_and_add;
        inline T fetch_and_add(T val)
        {
          return __fetch_and_add((volatile int *)&_atom, val);
        }

        volatile T _atom;
    };

#ifdef __64BIT__
    template <>
    inline long XlcBuiltinT<long>::fetch_and_inc_impl ()
    {
      return __fetch_and_addlp(&_atom, 1);
    };

    template <>
    inline long XlcBuiltinT<long>::fetch_and_dec_impl ()
    {
      return __fetch_and_addlp(&_atom, -1);
    };

    template<>
    inline long XlcBuiltinT<long>::fetch_and_clear_impl()
    {
      return __fetch_and_andlp((volatile unsigned long *)&_atom, 0);
    };


    template <>
    inline long XlcBuiltinT<long>::fetch_and_add(long val)
    {
      return __fetch_and_addlp(&_atom, val);
    }

#endif
  };
};
#endif // __components_atomic_xlc_XlcBuiltinT_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
