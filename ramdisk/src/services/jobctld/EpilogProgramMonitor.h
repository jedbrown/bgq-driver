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

//! \file  EpilogProgramMonitor.h
//! \brief Declaration and inline methods for bgcios::jobctl::EpilogProgramMonitor class.

#ifndef JOBCTL_EPILOGPROGRAMMONITOR_H
#define JOBCTL_EPILOGPROGRAMMONITOR_H

// Includes
#include <ramdisk/include/services/common/Thread.h>
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <string>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief  Monitor a job epilog program.

class EpilogProgramMonitor : public Thread
{
public:

   //! \brief  Default constructor.
   //! \param  epilogProgramPath Path to epilog program.
   //! \param  timeout Number of seconds to wait for epilog program to complete.
   //! \param  jobId Job identifier.
   //! \param  ackMsg Pointer to accumulated CleanupJobAck message.

   EpilogProgramMonitor(std::string epilogProgramPath, int32_t timeout, uint64_t jobId, CleanupJobAckMessage *ackMsg) : Thread()
   {
      _epilogProgramPath = epilogProgramPath;
      _timeout = timeout;
      _jobId = jobId;
      memcpy(&_ackMsg, ackMsg, sizeof(_ackMsg));
   }

   //! \brief  Default destructor.

   ~EpilogProgramMonitor();

   //! \brief  Run the monitor function.
   //! \return Pointer to return code value (can be NULL).

   void * run(void);

private:

   //! Path to epilog program.
   std::string _epilogProgramPath;

   //! Number of seconds to wait for epilog program to complete.
   int32_t _timeout;

   //! Job identifier.
   uint64_t _jobId;

   //! Pointer to accumulated CleanupJobAck message.
   CleanupJobAckMessage _ackMsg;

   //! Command channel for communicating with jobctld.
   bgcios::LocalDatagramSocketPtr _cmdChannel;

};

//! Smart pointer for EpilogProgamMonitor class.
typedef std::tr1::shared_ptr<EpilogProgramMonitor> EpilogProgramMonitorPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_EPILOGPROGRAMMONITOR_H

