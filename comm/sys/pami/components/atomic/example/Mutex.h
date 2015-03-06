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
 * \file components/atomic/example/Mutex.h
 * \brief ???
 */

#ifndef __components_atomic_example_Mutex_h__
#define __components_atomic_example_Mutex_h__

namespace PAMI
{
  namespace Atomic
  {
    namespace Interface
    {
      ///
      /// \brief Mutex interface class
      ///
      template <class T>
      class Mutex
      {
        public:

          ///
          /// \brief  Allocate and initialize the mutex
          ///
          inline void init();

          ///
          /// \brief  Acquire a lock atomically
          ///
          inline void acquire();

          ///
          /// \brief  Release a lock atomically
          ///
          inline void release();
      };
    };
  };
};

template <class T>
inline void PAMI::Atomic::Interface::Mutex<T>::init()
{
  static_cast<T*>(this)->init_impl();
}

template <class T>
inline void PAMI::Atomic::Interface::Mutex<T>::acquire()
{
  static_cast<T*>(this)->acquire_impl();
}

template <class T>
inline void PAMI::Atomic::Interface::Mutex<T>::release()
{
  static_cast<T*>(this)->release_impl();
}

#endif
