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

//! \file  sysiod/Job.h
//! \brief Declaration and inline methods for bgcios::sysio::Job class.

#ifndef SYSIO_JOB_H
#define SYSIO_JOB_H

// Includes
#include <ramdisk/include/services/common/StopWatch.h>
#include <string>
#include <tr1/memory>
#include <list>

namespace bgcios
{

namespace sysio
{

//! \brief Manage a job running on a compute node.

class Job
{
public:

   //! \brief  Default constructor.

   Job(uint64_t jobId)
   {
      _jobId = jobId;
      _posixMode = true;
      _logJobStatistics = false;
      _shortCircuitPath.clear();
      _shortCircuitFd = -1;
      _killed = false;
      _WaitingForJobCleanup = true;
     _TimedOutForKernelInternalWrite = false;
   }

   //! \brief  Get the job identifier.
   //! \return Job id value.
inline
   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Return indicator if posix mode is enabled for job.
   //! \return True if posix mode is enabled, otherwise false.
inline
   bool posixMode(void) const { return _posixMode; }

   //! \brief  Set the posix mode attribute.
   //! \param  value New value for posix mode attribute.
   //! \return Nothing.
inline
   void setPosixMode(bool value) { _posixMode = value; }

   //! \brief  Return indicator if log job statistics is enabled for job.
   //! \return True if log job statistics is enabled, otherwise false.
inline
   bool logJobStatistics(void) const { return _logJobStatistics; }

   //! \brief  Set the log job statistics attribute.
   //! \param  value New value for posix mode attribute.
   //! \return Nothing.
inline
   void setLogJobStatistics(bool value) { _logJobStatistics = value; }

   //! \brief  Get the short circuit descriptor (-1 means short circuit is not enabled).
   //! \return Short circuit file descriptor.

   int getShortCircuitFd(void) { return _shortCircuitFd; }

   //! \brief  Set the short circuit descriptor if the specified path name matches the short circuit path.
   //! \param  pathname Path to file.
   //! \param  fd File descriptor to specified file.
   //! \return Nothing.

   void setShortCircuitFd(std::string pathname, int fd)
   {
      if (_shortCircuitPath == pathname) {
         _shortCircuitFd = fd;
      }
      return;
   }

   //! \brief  Set the short circuit path.
   //! \param  pathname Path to short circuit file.
   //! \return Nothing.
inline
   void setShortCircuitPath(std::string pathname) { _shortCircuitPath = pathname; }

   //! \brief  Mark job as killed so subsequent messages do not start an I/O operation.
   //! \return Nothing.
inline
   void markKilled(void) { _killed = true; }

   //! \brief  Check if job is marked as killed.
   //! \return True if job is marked as killed, otherwise false.
inline
   bool isKilled(void) { return _killed; }

   //! Stopwatch for timing pread64 system calls.
   bgcios::StopWatch preadTimer;

   //! Stopwatch for timing pwrite64 system calls.
   bgcios::StopWatch pwriteTimer;

   //! Stopwatch for timing read system calls.
   bgcios::StopWatch readTimer;

   //! Stopwatch for timing recv system calls.
   bgcios::StopWatch recvTimer;

   //! Stopwatch for timing recvfrom system calls.
   bgcios::StopWatch recvfromTimer;

   //! Stopwatch for timing send system calls.
   bgcios::StopWatch sendTimer;

   //! Stopwatch for timing sendto system calls.
   bgcios::StopWatch sendtoTimer;

   //! Stopwatch for timing write system calls.
   bgcios::StopWatch writeTimer;

   //! Stopwatch for timing open system calls.
   bgcios::StopWatch openTimer;

   //! Get value of _WaitingForJobCleanup
inline
   bool getWaitingForJobCleanup(){return _WaitingForJobCleanup;}

   //! Set value of _WaitingForJobCleanup
inline 
   void setWaitingForJobCleanup(bool b){ _WaitingForJobCleanup=b;}

 size_t sizeShortCircuitPath(){return _shortCircuitPath.size();}

 void addRankEINTR(uint32_t rank){ _rankEINTRList.push_back(rank);}

 bool removedRankEINTR(uint32_t rank){
   const std::list<uint32_t>::iterator r = std::find(_rankEINTRList.begin(),_rankEINTRList.end(),rank);
   if ( r != _rankEINTRList.end() ) {
     _rankEINTRList.erase(r);
     return true;
   }
   return false;
 }

 bool foundRankEINTR(uint32_t rank){
   const std::list<uint32_t>::iterator r = std::find(_rankEINTRList.begin(),_rankEINTRList.end(),rank);
   return ( r != _rankEINTRList.end() );
 }

 void timedOutForKernelWrite(){_TimedOutForKernelInternalWrite=true;}
 bool getTimedOutForKernelWrite(){return _TimedOutForKernelInternalWrite;}
private:

   //! Job identifier.
   uint64_t _jobId;

   //! Attribute to control posix mode for read and write operations.
   bool _posixMode;

   //! Attribute to control logging statistics when job ends.
   bool _logJobStatistics;

   //! Path to short circuit file.
   std::string _shortCircuitPath;

   //! Descriptor of short circuit file for skipping file operations.
   int _shortCircuitFd;

   //! True when job has been sent a SIGKILL signal.
   volatile int _killed;

   //! True when job has been cleaned up
   volatile int _WaitingForJobCleanup;

   //! True when job has "timed" out
   volatile int _TimedOutForKernelInternalWrite;

   //! List of rank
   std::list <uint32_t>  _rankEINTRList;

};

//! Smart pointer for ClientMessage object.
typedef std::tr1::shared_ptr<Job> JobPtr;

} // namespace sysio

} // namespace bgcios

#endif // SYSIO_JOB_H

