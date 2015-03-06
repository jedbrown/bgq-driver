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
 * \file components/atomic/example/LockboxMutex.h
 * \brief ???
 */

#ifndef __components_atomic_example_LockboxMutex_h__
#define __components_atomic_example_LockboxMutex_h__

namespace PAMI
{
  namespace Atomic
  {
    ///
    /// \brief Blue Gene/P lockbox mutex class
    ///
    class LockboxMutex : public Interface::Mutex<LockboxMutex>,
                         public Interface::NodeScope<LockboxMutex>,
                         public Interface::ProcessScope<LockboxMutex>
    {
      private:

        typedef enum
        {
          LOCKBOX_SCOPE_NODE = 0;
          LOCKBOX_SCOPE_PROCESS
        } lockbox_scope_t;

        lockbox_scope_t _scope;

      public:

        /// \see PAMI::Atomic::Interface::NodeScope
        /// \see PAMI::Atomic::Interface::ProcessScope
        bool _scoped;

        inline LockboxMutex  () :
          Interface::Mutex<LockboxMutex> (),
          Interface::NodeScope<LockboxMutex> (),
          Interface::ProcessScope<LockboxMutex> (),
          _scoped (false)
        {};

        inline ~LockboxMutex () {};

        inline void init_impl (PAMI::Memory::MemoryManager *mm)
        {
          __global.lockboxFactory.lbx_alloc((void **)&_mutex, 1, );
        };

        inline void acquire_impl ()
        {
        };

        inline void release_impl ()
        {
        }

        inline void setNodeScope_impl ()
        {
          _scope  = LOCKBOX_SCOPE_NODE;
          _scoped = true;
        };

        inline void setProcessScope_impl ()
        {
          _scope  = LOCKBOX_SCOPE_PROCESS;
          _scoped = true;
        };
    };
  };
};

#endif
