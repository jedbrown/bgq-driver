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

//! \file  ServiceProcess.cc
//! \brief Methods for bgcios::iosctl::ServiceProcess class.

// Includes
#include "ServiceProcess.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/common/logging.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");


ServiceProcess::~ServiceProcess()
{
   _arguments.clear();
   _environVariables.clear();
   _processId = 0;
}

bgcios::MessageResult
ServiceProcess::start(void)
{
   bgcios::MessageResult result;

   // Make sure there is a program to run.
   if (_program.empty()) {
      result.set(bgcios::RequestFailed, ENOENT);
      LOG_ERROR_MSG("failed to start service: path was not specified");
      return result;
   }

   // Fork a new process for running the program.
   pid_t child = fork();

   // There was an error from fork().
   if (child == -1) {
      result.set(bgcios::RequestFailed, errno);
      LOG_ERROR_MSG("failed to fork new service process: " << bgcios::errorString(result.errorCode()));
      return result;
   }

   // Start the program in the child process.
   else if (child == 0) {
      int err = runChild();

      // If we get here, there was an error.
      LOG_FATAL_MSG("failed to exec program " << _program << ": " << bgcios::errorString(err));
      bgcios::RasEvent rasEvent(bgcios::DaemonStartFailed);
      rasEvent << getpid();
      rasEvent << err;
      rasEvent.send();
      exit(EXIT_FAILURE);
   }

   // Update info in the parent process.
   else {
      _processId = child;
      ++_numStarts;
      LOG_CIOS_TRACE_MSG("started " << _program << " in process " << _processId << " (" << _numStarts << " times)");
   }

   return result;
}

