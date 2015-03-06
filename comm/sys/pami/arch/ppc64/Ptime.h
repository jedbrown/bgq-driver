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
/// \file arch/ppc64/Ptime.h
/// \brief ???
///
#ifndef __arch_ppc64_Ptime_h__
#define __arch_ppc64_Ptime_h__

#include <pami.h>
#include <sys/time.h>
#include "arch/TimeInterface.h"
#include <stdio.h>

#define SPRN_TBRU 0x10D
#define SPRN_TBRL 0x10C

namespace PAMI
{
  static inline uint64_t tb()
  {
    unsigned temp;
    union
    {
      struct { unsigned hi, lo; } w;
      uint64_t d;
    } result;

    do
      {
        asm volatile ("mfspr %0,%1" : "=r" (temp)        : "i" (SPRN_TBRU));
        asm volatile ("mfspr %0,%1" : "=r" (result.w.lo) : "i" (SPRN_TBRL));
        asm volatile ("mfspr %0,%1" : "=r" (result.w.hi) : "i" (SPRN_TBRU));
      }
    while (temp != result.w.hi);

    return result.d;
  }

  static uint64_t timeGetTime( void )
  {
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec * 1000000ULL + tv.tv_usec;
  }

  class Time : public Interface::Time<Time>
  {
    public:

      inline Time () :
          Interface::Time<Time>(),
          _clockMHz(0)
      {
      };

      ///
      /// \brief Initialize the time object.
      ///
      inline pami_result_t init_impl (uint64_t dummy)
      {
        (void)dummy;
        _clockMHz      = clockMHz();
        _sec_per_cycle = 1.0 / ((double)_clockMHz * 1000000.0);

        if (_clockMHz == -1ULL)
          return PAMI_ERROR;
        else
          return PAMI_SUCCESS;
      };

      ///
      /// \brief The processor clock in MHz.
      ///
      /// \warning This returns \b mega hertz. Do not be confused.
      ///
      uint64_t clockMHz_impl ()
      {
        if (_clockMHz == 0.0)
          {
            uint64_t sampleTime = 100ULL; //sample time in usec
            uint64_t timeStart = 0ULL, timeStop = 0ULL;
            uint64_t startBase = 0ULL, endBase = 0ULL;
            uint64_t overhead = 0ULL, tbf = 0ULL, tbi = 0ULL;
            uint64_t ticks = 0ULL;
            int      iter = 0ULL;

            do
              {
                tbi = tb();
                tbf = tb();
                tbi = tb();
                tbf = tb();

                overhead = tbf - tbi;
                timeStart = timeGetTime();

                while (timeGetTime() == timeStart)
                  timeStart = timeGetTime();

                while (1)
                  {
                    timeStop = timeGetTime();

                    if ((timeStop - timeStart) > 1)
                      {
                        startBase = tb();
                        break;
                      }
                  }

                timeStart = timeStop;

                while (1)
                  {
                    timeStop = timeGetTime();

                    if ((timeStop - timeStart) > sampleTime)
                      {
                        endBase = tb();
                        break;
                      }
                  }

                ticks = ((endBase - startBase) + (overhead));
                iter++;

                if (iter == 10ULL)
                  {
                    fprintf(stderr, "Warning: unable to initialize high resolution timer.\n");
                    return -1;
                  }
              }
            while (endBase <= startBase);

            return (uint64_t) (ticks / sampleTime);
          }
        else
          return _clockMHz;
      }
      ///
      /// \brief Returns the number of "cycles" elapsed on the calling processor.
      ///
      uint64_t timebase_impl ()
      {
        return tb();
      };

      ///
      /// \brief Computes the smallest clock resolution theoretically possible
      ///
      double tick_impl ()
      {
        return _sec_per_cycle;
      };

      ///
      /// \brief Returns an elapsed time on the calling processor.
      ///
      double time_impl ()
      {
        return ((double)tb() * _sec_per_cycle);
      };

    protected:
      uint64_t _clockMHz;
      double _sec_per_cycle;
  };	// class Time
};	// namespace PAMI
#undef SPRN_TBRU
#undef SPRN_TBRL
#endif // __arch_ppc64_Time_h__
