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
/// \file arch/i386/Ptime.h
/// \brief ???
///
#ifndef __arch_i386_Ptime_h__
#define __arch_i386_Ptime_h__

#include <pami.h>
#include <sys/time.h>
#include "arch/TimeInterface.h"
#include <stdio.h>

namespace PAMI{

class Time : public Interface::Time<Time>
{
private:
    struct cpuid_t
    {
      uint32_t eax;
      uint32_t ebx;
      uint32_t ecx;
      uint32_t edx;
    } __attribute__((packed));
public:
  inline Time () ;

  ///
  /// \brief Initialize the time object.
  ///
  inline pami_result_t init_impl (size_t dummy);

  ///
  /// \brief determine if high res timers can be used
  ///
  inline void cpuid(cpuid_t *r, uint32_t eax);

  ///
  /// \brief Calculate the clock frequency
  ///
  inline void calculateClockMhz ();

  /// \brief The processor clock in MHz.
  ///
  /// \warning This returns \b mega hertz. Do not be confused.
  ///
  inline size_t clockMHz_impl ();

  ///
  /// \brief Returns the number of "cycles" elapsed on the calling processor.
  ///
  inline unsigned long long timebase_impl ();

  ///
  /// \brief Computes the smallest clock resolution theoretically possible
  ///
  inline double tick_impl ();
  ///
  /// \brief Returns an elapsed time on the calling processor.
  ///
  inline double time_impl ();

private:
  inline uint64_t rdtscp(void);

protected:
  uint64_t _clockMHz;
  double   _sec_per_cycle;
  cpuid_t  _cpuid;
  bool     _high_res;
};

inline Time::Time () :
  Interface::Time<Time>(),
  _clockMHz(0),
  _sec_per_cycle(-1.0),
  _high_res(true)
{
#define __BIT(b) (1ULL << (b))
  // check for invariant clock
  cpuid(&_cpuid, 0x80000007);
  if((_cpuid.edx & __BIT(8)) == 0) _high_res = false;

  // check for availablilty of rdtscp
  cpuid(&_cpuid, 0x80000001);
  if((_cpuid.edx & __BIT(27)) == 0) _high_res = false;

  if(_high_res) calculateClockMhz();
  else
  {
    int rc;
    struct timespec ts;
    rc = clock_getres( CLOCK_REALTIME, &ts );
    if (!rc)
      _sec_per_cycle = ts.tv_sec + 1.0e-9 * ts.tv_nsec;
    else
      _sec_per_cycle = 1.0e-9;
    _clockMHz      = (size_t)((1.0/_sec_per_cycle)/1000000.0);
  }
};

inline void Time::calculateClockMhz ()
{
  unsigned long long t1, t2;
  struct timeval     tv1, tv2;
  double             td1, td2;
  gettimeofday(&tv1, NULL);
  t1=rdtscp();
  usleep(250000);
  gettimeofday(&tv2, NULL);
  t2=rdtscp();
  td1 = tv1.tv_sec + tv1.tv_usec / 1000000.0;
  td2 = tv2.tv_sec + tv2.tv_usec / 1000000.0;
  _sec_per_cycle = (td2 - td1) / (double)(t2 - t1);
  _clockMHz      = (size_t)((1.0/_sec_per_cycle)/1000000.0);
}

inline pami_result_t Time::init_impl (size_t dummy)
{
  return PAMI_SUCCESS;
};

inline size_t Time::clockMHz_impl ()
{
  return _clockMHz;
}

inline unsigned long long Time::timebase_impl ()
{
  if(_high_res)
    return rdtscp();
  else
  {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1e9 + ts.tv_nsec;
  }
};

inline double Time::tick_impl ()
{
  return _sec_per_cycle;
};

inline double Time::time_impl ()
{
  if(_high_res)
    return ((double)rdtscp() * _sec_per_cycle);
  else
  {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + 1.0e-9*(double)ts.tv_nsec;
  }
};

inline uint64_t Time::rdtscp(void)
{
  uint32_t lo, hi;
  asm volatile("rdtscp" : "=a"(lo), "=d"(hi) :: "ecx" );
  return (uint64_t)hi << 32 | lo;
}

inline void Time::cpuid(cpuid_t *r, uint32_t in)
{
    asm volatile("cpuid;"
            : "=a"(r->eax), "=b"(r->ebx), "=c"(r->ecx), "=d"(r->edx)
            : "a"(in));
}


};	// namespace PAMI
#endif // __arch_i386_Time_h__
