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

//! \file  Process.cc
//! \brief Methods for bgcios::Process class.

// Includes
#include <ramdisk/include/services/common/Process.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace bgcios;

LOG_DECLARE_FILE("cios.common");

void
Process::prepareChild(char *args[], char *envs[])
{
   // Ask kernel to SIGTERM child if parent dies.
   prctl(PR_SET_PDEATHSIG, SIGTERM);

   // Put the child in its own process group.
   if (setpgrp() != 0) {
      int err = errno;
      LOG_ERROR_MSG("failed to put child process " << getpid() << " in a new process group: " << bgcios::errorString(err));
   }

   // Close descriptors inherited from parent process.
   struct rlimit rl;
   getrlimit(RLIMIT_NOFILE, &rl);
   for (rlim_t index = STDERR_FILENO + 1; index < rl.rlim_cur; ++index) {
      if (close((int)index)) {
         if (errno != EBADF) {
            LOG_ERROR_MSG("failed to close descriptor " << index << " in child process");
         }
      }
   }

   // Build the list of arguments.
   int counter = 0;
   args[counter] = new char [_program.length() + 2];
   strcpy(args[counter], _program.c_str());
   ++counter;

   for (std::vector<std::string>::iterator iter = _arguments.begin(); iter != _arguments.end(); ++iter) {
      args[counter] = new char [iter->length() + 2];
      strcpy(args[counter], iter->c_str());
      ++counter;
   }
   args[counter] = NULL; // Null terminate

   // Build the list of environment variables.
   counter = 0;
   for (std::vector<std::string>::iterator iter = _environVariables.begin(); iter != _environVariables.end(); ++iter) {
      envs[counter] = new char [iter->length() + 2];
      strcpy(envs[counter], iter->c_str());
      ++counter;
   }
   envs[counter] = NULL; // Null terminate

   return;
}

int
Process::runChild(void)
{
   // Setup array of pointers for argument strings.
   size_t numArguments = _arguments.size() + 2;
   char *args[numArguments];

   // Setup array of pointers for environment variable strings.
   size_t numEnvironVariables = _environVariables.size() + 1;
   char *envs[numEnvironVariables];

   // Prepare child process for new program.
   prepareChild(args, envs);

   // Transfer control to the program.
   execve(args[0], args, envs);

   // We only get here if execve() failed.
   int err = errno;
   return err;
}

int
Process::waitFor(time_t timeoutSeconds)
{
   // Just return if child process has already ended.
   if (isWaitedFor()) {
      return 0;
   }

   time_t endTime = time(NULL) + timeoutSeconds;
   bool done = false;

   while (!done) {

      // Wait for a SIGCHLD signal to be delivered.  Note that it is caller's responsibility to block SIGCHLD from all threads.
      sigset_t waitSet;
      sigemptyset(&waitSet);
      sigaddset(&waitSet, SIGCHLD);
      struct timespec timeout;
      timeout.tv_sec = timeoutSeconds;
      timeout.tv_nsec = 0;
      int rc = sigtimedwait(&waitSet, NULL, &timeout);
      if (rc < 0) {
         return errno;
      }

      // Wait for the child process (status should be available since we already received the SIGCHLD signal).
      pid_t child = waitpid(_processId, &_status, WNOHANG);
      if (child == -1) {
         return errno;
      }

      // The child process has ended. 
      if (child == _processId) {
         return 0;
      }

      // Another child process ended which caused a SIGCHLD signal to be delivered.  Reset the timeout and wait again.
      timeoutSeconds = endTime - time(NULL);
   }

   return 0;
}

