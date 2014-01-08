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

//! \file  ToolProcess.cc
//! \brief Methods for bgcios::jobctl::ToolProcess class.

// Includes
#include "ToolProcess.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


ToolProcess::~ToolProcess()
{
   LOG_CIOS_TRACE_MSG("Job " << _jobId << ": destroying ToolProcess object for '" << _program << "' (" << _toolId << ")");
   _arguments.clear();
   _environVariables.clear();
   _processId = 0;
}

bgcios::MessageResult
ToolProcess::start(UserIdentity& identity, bool simulation)
{
   bgcios::MessageResult result;

   // Make sure there is a program to run.
   if (_program.empty()) {
      LOG_ERROR_MSG("Job " << _jobId << ": failed to start tool: path to program was not specified");
      result.set(bgcios::ToolStartError, ENOENT);
      return result;
   }

   // Make sure the tool is not going to get root privileges.
   struct stat statbuf;
   if (::stat(_program.c_str(), &statbuf) == 0) {
      if ((statbuf.st_uid == 0) || (statbuf.st_gid == 0)) {
         if ((statbuf.st_mode & S_ISUID) || (statbuf.st_mode & S_ISGID)) {
            LOG_ERROR_MSG("Job " << _jobId << ": failed to start tool '" << _program << "': owner or group is not valid");
            result.set(bgcios::ToolAuthorityError, EPERM);
            return result;
         }
      }
   }
   else {
      int err = errno;
      LOG_ERROR_MSG("Job " << _jobId << ": failed to check status for tool '" << _program << "': " << bgcios::errorString(err));
      result.set(bgcios::ToolStartError, err);
      return result;
   }

   // Fork a new process for running the program.
   const pid_t child = fork();

   // There was an error from fork().
   if (child == -1) {
      int err = errno;
      LOG_ERROR_MSG("Job " << _jobId << ": failed to fork new process for tool '" << _program << "': " << bgcios::errorString(err));
      result.set(bgcios::ToolStartError, err);
      return result;
   }

   // Start the program in the child process.
   else if (child == 0) {

      // change process group
      setpgrp();

      // Setup array of pointers for argument strings.
      size_t numArguments = _arguments.size() + 2;
      char *args[numArguments];

      // Setup array of pointers for environment variable strings.
      size_t numEnvironVariables = _environVariables.size() + 1;
      char *envs[numEnvironVariables];

      // Prepare child process for new program.
      prepareChild(args, envs);

      // Swap to the user's identity.
      if ( !simulation ) {
          bgcios::MessageResult result = identity.swap();
          if (result.returnCode() != bgcios::Success) {
              LOG_ERROR_MSG("Job " << _jobId << ": failed to swap to user identity for user " << identity.getUserName() <<
                      ": " << bgcios::errorString(result.errorCode()));
              exit(1);
          }
      }

      // Exec the tool program.
      execve(_program.c_str(), args, envs);

      // If we get here, there was an error.
      int err = errno;
      LOG_ERROR_MSG("Job " << _jobId << ": failed to exec program '" << _program << "': " << bgcios::errorString(err));
      exit(126);
   }

   // Update info in the parent process.
   else {
      _processId = child;
      LOG_CIOS_TRACE_MSG("Job " << _jobId << ": started tool '" << _program << "' (" << _toolId << ") in process " << _processId);
   }

   return result;
}

