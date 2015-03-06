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
///
/// \file arch/TimeInterface.h
/// \brief ???
///
#ifndef __arch_TimeInterface_h__
#define __arch_TimeInterface_h__

#include <pami.h>

namespace PAMI
{
    namespace Interface
    {
      ///
      /// \param T Time template implementation class
      ///
      template <class T>
      class Time
      {
        public:

          ///
          /// \brief Initialize the time object.
          ///
          inline pami_result_t init (uint64_t clockMHz);


          ///
          /// \brief The processor clock in MHz.
          ///
          /// \warning This returns \b mega hertz. Do not be confused.
          ///
          uint64_t clockMHz ();

          ///
          /// \brief Returns the number of "cycles" elapsed on the calling processor.
          ///
          unsigned long long timebase ();

          ///
          /// \brief Computes the smallest clock resolution theoretically possible
          ///
          double tick ();


          ///
          /// \brief Returns an elapsed time on the calling processor.
          ///
          double time ();
      }; // class Time

      template <class T>
      inline pami_result_t Time<T>::init (uint64_t clockMHz)
      {
        return static_cast<T*>(this)->init_impl (clockMHz);
      }

      template <class T>
      uint64_t Time<T>::clockMHz ()
      {
        return static_cast<T*>(this)->clockMHz_impl ();
      }

      template <class T>
      unsigned long long Time<T>::timebase ()
      {
        return static_cast<T*>(this)->timebase_impl ();
      }

      template <class T>
      double Time<T>::tick ()
      {
        return static_cast<T*>(this)->tick_impl ();
      }

      template <class T>
      double Time<T>::time ()
      {
        return static_cast<T*>(this)->time_impl ();
      }
    };	// namespace Interface
};	// namespace PAMI
#endif // __arch_TimeInterface_h__
