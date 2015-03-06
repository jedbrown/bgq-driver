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

#ifndef __components_atomic_bgp_LwarxStwcxMutex_h__
#define __components_atomic_bgp_LwarxStwcxMutex_h__

/**
 * \file components/atomic/bgp/LwarxStwcxMutex.h
 * \brief BGP implementation of Node- or Process-scoped Mutexes
 */

#include "components/atomic/MutexInterface.h"

#include <spi/bgp_SPI.h>
#include <bpcore/bgp_atomic_ops.h>

namespace PAMI
{
  namespace Mutex
  {
    namespace BGP
    {
      class LwarxStwcx : public PAMI::Mutex::Interface<LwarxStwcx>
      {
          // in order to avoid excessive size due to ALIGN_L1D_CACHE alignment of
          // _BGP_Atomic, we use the recommended 8B alignment here.
          typedef struct
          {
            volatile uint32_t atom;
          } __attribute__ ((aligned(8))) _BGPLM_Atomic;

        public:

          friend class PAMI::Mutex::Interface<LwarxStwcx>;

          inline LwarxStwcx() :
            PAMI::Mutex::Interface<LwarxStwcx> ()
          {
            _addr.atom = 0;
          }

          inline ~LwarxStwcx() {};

        protected:

          inline void acquire_impl()
          {
            while (!_bgp_test_and_set((_BGP_Atomic *)&_addr, 1));
          }

          inline void release_impl()
          {
            _addr.atom = 0;
          }

          inline bool tryAcquire_impl()
          {
            return (_bgp_test_and_set((_BGP_Atomic *)&_addr, 1) != 0);
          }

          inline bool isLocked_impl()
          {
            return (_addr.atom != 0);
          }

        private:

          _BGPLM_Atomic _addr;

      }; // class     PAMI::Mutex::BGP::LwarxStwcx
    };   // namespace PAMI::Mutex::BGP
  };     // namespace PAMI::Mutex
};       // namespace PAMI
#endif   //  __components_atomic_bgp_LwarxStwcxMutex_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
