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

//! \file  PrologProgramMonitor.cc
//! \brief Methods for bgcios::jobctl::PrologProgramMonitor class.

// Includes
#include "PrologProgramMonitor.h"
#include "PrologProcess.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/common/logging.h>
#include <sstream>

#include <boost/lexical_cast.hpp>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");

PrologProgramMonitor::~PrologProgramMonitor()
{
   LOG_CIOS_TRACE_MSG("Job " << _jobId << ": destroying PrologProgramMonitor object");
   pthread_attr_destroy(&_attributes);
}

void *
PrologProgramMonitor::run(void)
{
   // Create a socket for communicating with jobctld.
   try {
      _cmdChannel = LocalDatagramSocketPtr(new LocalDatagramSocket());
   }
   catch (bgcios::SocketError& e) {
      LOG_ERROR_MSG("error creating command channel for prolog program monitor: " << bgcios::errorString(e.errcode()));
      return NULL;
   }

   // Create path to jobctld command channel.
   std::ostringstream jobctlPath;
   jobctlPath << bgcios::WorkDirectory << bgcios::JobctlCommandChannelName;

   // Start the job prolog program in a new process.
   PrologProcessPtr prologProcess = PrologProcessPtr(new PrologProcess(_prologProgramPath, _jobId));
   std::ostringstream jobIdArg;
   jobIdArg << _jobId;
   prologProcess->addArgument(jobIdArg.str());

   // Add BG_JOBID variable
   prologProcess->addEnvironVariable( "BG_JOBID=" + boost::lexical_cast<std::string>(_jobId) );

   bgcios::MessageResult result = prologProcess->start();
   if (result.isError()) {
      result.setHeader(_ackMsg.header);
      _cmdChannel->sendTo(jobctlPath.str(), &_ackMsg, _ackMsg.header.length);
      return NULL;
   }

   // Wait for the job prolog program to end.
   LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": waiting for " << _timeout << " seconds for '" << _prologProgramPath << "' to end ...");
   int err = 0;
   if (_timeout == -1) {
      err = prologProcess->waitFor();
   }
   else {
      err = prologProcess->waitFor((time_t)_timeout);
   }

   // Check the result from waiting for the job prolog program to end.
   if (err != 0) {
      if (err == EAGAIN) {
         LOG_CIOS_WARN_MSG("Job " << _jobId << ": prolog program '" << _prologProgramPath << "' exceeded timeout of " << _timeout << " seconds");
         _ackMsg.header.returnCode = bgcios::PrologPgmError;
         _ackMsg.header.errorCode = ETIME;

         // Forcibly end the job prolog program and wait for it to end.
         prologProcess->signal(SIGKILL);
         prologProcess->waitFor();
      }

      else {
         LOG_ERROR_MSG("Job " << _jobId << ": error waiting for prolog program '" << _prologProgramPath << "' to end: " << bgcios::errorString(err));
      }
   }

   // Set the return code based on the result of the prolog program.
   if (_ackMsg.header.returnCode == bgcios::Success) {
      if (prologProcess->isSignaled()) {
         LOG_CIOS_INFO_MSG("Job " << _jobId << ": prolog program '" << _prologProgramPath << "' ended with signal " << prologProcess->getTermSignal());
         _ackMsg.header.returnCode = bgcios::PrologPgmError;
         _ackMsg.header.errorCode = ECANCELED;
      }
      else if ((prologProcess->isExited()) && (prologProcess->getExitStatus() > 0)) {
         LOG_CIOS_INFO_MSG("Job " << _jobId << ": prolog program '" << _prologProgramPath << "' ended with exit status " << prologProcess->getExitStatus());
         _ackMsg.header.returnCode = bgcios::PrologPgmError;
         _ackMsg.header.errorCode = EINVAL;
      }
      else {
         LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": prolog program '" << _prologProgramPath << "' ended successfully");
      }
   }

   // Send LoadJobAck message to jobctld.
   LOG_CIOS_DEBUG_MSG("Job " << _ackMsg.header.jobId << ": LoadJobAck message sent on command channel");
   _cmdChannel->sendTo(jobctlPath.str(), &_ackMsg, _ackMsg.header.length);

   return NULL;
}
