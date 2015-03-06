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

#ifndef __components_atomic_bgp_LockBoxMutex_h__
#define __components_atomic_bgp_LockBoxMutex_h__

/**
 * \file components/atomic/bgp/LockBoxMutex.h
 * \brief BGP implementation of Node- or Process-scoped Mutexes
 *
 * LockBox is the only implementation, currently. Additional implementations
 * would be new classes, all inheriting from "Mutex".
 */
#include "Global.h"
#include "Memory.h"
#include "common/bgp/LockBoxFactory.h"
#include "components/atomic/MutexInterface.h"
#include "components/atomic/IndirectInterface.h"

#include <spi/bgp_SPI.h>
#include <bpcore/bgp_atomic_ops.h>

#ifndef __defined__tsc__
#define __defined__tsc__
static inline unsigned long __tsc()
{
  unsigned long tsc;
  asm volatile ("mfspr %0,%1" : "=r" (tsc) : "i" (SPRN_TBRL));
  return tsc;
}
#endif /* ! __defined__tsc__ */

namespace PAMI
{
  namespace Mutex
  {
    namespace BGP
    {
      //
      // These classes are used internally ONLY. See following classes for users
      //

      class LockBoxMutex : public PAMI::Mutex::Interface<LockBoxMutex>,
                           public PAMI::Atomic::Indirect<LockBoxMutex>
      {
        public:

          friend class PAMI::Mutex::Interface<LockBoxMutex>;
          friend class PAMI::Atomic::Indirect<LockBoxMutex>;

          LockBoxMutex() { _addr = NULL; }
          ~LockBoxMutex() {}

          static const bool indirect = true;

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl (T_MemoryManager *mm, const char *key)
          {
            PAMI_assert_debugf(!_addr, "Re-init or object is in shmem");
            __global.lockboxFactory.lbx_alloc(&this->_addr, 1,
                                              key ? PAMI::Atomic::BGP::LBX_NODE_SCOPE : PAMI::Atomic::BGP::LBX_PROC_SCOPE);
          }
      
          template <class T_MemoryManager, unsigned T_Num>
          static void init_impl (T_MemoryManager * mm,
                const char      * key,
                LockBoxMutex       (&atomic)[T_Num])
          {
          unsigned i;
          char mykey[PAMI::Memory::MMKEYSIZE];

           for (i=0; i<T_Num; i++)
           {
             sprintf(mykey, "%s-%u", key, i);
             atomic[i].init (mm, mykey);
           }

          }


          inline void clone_impl (LockBoxMutex & atomic)
          {
            PAMI_abortf("how do we clone the lockbox objects?\n");

            //_addr = atomic._addr;
          };

          // -------------------------------------------------------------------
          // PAMI::Mutex::Interface<T> implementation
          // -------------------------------------------------------------------

          void acquire_impl()
          {
            LockBox_MutexLock((LockBox_Mutex_t)_addr);
            Memory::sync();
          }

          void release_impl()
          {
            LockBox_MutexUnlock((LockBox_Mutex_t)_addr);
          }

          bool tryAcquire_impl()
          {
            return (LockBox_MutexTryLock((LockBox_Mutex_t)_addr) == 0) ? true : false;
          }

          bool isLocked_impl()
          {
            return (LockBox_Query((LockBox_Counter_t)_addr) > 0) ? true : false;
          }

          void *_addr;

      }; // class LockBoxMutex


      class FairLockBoxMutex : public PAMI::Mutex::Interface<FairLockBoxMutex>,
                               public PAMI::Atomic::Indirect<FairLockBoxMutex>
      {
        public:

          friend class PAMI::Mutex::Interface<FairLockBoxMutex>;
          friend class PAMI::Atomic::Indirect<FairLockBoxMutex>;

          FairLockBoxMutex() { _addr = NULL; }
          ~FairLockBoxMutex() {}

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl (T_MemoryManager *mm, const char *key)
          {
            PAMI_assert_debugf(!_addr, "Re-init or object is in shmem");
            __global.lockboxFactory.lbx_alloc(&this->_addr, 1,
                                              key ? PAMI::Atomic::BGP::LBX_NODE_SCOPE : PAMI::Atomic::BGP::LBX_PROC_SCOPE);
          }

          inline void clone_impl (FairLockBoxMutex & atomic)
          {
            PAMI_abortf("how do we clone the lockbox objects?\n");

            //_addr = atomic._addr;
          };

          // -------------------------------------------------------------------
          // PAMI::Mutex::Interface<T> implementation
          // -------------------------------------------------------------------

          void acquire_impl()
          {
            // LockBox_MutexLock((LockBox_Mutex_t)_addr);
            //
            // There is some unknown problem with lockbox mutexes whereby
            // some contenders get "shut-out" and effectively never get
            // the mutex. The following back-off scheme seems to eliminate
            // the problem. If we don't get the lock on the first try,
            // we delay a short time to keep from swamping the lockbox.
            // We continue trying and delaying, however if we detect that
            // someone else got the lock instead of us (we see the lock value
            // decrease - or not increase), then we reduce our delay amount.
            // This means that if we wait too long, we effectively have no
            // delay, while other contenders that got the lock recently
            // will be delaying by the full amount and thus be at a greater
            // disadvantage compared to us. This imbalance of traffic seems
            // to be enough to "break the spell" and allow us to get the lock.
            //
            unsigned long lbv, lst = 0;
            unsigned long td = 127;

            while ((lbv = LockBox_FetchAndInc((LockBox_Counter_t)_addr)) != 0)
              {
                if (td && lbv <= lst) --td;

                lst = lbv;

                if (td)
                  {
                    unsigned long t0 = __tsc();

                    while (__tsc() - t0 < td);
                  }
              }
          }

          void release_impl()
          {
            LockBox_MutexUnlock((LockBox_Mutex_t)_addr);
          }

          bool tryAcquire_impl()
          {
            return (LockBox_MutexTryLock((LockBox_Mutex_t)_addr) == 0) ? true : false;
          }

          bool isLocked_impl()
          {
            return (LockBox_Query((LockBox_Counter_t)_addr) > 0) ? true : false;
          }

          void *_addr;

      }; // class     PAMI::Mutex::BGP::FairLockBoxMutex
    };   // namespace PAMI::Mutex::BGP
  };     // namespace PAMI::Mutex
};       // namespace PAMI

#endif // __components_atomic_bgp_LockBoxMutex_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
