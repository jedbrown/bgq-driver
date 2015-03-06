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
 * \file components/atomic/bgp/BgpAtomicCounter.h
 * \brief ???
 */

#ifndef __components_atomic_bgp_BgpAtomicCounter_h__
#define __components_atomic_bgp_BgpAtomicCounter_h__

#include "components/atomic/CounterInterface.h"

#include <spi/bgp_SPI.h>
#include <bpcore/bgp_atomic_ops.h>

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
  namespace Counter
  {
    namespace BGP
    {
      ///
      /// \brief PAMI::Counter::Interface implementation using bgp atomics
      ///
      /// The PAMI::Counter::BGP::Atomic class is considered an "in place"
      /// implementation because the storage for the actual atomic resource is
      /// embedded within the class instance.
      ///
      /// Any "in place" counter implementation may be converted to an "indirect"
      /// counter implementation, where the atomic resource is located outside
      /// of the class instance, by using the PAMI::Counter::Indirect<T> class
      /// instead of the native "in place" implementation.
      ///
      class Atomic : public PAMI::Counter::Interface <PAMI::Counter::BGP::Atomic>
      {
        public:

          friend class PAMI::Counter::Interface <PAMI::Counter::BGP::Atomic>;

          inline Atomic () :
              PAMI::Counter::Interface <PAMI::Counter::BGP::Atomic> (),
              _atomic (0)
          {};

          inline ~Atomic () {};

        protected:

          // -------------------------------------------------------------------
          // PAMI::Counter::Interface<T> implementation
          // -------------------------------------------------------------------

          inline size_t fetch_impl()
          {
            return _bgp_fetch_and_add((_BGP_Atomic *)&_atomic, 0);
          };

          inline size_t fetch_and_inc_impl()
          {
            return _bgp_fetch_and_add((_BGP_Atomic *)&_atomic, 1);
          }

          inline size_t fetch_and_dec_impl()
          {
            return _bgp_fetch_and_add((_BGP_Atomic *)&_atomic, -1);
          }

          inline size_t fetch_and_clear_impl()
          {
            return _bgp_fetch_and_and((_BGP_Atomic *)&_atomic, 0);
          }

          inline void clear_impl()
          {
            _atomic = 0;
          }

        private:

          volatile uint32_t _atomic __attribute__ ((aligned(8)));
      }; // class     PAMI::Counter::BGP::Atomic
    };   // namespace PAMI::Counter::BGP
  };     // namespace PAMI::Counter
};       // namespace PAMI
#endif   // __components_atomic_bgp_BgpAtomicCounter_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
