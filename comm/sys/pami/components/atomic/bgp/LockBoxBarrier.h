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

#ifndef __components_atomic_bgp_LockBoxBarrier_h__
#define __components_atomic_bgp_LockBoxBarrier_h__

/**
 * \file components/atomic/bgp/LockBoxBarrier.h
 * \brief LockBox implementation of a Node-scoped Barrier
 *
 * This implementation is different from, and incompatible with, the
 * BGP SPI LockBox_Barrier implementation. Support for core
 * granularity barriers is added here, in a way that makes the barrier
 * code common for all granularities.
 */
#include "common/bgp/LockBoxFactory.h"
#include "components/atomic/BarrierInterface.h"
#include "Global.h"
#include <spi/bgp_SPI.h>

namespace PAMI
{
  namespace Barrier
  {
    namespace BGP
    {
      /*
       * This class cannot be used directly. The super class must allocate the
       * particular type of lockbox based on desired scope.
       */
      class _LockBoxBarrier
      {
        private:
          /**
           * \brief Base structure for lockbox barrier
           *
           * This houses 5 lockboxes which are used to implement
           * a barrier.
           */
          struct LockBox_Barrier_s
          {
            union
            {
              uint32_t *lkboxes[5]; /**< access all 5 lockboxes */
              struct
              {
                LockBox_Counter_t ctrl_lock; /**< the control lockbox (phase) */
                LockBox_Counter_t lock[2]; /**< lock lockboxes for all phases */
                LockBox_Counter_t status[2]; /**< status lockboxes for all phases */
              } lbx_s;
            } lbx_u;
            uint8_t master;    /**< master participant */
            uint8_t coreshift; /**< convert core to process for comparing to master */
            uint8_t nparties;  /**< number of participants */
            uint8_t _pad;      /**< pad to int */
          };
#define lbx_lkboxes	lbx_u.lkboxes		/**< shortcut for lkboxes */
#define lbx_ctrl_lock	lbx_u.lbx_s.ctrl_lock	/**< shortcut for ctrl_lock */
#define lbx_lock	lbx_u.lbx_s.lock	/**< shortcut for lock */
#define lbx_status	lbx_u.lbx_s.status	/**< shortcut for status */
        protected:
          inline _LockBoxBarrier () :
            _data (NULL),
            _active (false),
            _initialized (false)
          {};

          inline ~_LockBoxBarrier () {};

          // -------------------------------------------------------------------
          // PAMI::Barrier::Interface<T> implementation
          // -------------------------------------------------------------------

          inline void enter_impl()
          {
            begin_impl ();
            while (poll_impl ());
            end_impl ();
          }

          template <class T_Functor>
          inline void enter_impl(T_Functor & functor)
          {
            begin_impl ();
            while (poll_impl ()) functor ();
            end_impl ();
          }

          inline void begin_impl()
          {
            PAMI_assert(_initialized == true);
            PAMI_assert(_active == false);

            uint32_t lockup;
            _bgp_msync();
            lockup = LockBox_Query(_barrier.lbx_ctrl_lock);
            LockBox_FetchAndInc(_barrier.lbx_lock[lockup]);
            _data = (void*)lockup;
            _active = true;
          }

          inline bool poll_impl()
          {
            PAMI_assert(_initialized == true);
            PAMI_assert(_active == true);

            uint32_t lockup, value;
            lockup = (unsigned)_data;

            if (LockBox_Query(_barrier.lbx_lock[lockup]) < _barrier.nparties)
              {
                return true;
              }

            // All cores have participated in the barrier
            // We need all cores to block until checkin
            // to clear the lock atomically
            LockBox_FetchAndInc(_barrier.lbx_lock[lockup]);

            do
              {
                value = LockBox_Query(_barrier.lbx_lock[lockup]);
              }
            while (value > 0 && value < (unsigned)(2 * _barrier.nparties));

            if ((Kernel_PhysicalProcessorID() >> _barrier.coreshift) == _barrier.master)
              {
                if (lockup)
                  {
                    LockBox_FetchAndDec(_barrier.lbx_ctrl_lock);
                  }
                else
                  {
                    LockBox_FetchAndInc(_barrier.lbx_ctrl_lock);
                  }

                LockBox_FetchAndClear(_barrier.lbx_status[lockup]);
                LockBox_FetchAndClear(_barrier.lbx_lock[lockup]);
              }
            else
              {
                // wait until master releases the barrier by clearing the lock
                while (LockBox_Query(_barrier.lbx_lock[lockup]) > 0);
              }

            _active = false;
            return false;
          }

          inline void end_impl ()
          {
            PAMI_assert(_initialized == true);
            //PAMI_assert(_active == false);   // ???
            // noop
          };

          LockBox_Barrier_s   _barrier;
          void              * _data;
          bool                _active;
          bool                _initialized;

      }; // class _LockBoxBarrier

      class LockBoxNodeCoreBarrier :
          public PAMI::Barrier::Interface<LockBoxNodeCoreBarrier>,
          public PAMI::Atomic::Indirect<LockBoxNodeCoreBarrier>,
          public _LockBoxBarrier
      {
        public:

          friend class PAMI::Barrier::Interface<LockBoxNodeCoreBarrier>;
          friend class PAMI::Atomic::Indirect<LockBoxNodeCoreBarrier>;

          inline LockBoxNodeCoreBarrier (size_t participants, bool master) :
            PAMI::Barrier::Interface<LockBoxNodeCoreBarrier> (0, false)
          {};

          ~LockBoxNodeCoreBarrier() {}

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl (T_MemoryManager *mm, const char *key)
          {
            // For core-granularity, everything is
            // a core number. Assume the master core
            // is the lowest-numbered core in the
            // process.
            /** \todo #warning take master cue from params? */
            _barrier.master = __global.lockboxFactory.masterProc() << __global.lockboxFactory.coreShift();
            _barrier.coreshift = 0;
            _barrier.nparties = __global.lockboxFactory.numCore();
            __global.lockboxFactory.lbx_alloc((void **)_barrier.lbx_lkboxes, 5,
                                              key ? PAMI::Atomic::BGP::LBX_NODE_SCOPE : PAMI::Atomic::BGP::LBX_PROC_SCOPE);
            _initialized = true;
          }

        template <class T_MemoryManager, unsigned T_Num>
        static void init_impl (T_MemoryManager * mm,
                const char      * key,
                LockBoxNodeCoreBarrier       (&atomic)[T_Num])
        {
          unsigned i;
          char mykey[PAMI::Memory::MMKEYSIZE];

           for (i=0; i<T_Num; i++)
           {
             sprintf(mykey, "%s-%u", key, i);
             atomic[i].init (mm, mykey);
           }

        }

      

          inline void clone_impl (LockBoxNodeCoreBarrier & atomic)
          {
            PAMI_abortf("how do we clone the lockbox objects?\n");

            //_barrier.master = atomic._barrier.master;
            //_barrier.coreshift = atomic._barrier.coreshift;
            //_barrier.nparties = atomic._barrier.nparties;
          };

      }; // class LockBoxNodeCoreBarrier

      class LockBoxNodeProcBarrier :
          public PAMI::Barrier::Interface<LockBoxNodeProcBarrier>,
          public PAMI::Atomic::Indirect<LockBoxNodeProcBarrier>,
          public _LockBoxBarrier
      {
        public:

          friend class PAMI::Barrier::Interface<LockBoxNodeProcBarrier>;
          friend class PAMI::Atomic::Indirect<LockBoxNodeProcBarrier>;

          inline LockBoxNodeProcBarrier(size_t participants, bool master) :
            PAMI::Barrier::Interface<LockBoxNodeProcBarrier> (0, false)
          {};

          inline ~LockBoxNodeProcBarrier() {};

        protected:

          // -------------------------------------------------------------------
          // PAMI::Atomic::Indirect<T> implementation
          // -------------------------------------------------------------------

          template <class T_MemoryManager>
          inline void init_impl(T_MemoryManager *mm, const char *key)
          {
            // For proc-granularity, must convert
            // between core id and process id,
            // and only one core per process will
            // participate.
            _barrier.master = __global.lockboxFactory.coreXlat(__global.lockboxFactory.masterProc()) >> __global.lockboxFactory.coreShift();
            _barrier.coreshift = __global.lockboxFactory.coreShift();
            _barrier.nparties = __global.lockboxFactory.numProc();
            __global.lockboxFactory.lbx_alloc((void **)_barrier.lbx_lkboxes, 5,
                                              key ? PAMI::Atomic::BGP::LBX_NODE_SCOPE : PAMI::Atomic::BGP::LBX_PROC_SCOPE);
            _initialized = true;
          }

          template <class T_MemoryManager, unsigned T_Num>
            static void init_impl (T_MemoryManager * mm,
                const char      * key,
                LockBoxNodeProcBarrier       (&atomic)[T_Num])
            {
              unsigned i;
              char mykey[PAMI::Memory::MMKEYSIZE];

              for (i=0; i<T_Num; i++)
              {
                sprintf(mykey, "%s-%u", key, i);
                atomic[i].init (mm, mykey);
              }

            }


          inline void clone_impl (LockBoxNodeCoreBarrier & atomic)
          {
            PAMI_abortf("how do we clone the lockbox objects?\n");

            //_barrier.master = atomic._barrier.master;
            //_barrier.coreshift = atomic._barrier.coreshift;
            //_barrier.nparties = atomic._barrier.nparties;
          };

      }; // class LockBoxNodeProcBarrier
    }; // BGP namespace
  }; // Barrier namespace
}; // PAMI namespace

#endif // __pami_bgp_lockboxbarrier_h__
