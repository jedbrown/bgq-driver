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
 * \file components/atomic/bgp/BgpAtomicMutex.h
 * \brief ???
 */
#ifndef __components_atomic_bgp_BgpAtomicMutex_h__
#define __components_atomic_bgp_BgpAtomicMutex_h__

#include "components/atomic/MutexInterface.h"
#include "components/memory/MemoryManager.h"

#include <spi/bgp_SPI.h>
#include <bpcore/bgp_atomic_ops.h>

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
  namespace Mutex
  {
    namespace BGP
    {
      ///
      /// \brief PAMI::Mutex::Interface implementation using bgp atomics
      ///
      /// The PAMI::Mutex::BGP::Atomic class is considered an "in place"
      /// implementation because the storage for the actual atomic resource is
      /// embedded within the class instance.
      ///
      /// Any "in place" mutex implementation may be converted to an "indirect"
      /// mutex implementation, where the atomic resource is located outside
      /// of the class instance, by using the PAMI::Mutex::Indirect<T> class
      /// instead of the native "in place" implementation.
      ///
      class Atomic : public PAMI::Mutex::Interface <PAMI::Mutex::BGP::Atomic>
      {
        public:

          friend class PAMI::Mutex::Interface <PAMI::Mutex::BGP::Atomic>;

          inline Atomic () :
             PAMI::Mutex::Interface <PAMI::Mutex::BGP::Atomic> (),
              _atomic (0)
          {};

          ~Atomic () {};

        protected:

          // -------------------------------------------------------------------
          // PAMI::Mutex::Interface<T> implementation
          // -------------------------------------------------------------------

          inline void acquire_impl ()
          {
            while (!_bgp_test_and_set((_BGP_Atomic *)&_atomic, 1));
          };

          inline void release_impl ()
          {
            _atomic = 0;
          };

          inline bool tryAcquire_impl ()
          {
            return (_bgp_test_and_set((_BGP_Atomic *)&_atomic, 1) != 0);
          };

          inline bool isLocked_impl ()
          {
            return (_atomic != 0);
          };

        private:

          volatile uint32_t _atomic __attribute__ ((aligned(8)));
      }; // class     PAMI::Mutex::BGP::Atomic
    };   // namespace PAMI::Mutex::BGP
  };     // namespace PAMI::Mutex
};       // namespace PAMI
#endif   //  __components_atomic_bgp_BgpAtomicMutex_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
