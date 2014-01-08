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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  StopWatch.h
//! \brief Declaration and methods for bgcios::StopWatch class.

#ifndef COMMON_STOPWATCH_H
#define COMMON_STOPWATCH_H

// Includes
#include <sys/time.h>
#include <stdint.h>
#include <sstream>
#include <hwi/include/bqc/A2_inlines.h>

namespace bgcios
{

//! \brief Collect performance timings for critical operations.
//!
//! This class collects timing information in microseconds for an operation.
//! A new operation begins when the start() method is called and ends when
//! the stop() method is called.  It is the caller's responsibility to match
//! calls to the start() and stop() methods.

class StopWatch 
{
public:

   //! \brief  Default constructor.

   StopWatch() { reset(); }

   //! \brief  Reset the object back to a known state.
   //! \return Nothing.

   void reset(void)
   {
      resetTotals();
      _startTime = 0;
      _stopTime = 0;
      return;
   }

   void resetTotals(void)
   {
      _numOperations = 0;
      _totalTime = 0;
      _minTime = (uint64_t)-1;
      _maxTime = 0;
      _byteCount = 0;
      return;
   }

   //! Microseconds per second.
   static const uint64_t million = 1000000;

   //! CPU cycles per second.
   static const uint64_t cyclesPerMicrosecond = 1600;

   //! \brief  Return the time in microseconds.
   //! \return Current time in microseconds.

   inline uint64_t utime(void)
   { 
      struct timeval now;
      uint64_t temptime = GetTimeBase();
      return (temptime/cyclesPerMicrosecond);   // Frequency of bgq is 1.6 GHz 
   }

   //! \brief  Start the timer.
   //! \return Nothing.

   inline void start(void)
   {
       _startTime = utime();
      return;
   }

   //! \brief  Stop the timer.
   //! \return Nothing.

   inline void stop(void) { stop(0); }

   //! \brief  Stop the timer.
   //! \param  bytes Number of bytes (or other value) processed with this operation.
   //! \return Nothing.

   inline void stop(uint64_t bytes)
   {
       _stopTime = utime();
      uint64_t elapsedTime = _stopTime - _startTime;
      _totalTime += elapsedTime;
      if (elapsedTime < _minTime) _minTime = elapsedTime;
      if (elapsedTime > _maxTime) _maxTime = elapsedTime;
      if (bytes > 0) _byteCount += bytes;
      ++_numOperations;
      return;
   }

   //! \brief  Return the total number of operations.
   //! \return Number of operations.

   uint64_t getNumOperations(void) const { return _numOperations; }

   //! \brief  Return the total time spent on all operations.
   //! \return Total time in microseconds.

   uint64_t getTotalTime(void) const { return _totalTime; }

   //! \brief  Return the minimum time spent on a single operation.
   //! \return Minimum time in microseconds.

   uint64_t getMinTime(void) const { return _minTime; }

   //! \brief  Return the maximum time spent on a single operation.
   //! \return Maximum time in microseconds.

   uint64_t getMaxTime(void) const { return _maxTime; }

   //! \brief  Return the elapsed time of the current operation.
   //! \return Elapsed time in microseconds.

   uint64_t getElapsedTime(void) const { return _stopTime - _startTime; }

   //! \brief  Return the byte count (or other value).
   //! \return Accumulated byte count.

   uint64_t getByteCount(void) const { return _byteCount; }

private:

   uint64_t _numOperations;       //!< Number of operations performed.
   uint64_t _totalTime;           //!< Total time spent on operations.
   uint64_t _minTime;             //!< Minimum time spent on one operation.
   uint64_t _maxTime;             //!< Maximum time spent on one operation.
   uint64_t _startTime;           //!< Time current operation started.
   uint64_t _stopTime;            //!< Time current operation stopped.
   uint64_t _byteCount;           //!< Total number of bytes.

};

//! \brief StopWatch shift operator for output.

inline std::ostream& operator<<(std::ostream& os, const StopWatch& w)
{
   if (w.getNumOperations() > 0) {
      os << "num ops=" << w.getNumOperations() << " Total time="<< w.getTotalTime() <<"usec avg time=" << w.getTotalTime() / w.getNumOperations();
      os << " usecs, min time=" << w.getMinTime() << " usecs, max time=" << w.getMaxTime() << " usecs";
   }
   else {
      os << "no operations recorded";
   }
   return os;
}

} // namespace bgcios

#endif // COMMON_STOPWATCH_H


