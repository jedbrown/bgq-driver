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
 * \file components/atomic/noop/Noop.h
 * \brief ???
 */
#ifndef __components_atomic_noop_Noop_h__
#define __components_atomic_noop_Noop_h__

#include "components/atomic/CounterInterface.h"
#include "components/atomic/MutexInterface.h"

namespace PAMI
{
  namespace Counter
  {
    ///
    /// \brief A "noop" atomic counter and mutex
    ///
    class Noop : public PAMI::Counter::Interface <Noop>
    {
      public:

        friend class PAMI::Counter::Interface <Noop>;

        Noop () :
            PAMI::Counter::Interface <Noop> ()
        {};

        ~Noop () {};

      protected:

        // -------------------------------------------------------------------
        // PAMI::Counter::Interface<T> implementation
        // -------------------------------------------------------------------

        inline size_t fetch_impl () { return 0; };

        inline size_t fetch_and_inc_impl () { return 0; };

        inline size_t fetch_and_dec_impl () { return 0; };

        inline size_t fetch_and_clear_impl () { return 0; };

        inline void clear_impl () {};

    };
  };
  namespace Mutex
  {
    ///
    /// \brief A "noop" atomic counter and mutex
    ///
    class Noop : public PAMI::Mutex::Interface <Noop>
    {
      public:

        friend class PAMI::Mutex::Interface <Noop>;

        Noop () :
            PAMI::Mutex::Interface <Noop> ()
        {};

        ~Noop () {};

      protected:

        // -------------------------------------------------------------------
        // PAMI::Mutex::Interface<T> implementation
        // -------------------------------------------------------------------

        inline void acquire_impl () {};

        inline void release_impl () {};

        inline bool tryAcquire_impl () { return true; };

        inline bool isLocked_impl () { return false; };
    };
  };
};

#endif // __components_atomic_noop_Noop_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
