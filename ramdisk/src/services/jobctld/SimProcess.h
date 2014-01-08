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

//! \file  SimProcess.h
//! \brief Declaration and inline methods for bgcios::jobctl::SimProcess class.

#ifndef JOBCTL_SIMPROCESS_H
#define JOBCTL_SIMPROCESS_H

// Includes
#include <ramdisk/include/services/common/Process.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <string>
#include <sys/types.h>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief Compute node process running in control system simulator.

class SimProcess : public Process
{
public:

   //! \brief  Default constructor.

   SimProcess() : Process()
   {
      _workingDirectory.clear();
      _umask = 0;
      _stdoutFd = -1;
      _stderrFd = -1;
   }

   //! \brief  Constructor.

   SimProcess(std::string program, std::string dir, mode_t mask, uint64_t jobId) : Process()
   {
      _program = program;
      if (dir.empty()) {
         _workingDirectory = ".";
      }
      else {
         _workingDirectory = dir;
      }
      _umask = mask;
      _jobId = jobId;
      _stdoutFd = -1;
      _stderrFd = -1;
   }

   //! \brief  Default destructor.

   ~SimProcess();

   //! \brief  Create a new process and start the program.
   //! \return Result of operation.

   bgcios::MessageResult start(void);

   //! \brief  Get the job identifier.
   //! \return Job id value.

   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Get the standard output pipe descriptor.
   //! \return Descriptor number.

   int getStdoutFd(void) const { return _stdoutFd; }

   //! \brief  Get the standard error pipe descriptor.
   //! \return Descriptor number.

   int getStderrFd(void) const { return _stderrFd; }

private:

   //! Current working directory for the new process.
   std::string _workingDirectory;

   //! Umask for the new process.
   mode_t _umask;

   //! Job identifier for job this process is participating in.
   uint64_t _jobId;

   //! Descriptor for standard output.
   int _stdoutFd;

   //! Descriptor for standard error.
   int _stderrFd;

};

//! Smart pointer for SimProcess class.
typedef std::tr1::shared_ptr<SimProcess> SimProcessPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_SIMPROCESS_H

