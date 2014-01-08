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

//! \file  stdiod/Job.h
//! \brief Declaration and inline methods for bgcios::stdio::Job class.

#ifndef STDIO_JOB_H
#define STDIO_JOB_H

// Includes
#include <ramdisk/include/services/common/StopWatch.h>
#include "StdioAccumulators.h"
#include <ramdisk/include/services/common/RdmaClient.h>
#include <tr1/memory>

namespace bgcios
{

namespace stdio
{

//! \brief Track a job submitted to a compute block.

class Job
{
public:

   //! \brief  Default constructor.
   //! \param  jobId Job identifier.
   //! \param  numRanks Number of ranks participating in job.

   Job(uint64_t jobId, uint32_t numRanks)
   {
      _jobId = jobId;
      _numRanks = numRanks;
      _numSentStdioMsgs = 0;
      _numSentStdioBytes = 0;
      _numDroppedStdioMsgs = 0;
      _numDroppedStdioBytes = 0;
      //_logJobStatistics = false;
      _logJobStatistics = true;
      _killed = false;
   }

   //! \brief  Get the job identifier.
   //! \return Job id value.

   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Get the number of ranks participating in job.
   //! \return Number of ranks.

   uint32_t getNumRanks(void) const { return _numRanks; }

   //! \brief  Set client connection of rank that is reading from standard input.
   //! \param  client Client connection.
   //! \return Nothing.

   void setStdinClient(RdmaClientPtr client) { _stdinClient = client; }

   //! \brief  Get the client connection of rank that is reading from standard input.
   //! \return Client connection.

   RdmaClientPtr& getStdinClient(void) { return _stdinClient; }

   //! \brief  Record data about a sent WriteStdio message.
   //! \param  bytes Number of data bytes in WriteStdio message.
   //! \return Nothing.

inline 
   void recordStdioMessage(uint64_t bytes)
   {
      ++_numSentStdioMsgs;
      _numSentStdioBytes += bytes;
      return;
   }

   //! \brief  Get the number of WriteStdio messages that have been sent.
   //! \return Number of messages.

   uint64_t getNumsentStdioMsgs(void) const { return _numSentStdioMsgs; }

   //! \brief  Get the number of data bytes in WriteStdio messages that have been sent.
   //! \return Number of bytes.

   uint64_t getNumSentStdioBytes(void) const { return _numSentStdioBytes; }

   //! \brief  Record data about a dropped WriteStdio message.
   //! \param  bytes Number of data bytes in WriteStdio message.
   //! \return Nothing.

   void dropStdioMessage(uint64_t bytes)
   {
      ++_numDroppedStdioMsgs;
      _numDroppedStdioBytes += bytes;
      return;
   }

   //! \brief  Get the number of WriteStdio messages that have been dropped.
   //! \return Number of messages.

   uint64_t getNumDroppedStdioMsgs(void) const { return _numDroppedStdioMsgs; }

   //! \brief  Get the number of data bytes in WriteStdio messages that have been dropped.
   //! \return Number of bytes.

   uint64_t getNumDroppedStdioBytes(void) const { return _numDroppedStdioBytes; }

   //! \brief  Mark job as killed so subsequent messages do not start an I/O operation.
   //! \return Nothing.

   void markKilled(void) { _killed = true; }

   //! \brief  Check if job is marked as killed.
   //! \return True if job is marked as killed, otherwise false.

inline
   bool isKilled(void) { return _killed; }

   //! Accumulate CloseStdio messages.
   CloseStdioAccumulator closeStdioAccumulator;

   //! \brief  Return indicator if log job statistics is enabled for job.
   //! \return True if log job statistics is enabled, otherwise false.
inline
   bool logJobStatistics(void) const { return _logJobStatistics; }

   //! \brief  Set the log job statistics attribute.
   //! \param  value New value for posix mode attribute.
   //! \return Nothing.
inline
   void setLogJobStatistics(bool value) { _logJobStatistics = value; }

   //! Stopwatch for timing write stdio calls.
   bgcios::StopWatch writeTimer;

private:

   //! Job identifier.
   uint64_t _jobId;

   //! Number of ranks participating in job.
   uint32_t _numRanks;

   //! Client connection for rank reading from standard input.
   RdmaClientPtr _stdinClient;

   //! Number of messages sent on data channel.
   uint64_t _numSentStdioMsgs;

   //! Number of bytes sent on data channel.
   uint64_t _numSentStdioBytes;

   //! Number of messages dropped because data channel connection is down.
   uint64_t _numDroppedStdioMsgs;

   //! Number of bytes dropped because data channel connection is down.
   uint64_t _numDroppedStdioBytes;

   //! True when job has been sent a SIGKILL signal.
   volatile int _killed;
   
   //! Attribute to control logging statistics when job ends.
   bool _logJobStatistics;

};

//! Smart pointer for Job class.
typedef std::tr1::shared_ptr<Job> JobPtr;

} // namespace stdio

} // namespace bgcios

#endif // STDIO_JOB_H

