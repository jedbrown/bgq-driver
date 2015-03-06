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
/// \file arch/a2qpx/Ptime.h
/// \brief ???
///
#ifndef __arch_a2qpx_Ptime_h__
#define __arch_a2qpx_Ptime_h__

#include <pami.h>

#include "arch/TimeInterface.h"

namespace PAMI
{
    class Time : public Interface::Time<Time>
    {
      public:

        friend class Interface::Time<Time>;

        inline Time () :
            Interface::Time<Time> (),
            _mhz(0)
        {};

      protected:

        ///
        /// \brief Initialize the time object.
        ///
        inline pami_result_t init_impl (size_t clockMHz)
        {
          _mhz = clockMHz;
          return PAMI_SUCCESS;
        };

        ///
        /// \brief The processor clock in MHz.
        ///
        /// \warning This returns \b mega hertz. Do not be confused.
        ///
        inline size_t clockMHz_impl ()
        {
          return _mhz;
        };

        ///
        /// \brief Returns the number of "cycles" elapsed on the calling processor.
        ///
        inline unsigned long long timebase_impl ()
        {
          return GetTimeBase(); // From hwi/include/bqc/A2_inlines.h
        };

        ///
        /// \brief Computes the smallest clock resolution theoretically possible
        ///
        inline double tick_impl ()
        {
          return PAMI::Time::seconds_per_cycle;
        };

        ///
        /// \brief Returns an elapsed time on the calling processor.
        ///
        inline double time_impl ()
        {
          return ((double)timebase() * PAMI::Time::seconds_per_cycle);
        };

        /// \brief BG/Q compute node processors run at 1.6ghz. This should be
        // changed when we know for sure how fast they are running at...
        static const double seconds_per_cycle;
        size_t _mhz;
    };	// class Time
};	// namespace PAMI
#endif // __arch_a2qpx_Time_h__
