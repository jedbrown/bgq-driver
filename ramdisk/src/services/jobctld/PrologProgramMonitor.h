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

//! \file  PrologProgramMonitor.h
//! \brief Declaration and inline methods for bgcios::jobctl::PrologProgramMonitor class.

#ifndef JOBCTL_PROLOGPROGRAMMONITOR_H
#define JOBCTL_PROLOGPROGRAMMONITOR_H

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

//! \brief  Monitor a job prolog program.

class PrologProgramMonitor : public Thread
{
public:

   //! \brief  Default constructor.
   //! \param  prologProgramPath Path to prolog program.
   //! \param  timeout Number of seconds to wait for prolog program to complete.
   //! \param  jobId Job identifier.
   //! \param  ackMsg Pointer to accumulated LoadJobAck message.

   PrologProgramMonitor(std::string prologProgramPath, int32_t timeout, uint64_t jobId, bgcios::MessageHeader *ackMsg) : Thread()
   {
      _prologProgramPath = prologProgramPath;
      _timeout = timeout;
      _jobId = jobId;
      memcpy(&_ackMsg, ackMsg, sizeof(_ackMsg));
   }

   //! \brief  Default destructor.

   ~PrologProgramMonitor();

   //! \brief  Run the monitor function.
   //! \return Pointer to return code value (can be NULL).

   void * run(void);

private:

   //! Path to prolog program.
   std::string _prologProgramPath;

   //! Number of seconds to wait for prolog program to complete.
   int32_t _timeout;

   //! Job identifier.
   uint64_t _jobId;

   //! Pointer to accumulated LoadJobAck message.
   LoadJobAckMessage _ackMsg;

   //! Command channel for communicating with jobctld.
   bgcios::LocalDatagramSocketPtr _cmdChannel;

};

//! Smart pointer for PrologProgramMonitor class.
typedef std::tr1::shared_ptr<PrologProgramMonitor> PrologProgramMonitorPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_PROLOGPROGRAMMONITOR_H

