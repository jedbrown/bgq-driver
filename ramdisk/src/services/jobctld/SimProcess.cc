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

//! \file  SimProcess.cc
//! \brief Methods for bgcios::jobctl::SimProcess class.

// Includes
#include "SimProcess.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


SimProcess::~SimProcess()
{
   LOG_CIOS_TRACE_MSG("Job " << _jobId << ": destroying SimProcess object for process " << _processId);
   _arguments.clear();
   _environVariables.clear();
   close(_stdoutFd);
   close(_stderrFd);
   _processId = 0;
}

bgcios::MessageResult
SimProcess::start(void)
{
   bgcios::MessageResult result;

   // Make sure there is a program to run.
   if (_program.empty()) {
      LOG_ERROR_MSG("Job " << _jobId << ": failed to start program: path to program was not specified");
      result.set(bgcios::AppOpenError, ENOENT);
      return result;
   }

   // Set the initial working directory.
   if (chdir(_workingDirectory.c_str()) != 0) {
      result.set(bgcios::WorkingDirError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to change to directory '" << _workingDirectory << "': " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Make sure the user has authority to the program.
   if (access(_program.c_str(), R_OK|X_OK) != 0) {
      result.set(bgcios::AppAuthorityError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed authority check for program '" << _program << "': " << bgcios::errorString(result.errorCode()));
      chdir("/");
      return result;
   }

   // Create a pipe for capturing standard output.
   int stdoutSet[2];
   if (pipe(stdoutSet) != 0) {
      result.set(bgcios::AppOpenError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to create stdout pipe: " << bgcios::errorString(result.errorCode()));
      chdir("/");
      return result;
   }

   // Create a pipe for capturing standard error.
   int stderrSet[2];
   if (pipe(stderrSet) != 0) {
      result.set(bgcios::AppOpenError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to create stdout pipe: " << bgcios::errorString(result.errorCode()));
      chdir("/");
      close(stdoutSet[0]);
      close(stdoutSet[1]);
      return result;
   }

   // Fork a new process for running the program.
   pid_t child = fork();

   // There was an error from fork().
   if (child == -1) {
      result.set(bgcios::AppOpenError, errno);
      LOG_ERROR_MSG("Job " << _jobId << ": failed to fork new process: " << bgcios::errorString(result.errorCode()));
      chdir("/");
      close(stdoutSet[0]);
      close(stdoutSet[1]);
      close(stderrSet[0]);
      close(stderrSet[1]);
      return result;
   }

   // Start the program in the child process.
   else if (child == 0) {
      // kill child if we go away
      prctl( PR_SET_PDEATHSIG, SIGKILL );

      // Build the list of arguments.
      size_t numArguments = _arguments.size() + 2;
      char *args[numArguments];
      int counter = 0;

      for (std::vector<std::string>::iterator iter = _arguments.begin(); iter != _arguments.end(); ++iter) {
         args[counter] = new char [iter->length() + 2];
         strcpy(args[counter], iter->c_str());
         LOG_CIOS_TRACE_MSG("argv[" << counter << "] = '" << args[counter] << "'");
         ++counter;
      }
      args[counter] = NULL; // Null terminate

      // Build the list of environment variables.
      size_t numEnvironVariables = _environVariables.size() + 1;
      char *envs[numEnvironVariables];
      counter = 0;

      for (std::vector<std::string>::iterator iter = _environVariables.begin(); iter != _environVariables.end(); ++iter) {
         envs[counter] = new char [iter->length() + 2];
         strcpy(envs[counter], iter->c_str());
         LOG_CIOS_TRACE_MSG("environ[" << counter << "] = '" << envs[counter] << "'");
         ++counter;
      }
      envs[counter] = NULL; // Null terminate

      // Move pipe descriptors to required descriptor numbers.
      close(stdoutSet[0]);
      dup2(stdoutSet[1], STDOUT_FILENO);
      close(stderrSet[0]);
      dup2(stderrSet[1], STDERR_FILENO);

      // Close descriptors inherited from parent process.
      struct rlimit rl;
      getrlimit(RLIMIT_NOFILE, &rl);
      for (rlim_t index = STDERR_FILENO + 1; index < rl.rlim_cur; ++index) {
         close((int)index);
      }

      // Set the umask.
      umask(_umask);

      // Exec the io proxy program.
      execve(_program.c_str(), args, envs);

      // If we get here, there was an error.
      int err = errno;
      LOG_ERROR_MSG("Job " << _jobId << ": failed to exec program '" << _program << "': " << bgcios::errorString(err));
      exit(126);
   }

   // Update info in the parent process.
   else {
      _processId = child;
      close(stdoutSet[1]);
      _stdoutFd = stdoutSet[0];
      close(stderrSet[1]);
      _stderrFd = stderrSet[0];
      chdir("/");
      LOG_CIOS_TRACE_MSG("Job " << _jobId << ": started '" << _program << "' in process " << _processId);
   }

   return result;
}
