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

//! \file  PrologProcess.cc
//! \brief Methods for bgcios::jobctl::PrologProcess class.

// Includes
#include "PrologProcess.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <unistd.h>
#include <stdlib.h>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


PrologProcess::~PrologProcess()
{
   LOG_CIOS_TRACE_MSG("Job " << _jobId << ": destroying PrologProcess object for '" << _program << "' in process " << _processId);
   _arguments.clear();
   _environVariables.clear();
   _processId = 0;
}

bgcios::MessageResult
PrologProcess::start(void)
{
   bgcios::MessageResult result;

   // Make sure there is a program to run.
   if (_program.empty()) {
      LOG_ERROR_MSG("Job " << _jobId << ": failed to start prolog program: path to program was not specified");
      result.set(bgcios::PrologPgmStartError, ENOENT);
      return result;
   }

   // Make sure we have execute authority to the program.
   if (::access(_program.c_str(), X_OK) != 0) {
      result.set(bgcios::PrologPgmStartError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to check authority to prolog program '" << _program << "': " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Fork a new process for running the program.
   pid_t child = fork();

   // There was an error from fork().
   if (child == -1) {
      result.set(bgcios::PrologPgmStartError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to fork new process for prolog program '" << _program << "': " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Start the program in the child process.
   else if (child == 0) {
      int err = runChild();

      // If we get here, there was an error.
      LOG_ERROR_MSG("Job " << _jobId << ": failed to exec prolog program '" << _program << "': " << bgcios::errorString(err));
      ::exit(126);
   }

   // Update info in the parent process.
   else {
      _processId = child;
      LOG_CIOS_DEBUG_MSG("Job " << _jobId << ": started prolog program '" << _program << "' in process " << _processId);
   }

   return result;
}


