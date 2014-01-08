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

//! \file  EpilogProgramMonitor.cc
//! \brief Methods for bgcios::jobctl::EpilogProgramMonitor class.

// Includes
#include "EpilogProgramMonitor.h"
#include "EpilogProcess.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/common/logging.h>
#include <sstream>

#include <boost/lexical_cast.hpp>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");

EpilogProgramMonitor::~EpilogProgramMonitor()
{
   LOG_CIOS_TRACE_MSG("Job " << _jobId << ": destroying EpilogProgramMonitor object");
   pthread_attr_destroy(&_attributes);
}

void *
EpilogProgramMonitor::run(void)
{
   // Create a socket for communicating with jobctld.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket());
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating command channel for epilog program monitor: " << bgcios::errorString(e.errcode()));
      return NULL;
   }

   // Create path to jobctld command channel.
   std::ostringstream jobctlPath;
   jobctlPath << bgcios::WorkDirectory << bgcios::JobctlCommandChannelName;

   // Start the job epilog program in a new process.
   EpilogProcessPtr epilogProcess = EpilogProcessPtr(new EpilogProcess(_epilogProgramPath, _jobId));
   std::ostringstream jobIdArg;
   jobIdArg << _jobId;
   epilogProcess->addArgument(jobIdArg.str());

   // Add BG_JOBID variable
   epilogProcess->addEnvironVariable( "BG_JOBID=" + boost::lexical_cast<std::string>(_jobId) );

   bgcios::MessageResult result = epilogProcess->start();
   if (result.isError()) {
      _ackMsg.epilogReturnCode = result.returnCode();
      _ackMsg.epilogErrorCode = (uint32_t)result.errorCode();
      _cmdChannel->sendTo(jobctlPath.str(), &_ackMsg, _ackMsg.header.length);
      return NULL;
   }

   // Wait for the job epilog program to end.
   LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": waiting for " << _timeout << " seconds for '" << _epilogProgramPath << "' to end ...");
   int err = 0;
   if (_timeout == -1) {
      err = epilogProcess->waitFor();
   }
   else {
      err = epilogProcess->waitFor((time_t)_timeout);
   }

   // Check the result from waiting for the job epilog program to end.
   if (err != 0) {
      if (err == EAGAIN) {
         LOG_CIOS_WARN_MSG("Job " << _jobId << ": epilog program '" << _epilogProgramPath << "' exceeded timeout of " << _timeout << " seconds");
         _ackMsg.header.returnCode = bgcios::EpilogPgmError;
         _ackMsg.header.errorCode = ETIME;

         // Forcibly end the job epilog program and wait for it to end.
         epilogProcess->signal(SIGKILL);
         epilogProcess->waitFor();
      }

      else {
         LOG_ERROR_MSG("Job " << _jobId << ": error waiting for epilog program '" << _epilogProgramPath << "' to end: " << bgcios::errorString(err));
      }
   }


   // Set the return code based on the result of the epilog program.
   if (_ackMsg.header.returnCode == bgcios::Success) {
      if (epilogProcess->isSignaled()) {
         LOG_CIOS_INFO_MSG("Job " << _jobId << ": epilog program '" << _epilogProgramPath << "' ended with signal " << epilogProcess->getTermSignal());
         _ackMsg.epilogReturnCode = bgcios::EpilogPgmError;
         _ackMsg.epilogErrorCode = ECANCELED;
      }
      else if ((epilogProcess->isExited()) && (epilogProcess->getExitStatus() > 0)) {
         LOG_CIOS_INFO_MSG("Job " << _jobId << ": epilog program '" << _epilogProgramPath << "' ended with exit status " << epilogProcess->getExitStatus());
         _ackMsg.epilogReturnCode = bgcios::EpilogPgmError;
         _ackMsg.epilogErrorCode = EINVAL;
      }
      else {
         LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": epilog program '" << _epilogProgramPath << "' ended successfully");
      }
   }

   // Send CleanupJobAck message to jobctld.
   LOG_CIOS_DEBUG_MSG("Job " << _ackMsg.header.jobId << ": CleanupJobAck message sent on command channel");
   _cmdChannel->sendTo(jobctlPath.str(), &_ackMsg, _ackMsg.header.length);

   return NULL;
}
