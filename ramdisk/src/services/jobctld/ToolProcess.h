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

//! \file  ToolProcess.h
//! \brief Declaration and inline methods for bgcios::jobctl::ToolProcess class.

#ifndef JOBCTL_TOOLPROCESS_H
#define JOBCTL_TOOLPROCESS_H

// Includes
#include <ramdisk/include/services/common/Process.h>
#include <ramdisk/include/services/common/UserIdentity.h>
#include <ramdisk/include/services/common/MessageResult.h>
#include <ramdisk/include/services/MessageHeader.h>
#include <tr1/memory>

namespace bgcios
{

namespace jobctl
{

//! \brief Process to run a tool for a job.

class ToolProcess : public Process
{
public:

   //! \brief  Default constructor.
   //! \param  program Path to executable program.
   //! \param  toolId Tool identifier.
   //! \param  jobId Job identifier.

   ToolProcess(std::string program, uint32_t toolId, uint64_t jobId) : Process()
   {
      _program = program;
      _toolId = toolId;
      _jobId = jobId;
      _removalPending = false;
   }

   //! \brief  Default destructor.

   ~ToolProcess();

   //! \brief  Create a new process and start the program with the specified user identity.
   //! \param  identity User identity.
   //! \param  simulation flag to indicate if we are in simulation mode
   //! \return Result of operation.

   bgcios::MessageResult start(UserIdentity& identity, bool simulation);

   //! \brief  Get the tool identifier.
   //! \return Tool id value.

   uint32_t getToolId(void) const { return _toolId; }

   //! \brief  Get the job identifier.
   //! \return Job id value.

   uint64_t getJobId(void) const { return _jobId; }

   //! \brief  Check if removal is pending for the tool.
   //! \note   When removal is pending, status has been obtained for the child process.
   //! \return True if removal is pending, otherwise false.

   bool isRemovalPending(void) const { return _removalPending; }

   //! \brief  Set removal pending flag for the tool.
   //! \param  value New value for removal pending flag.
   //! \return Nothing.

   void setRemovalPending(bool value) { _removalPending = value; }
   
   int signal(int signo)
   {
      int rc = killpg(_processId, signo);
      if (rc != 0) rc = errno;
      return rc;
   }

private:

   //! Tool identifier.
   uint32_t _toolId;

   //! Job identifier.
   uint64_t _jobId;

   //! True when removal is pending for the tool.
   bool _removalPending;

};

//! Smart pointer for ToolProcess class.
typedef std::tr1::shared_ptr<ToolProcess> ToolProcessPtr;

} // namespace jobctl

} // namespace bgcios

#endif // JOBCTL_TOOLPROCESS_H

