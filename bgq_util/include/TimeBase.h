/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef _TIMEBASE_H
#define _TIMEBASE_H
#include <unistd.h>      // this is required for builds on x86 with gcc3.2.2 compiler
#include <assert.h>
#include <stdio.h> // popen(), fprintf()
#include <stdlib.h> // exit()
#include <stdint.h> // uint32_t


// Access cpu time base registers.
//
class TimeBase
{
private:
   // value of cycle counter at program start
   //
   unsigned long long _offset;

   // time base frequency
   // note that this might not be the same as the
   // cpu frequency (on ppc machines, for example)
   //
   double _cyclesPerSecond;

public:
   TimeBase()
   {
      _offset = getCycles();
   #if __powerpc__
      // look in /proc/cpuinfo for a line like "timebase : 212993779"
      FILE *pipe = popen("/bin/fgrep 'timebase' </proc/cpuinfo | /bin/awk '{ print $3 }'", "r");
      if (pipe == 0 || fscanf(pipe, "%lf", &_cyclesPerSecond) != 1 || pclose(pipe) != 0)
         fprintf(stderr, "can't read timebase from /proc/cpuinfo\n"), exit(1);
   #else
      // look in /proc/cpuinfo for a line like "cpu MHz : 2392.194"
      double mhz;
      FILE *pipe = popen("/bin/fgrep 'cpu MHz' </proc/cpuinfo | /bin/awk '{ print $4 }'", "r");
      if (pipe == 0 || fscanf(pipe, "%lf", &mhz) != 1 || pclose(pipe) != 0)
         fprintf(stderr, "can't read cpu frequency from /proc/cpuinfo\n"), exit(1);
      //printf("mhz=%f\n", mhz);
      _cyclesPerSecond = mhz * 1e6;
   #endif
    //printf("cyclesPerSecond=%.0f\n", _cyclesPerSecond);
   }

   // Machine cycles since cpu reset.
   //
   inline unsigned long long
   getCycles()
   {
   #if __powerpc__
      unsigned volatile u1, u2, lo;
        do {
          asm volatile ("mftbu %0" : "=r" (u1) :);
          asm volatile ("mftb  %0" : "=r" (lo) :);
          asm volatile ("mftbu %0" : "=r" (u2) :);
        } while (u1!=u2);
      return (((unsigned long long)u2) << 32) | lo;
   #else
      ////unsigned long long val;
      ////__asm__ __volatile__("rdtsc" : "=A" (val));
      ////return val;
      uint32_t lowerPartOfValue  = 0;
      uint32_t higherPartOfValue = 0;
      // Since Intel processors can have out-of-order execution (instructions are not necessarily performed in the order they appear in the executable), 
      // |  we need to serialize to force every preceding instruction to complete before allowing the rdtsc to execute.      
   #if __x86_64__
      __asm__ __volatile__ ("xorl %%eax,%%eax \n        cpuid" ::: "%eax", "%ebx", "%ecx", "%edx");
   #else
      __asm__ __volatile__ ("xorl %%eax,%%eax \n        pushl %%ebx \n        cpuid \n        popl %%ebx" ::: "%eax", "%ecx", "%edx");
   #endif
      // Read the Time Stamp Counter (can not use "=A" because it would use %rax on x86_64).
      __asm__ __volatile__ ("rdtsc" : "=a" (lowerPartOfValue), "=d" (higherPartOfValue));
      return ((uint64_t)higherPartOfValue << 32) | (uint64_t)lowerPartOfValue;
   #endif
   }

   // Seconds since program start.
   //
   inline double
   getSeconds()
      {
      return (getCycles() - _offset) / _cyclesPerSecond;
      }

   // Milliseconds since program start.
   //
   inline double
   getMilliseconds()
      {
      return (getCycles() - _offset) / _cyclesPerSecond * 1e3;
      }

   // Delay the specified number of microseconds.
   //   Note: This is not a sleep, it does NOT release the processor for others to use.
   inline int delayMicroseconds(int iMicroSeconds)
   {
      if (iMicroSeconds > 51000)  // 10,000 microsecs = 1/100 of a second.
      {  // too large of a number - use usleep instead.
         ////printf("@ delayMicroseconds - value of %d microseconds is too big to delay for, using usleep instead\n", iMicroSeconds);
         return usleep(iMicroSeconds);
      }
      return  ( delayNanoseconds(iMicroSeconds * 1000ULL) );
   }

   // Delay the specified number of nanoseconds.
   //   Note: This is not a sleep, it does not release the processor for others to use.
   inline int delayNanoseconds(unsigned long long nanosecs)
   {
      const unsigned long long ullBillion = 1000000000ULL;
      // Save starting num of cycles (so we can minimize any delay we insert).
      unsigned long long cycles = getCycles();
      if (nanosecs > ullBillion)  // 1 second
      {
        printf("! delayNanoseconds - too many nanoseconds were specified (%llu), use a derivitive of sleep!!!\n", nanosecs);
        assert(1==0);
      }
      // Calculate number of picoseconds per cycle.
      double cyclesPerPicoSecond = (_cyclesPerSecond) / 1e12;
      double picosecsPerCycle = 1 / cyclesPerPicoSecond;
      // Calculate number of cycles to wait for specified number of nanoseconds.
      unsigned long long cyclesToWait = (unsigned long long)((nanosecs * 1000ULL) / picosecsPerCycle);
      // Calc ending number of cycles (adding 1 in case of rounding error [too long of wait is better than too short]).
      cycles += cyclesToWait + 1;
      
      // Wait for number of cycles to elapse.
      for (unsigned long long ullDeadmanCntr = 0ULL; ullDeadmanCntr < ullBillion; ++ullDeadmanCntr)
      {
        if (getCycles() >= cycles)
          return 0;
      }
      
      printf("! delayNanoseconds - iterated large number of times and the delay still has not completed!!!\n");
      assert(1==0);
      return -1;
   }
   
};

extern class TimeBase TimeBase;

#if 0
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include "TimeBase.h"

int
main(int argc, char **argv)
   {
#if __powerpc__
   printf("ppc\n");
#else
   printf("intel\n");
#endif

   class TimeBase t;

   for (unsigned i = 0; i < 20; ++i)
      {
      double start = t.getSeconds();
    //double start = t.getMilliseconds();

    //usleep(1234567);
    //usleep(1);
      sched_yield();

      double diff = t.getSeconds() - start;
    //double diff = t.getMilliseconds() - start;

      printf("%.9f\n", diff);
      }
   }
#endif

#endif
