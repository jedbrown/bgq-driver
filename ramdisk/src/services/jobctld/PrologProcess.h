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

//! \file  PrologProcess.h
//! \brief Declaration and inline methods for bgcios::jobctl::PrologProcess class.

#ifndef JOBCTL_PROLOGPROCESS_H
#define JOBCTL_PROLOGPROCESS_H

// Includes
#include <ramdisk/include/services/common/Process.h>
#include <ramdisk/include/services/common/MessageResult.h>
#include <string>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief Process to run a prolog program for a job.

class PrologProcess : public Process
{
public:

   //! \brief  Default constructor.
   //! \param  program Path to executable program.
   //! \param  jobId Job identifier.

   PrologProcess(std::string program, uint64_t jobId) : Process()
   {
      _program = program;
      _jobId = jobId;
   }

   //! \brief  Default destructor.

   ~PrologProcess();

   //! \brief  Create a new process and start the job prolog program.
   //! \return Result of operation.

   bgcios::MessageResult start(void);

   //! \brief  Get the job identifier.
   //! \return Job id value.

   uint64_t getJobId(void) const { return _jobId; }

private:

   //! Job identifier.
   uint64_t _jobId;

};

//! Smart pointer for ToolProcess class.
typedef std::tr1::shared_ptr<PrologProcess> PrologProcessPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_PROLOGPROCESS_H

