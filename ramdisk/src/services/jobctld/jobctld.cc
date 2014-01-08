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

//! \file  jobctld.cc
//! \brief Job control daemon.

// Includes
#include "JobctlConfig.h"
#include "JobController.h"
#include "HwJobController.h"
#include "SimJobController.h"
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/InetSocket.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include <ramdisk/include/services/common/PidFile.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <stdlib.h>
#include <signal.h>
#include <sstream>

LOG_DECLARE_FILE("cios.jobctld");

using namespace bgcios;
using namespace bgcios::jobctl;

//! \brief  Main entry point for jobctld.
//! \return 0 when successful, 1 when unsuccessful.

int main(int argc, char *argv[])
{


   bgcios::SignalHandler sigsegvHandler(SIGSEGV);
   bgcios::SignalHandler sigfpeHandler(SIGFPE);
   bgcios::SignalHandler sigbusHandler(SIGBUS);
   bgcios::SignalHandler sigillHandler(SIGILL);
   bgcios::SignalHandler sigsysHandler(SIGSYS);
   bgcios::SignalHandler sigabrtHandler(SIGABRT);
    
   sigset_t blockedSet;
   sigemptyset(&blockedSet);
   sigaddset(&blockedSet, SIGCHLD);
   if (pthread_sigmask(SIG_BLOCK, &blockedSet, NULL) != 0) {
      LOG_ERROR_MSG("error setting blocked signal mask: " << bgcios::errorString(errno));
   }

    
    // Ignore these signals
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    const int signals_ignored[] = { SIGHUP, SIGUSR2, SIGPIPE, SIGTRAP };
    const int num_signals_ignored = sizeof(signals_ignored) / sizeof(signals_ignored[0]);
    sa.sa_handler=SIG_IGN;
    for (int i = 0; i < num_signals_ignored; i++)
    {
        if (sigaction(signals_ignored[i], &sa, NULL))
        {
            LOG_ERROR_MSG( "cios sigaction ignored: " << strerror(errno) );
        }
    }

   // Set working directory.
   if (chdir("/") != 0) {
      LOG_ERROR_MSG("error changing working directory to '/': " << bgcios::errorString(errno));
   }

   //setFlightLogSize(4096);
   setFlightLogSize(32768);

   // Create configuration from command-line arguments and properties.
   JobctlConfigPtr config = JobctlConfigPtr(new JobctlConfig(argc, argv));

   LOGGING_DECLARE_PID_MDC( getpid() );
   LOGGING_DECLARE_SERVICE_MDC( "jobctld" );

   uint16_t simId = config->getSimulationId();
   in_port_t port = config->getListenPort();

   // Create a JobController object.
   JobControllerPtr controller;
   HwJobControllerPtr hwController;
   SimJobControllerPtr simController;

   if (bgcios::isHardware(simId)) {
      hwController = HwJobControllerPtr(new HwJobController(config));
      controller = std::tr1::static_pointer_cast<JobController>(hwController);

      // The last cpu is handling I/O link message traffic so stay off of it.
      size_t numCpus = controller->getNumCpus();
      controller->stayOffCpu(numCpus-1);

      LOG_CIOS_TRACE_MSG("running on hardware");
   }
   else {
      simController = SimJobControllerPtr(new SimJobController(config));
      controller = std::tr1::static_pointer_cast<JobController>(simController);
      if (!(bgcios::isIONSimulation(simId))) {
         port = 0;
      }
      LOG_CIOS_INFO_MSG("simulation mode enabled for id " << simId);
   }

   // Create a pid file for the daemon.
   bgcios::PidFile pidFile("jobctld", controller->isHardware());
   LOG_CIOS_INFO_MSG("started " << controller->getVersionString("jobctld", (int)ProtocolVersion) << " in process " << pidFile.getPid());

   // Start channels for handling messages.
   int err = controller->startup(port);
   if (err != 0) {
      LOG_FATAL_MSG("error starting channels for handling messages: " << bgcios::errorString(err));
      bgcios::RasEvent rasEvent(bgcios::DaemonInitFailed);
      rasEvent << err;
      rasEvent.send();
      exit(EXIT_FAILURE);
   }

   // Let iosd know we are ready.
   err = controller->sendReady(10001,0,controller->getIosdCmdChannelPath(),bgcios::IosctlService); // Special service id
   if (err != 0) {
      LOG_FATAL_MSG("error sending Ready message to iosd: " << bgcios::errorString(err));
      exit(EXIT_FAILURE);
   }
   LOG_CIOS_DEBUG_MSG("jobctld services are ready");

   // Monitor for events on all of the channels until told to stop.
   LOG_CIOS_DEBUG_MSG("starting to monitor for events ...");
       
   while (!controller->isTerminated()) {
      controller->eventMonitor();
   }
   LOG_CIOS_DEBUG_MSG("stopped monitoring for events");

   // Finish cleaning up and exit.
   controller->cleanup();
   LOG_CIOS_INFO_MSG("exited normally");
   exit(EXIT_SUCCESS);
}

