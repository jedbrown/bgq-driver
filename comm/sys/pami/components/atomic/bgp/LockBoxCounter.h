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

#ifndef __components_atomic_bgp_LockBoxCounter_h__
#define __components_atomic_bgp_LockBoxCounter_h__

/**
 * \file components/atomic/bgp/LockBoxCounter.h
 * \brief BGP implementation of Node- or Process-scoped Counter
 *
 * LockBox is the only implementation, currently. Additional implementations
 * would be new classes, all inheriting from "Counter".
 *
 *
 */
#include <pami.h>
#include "components/atomic/CounterInterface.h"
#include "components/atomic/indirect/IndirectCounter.h"
#include "Global.h"

#include <spi/bgp_SPI.h>
#include <bpcore/bgp_atomic_ops.h>

namespace PAMI
{
  namespace Counter
  {
    namespace BGP
    {
      //
      // This class is used internally ONLY. See following classes for users
      //
      class LockBoxCounter : public PAMI::Counter::Interface<LockBoxCounter>,
                             public PAMI::Atomic::Indirect<LockBoxCounter>
      {
        public:

          friend class PAMI::Counter::Interface<LockBoxCounter>;
          friend class PAMI::Atomic::Indirect<LockBoxCounter>;

          LockBoxCounter() { _addr = NULL; }
          ~LockBoxCounter() {}

          static const bool indirect = true;

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl(T_MemoryManager *mm, const char *key)
          {
            __global.lockboxFactory.lbx_alloc(&_addr, 1,
                                              key ? PAMI::Atomic::BGP::LBX_NODE_SCOPE : PAMI::Atomic::BGP::LBX_PROC_SCOPE);
          }

          template <class T_MemoryManager, unsigned T_Num>
           static void init_impl (T_MemoryManager * mm,
                const char      * key,
                LockBoxCounter       (&atomic)[T_Num])
            {
              unsigned i;
              char mykey[PAMI::Memory::MMKEYSIZE];

              for (i=0; i<T_Num; i++)
              {
                sprintf(mykey, "%s-%u", key, i);
                atomic[i].init (mm, mykey);
              }

            }


          inline void clone_impl (LockBoxCounter & atomic)
          {
            PAMI_abortf("how do we clone the lockbox objects?\n");

            //_addr = atomic._addr;
          };

          // -------------------------------------------------------------------
          // PAMI::Counter::Interface<T> implementation
          // -------------------------------------------------------------------

          inline size_t fetch_impl()
          {
            return LockBox_Query((LockBox_Counter_t)_addr);
          }

          inline size_t fetch_and_inc_impl()
          {
            return LockBox_FetchAndInc((LockBox_Counter_t)_addr);
          }

          inline size_t fetch_and_dec_impl()
          {
            return LockBox_FetchAndDec((LockBox_Counter_t)_addr);
          }

          inline size_t fetch_and_clear_impl()
          {
            return LockBox_FetchAndClear((LockBox_Counter_t)_addr);
          }

          inline void clear_impl()
          {
            LockBox_FetchAndClear((LockBox_Counter_t)_addr);
          }

          //
          // no such thing exists for BG/P lockboxes.
          //
          //inline bool compare_and_swap_impl(size_t compare, size_t swap) {}

          void *_addr;

      }; // class     PAMI::Counter::BGP::LockBoxCounter
    };   // namespace PAMI::Counter::BGP
  };     // namespace PAMI::Counter
};       // namespace PAMI

#endif // __components_atomic_bgp_LockBoxCounter_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
