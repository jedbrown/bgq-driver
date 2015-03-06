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
 * \file components/atomic/example/PthreadMutex.h
 * \brief ???
 */

#ifndef __components_atomic_example_PthreadMutex_h__
#define __components_atomic_example_PthreadMutex_h__

namespace PAMI
{
  namespace Atomic
  {
    ///
    /// \brief Standard pthread mutex class
    ///
    template <class T>
    class PthreadMutex : public Interface::Mutex<PthreadMutex>,
                         public Interface::ProcessScope<PthreadMutex>
    {
      public:

        inline PthreadMutex  () :
          Interface::Mutex<PthreadMutex> (),
          Interface::ProcessScope<PthreadMutex> (),
          _scoped (false)
        {};

        inline ~PthreadMutex () {};

        inline void init_impl (PAMI::Memory::MemoryManager *mm)
        {
          // foo...
        };

        inline void acquire_impl ()
        {
        };

        inline void release_impl ()
        {
        }

        inline void setProcessScope_impl ()
        {
          _scoped = true;
        };

        /// \see PAMI::Atomic::Interface::ProcessScope
        bool _scoped;
    };
  };
};

#endif
