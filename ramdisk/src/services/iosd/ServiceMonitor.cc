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

//! \file  ServiceMonitor.cc
//! \brief Methods for bgcios::iosctl::ServiceMonitor class.

// Includes
#include "ServiceMonitor.h"
#include <ramdisk/include/services/common/Process.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>

using namespace bgcios::iosctl;

LOG_DECLARE_FILE("cios.iosd");

ServiceMonitor::~ServiceMonitor()
{
   pthread_attr_destroy(&_attributes);
}

void *
ServiceMonitor::run(void)
{
   LOG_CIOS_DEBUG_MSG("monitoring " << _manager->listSize() << " services that are running on node");

   // Loop until told to stop.
   while (!_done) {
      int status;
      pid_t child;
      int err = bgcios::Process::waitForAny(&child, &status);
      if (err != 0) {
         if (err == EINTR) {
            LOG_CIOS_DEBUG_MSG("wait for any process was interrupted");
            _done = true;
            continue;
         }
         else if (err == ECHILD) {
            _done = true;
            continue;
         }
         else {
            LOG_ERROR_MSG("error waiting for any process to end: " << bgcios::errorString(err));
            return NULL;
         }
      }

      // Restart the service if it ended by signal that was not SIGTERM or a signal handler ended the process.
      if (WIFEXITED(status) && WEXITSTATUS(status) == 99) {
         LOG_CIOS_WARN_MSG("service running in process " << child << " caught a signal and ended, attempting to restart ...");
         _manager->restart(child);
      }
      else if (WIFSIGNALED(status) && WTERMSIG(status) != SIGTERM) {
         LOG_CIOS_WARN_MSG("service running in process " << child << " ended by signal " << WTERMSIG(status) << ", attempting to restart ...");
         bgcios::RasEvent rasEvent(bgcios::DaemonFailed);
         rasEvent << child;
         rasEvent << WTERMSIG(status);
         rasEvent.send();

         _manager->restart(child);
      }
      else {
         LOG_CIOS_INFO_MSG("service running in process " << child << " ended normally with status " << WEXITSTATUS(status));
         _manager->remove(child);
      }
   }

   LOG_CIOS_INFO_MSG("stopped monitoring services");
   return NULL;
}


