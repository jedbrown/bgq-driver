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
 * \file components/atomic/native/NativeCounter.h
 * \brief "native" compiler builtin atomics implementation of the atomic counter interface
 */
#ifndef __components_atomic_native_NativeCounter_h__
#define __components_atomic_native_NativeCounter_h__

#include <stdint.h>

#include "Compiler.h"
#include "components/atomic/CounterInterface.h"
#include "components/memory/MemoryManager.h"
#include "NativeAtomics.h"

namespace PAMI
{
  namespace Counter
  {
    ///
    /// \brief PAMI::Counter::Interface implementation using "native" compiler builtin atomics
    ///
    /// The PAMI::Counter::Native class is considered an "in place" implementation
    /// because the storage for the actual atomic resource is embedded within
    /// the class instance.
    ///
    /// Any "in place" counter implementation may be converted to an "indirect"
    /// counter implementation, where the atomic resource is located outside
    /// of the class instance, by using the PAMI::Counter::Indirect<T> class
    /// instead of the native "in place" implementation.
    ///
    class Native : public PAMI::Counter::Interface<Native>
    {
      public:

        friend class PAMI::Counter::Interface<Native>;

        inline Native() {};

        inline ~Native() {};

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm)
	{
                (void)mm;
		return true;
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm)
	{
                (void)mm;
		return true;
	}
      
      protected:

        // -------------------------------------------------------------------
        // PAMI::Counter::Interface<T> implementation
        // -------------------------------------------------------------------

        inline size_t fetch_impl ()
        {
          return _atom.fetch();
        };

        inline size_t fetch_and_inc_impl ()
        {
          return _atom.fetch_and_add (1);
        };

        inline size_t fetch_and_dec_impl ()
        {
          return _atom.fetch_and_sub (1);
        };

        inline size_t fetch_and_clear_impl ()
        {
          return _atom.fetch_and_and (0);
        }

        inline void clear_impl ()
        {
          _atom.set(0);
        }

        PAMI::Atomic::NativeAtomic _atom;

    }; // PAMI::Counter::Native class
  }; //   PAMI::Counter namespace
}; //     PAMI namespace

#endif // __components_atomic_native_NativeCounter_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
