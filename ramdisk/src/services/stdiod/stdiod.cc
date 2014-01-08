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

//! \file  stdiod.cc
//! \brief Standard I/O daemon.

// Includes
#include "StdioConfig.h"
#include "HwStdioController.h"
#include "SimStdioController.h"
#include <ramdisk/include/services/common/InetSocket.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include <ramdisk/include/services/common/PidFile.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <stdlib.h>
#include <sstream>

using namespace bgcios::stdio;

LOG_DECLARE_FILE("cios.stdiod");

int main(int argc, char *argv[])
{
   bgcios::SignalHandler sigsegvHandler(SIGSEGV);
   bgcios::SignalHandler sigfpeHandler(SIGFPE);
   bgcios::SignalHandler sigbusHandler(SIGBUS);
   bgcios::SignalHandler sigillHandler(SIGILL);
   bgcios::SignalHandler sigsysHandler(SIGSYS);
   bgcios::SignalHandler sigabrtHandler(SIGABRT);

    // Ignore these signals
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    const int signals_ignored[] = { SIGHUP, SIGUSR2, SIGUSR1, SIGPIPE, SIGTRAP };
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
   
   setFlightLogSize(32768);

   // Create configuration from command-line arguments and properties.
   StdioConfigPtr config = StdioConfigPtr(new StdioConfig(argc, argv));

   LOGGING_DECLARE_PID_MDC( getpid() );
   LOGGING_DECLARE_SERVICE_MDC( "stdiod" );

   uint16_t simId = config->getSimulationId();
   in_port_t port = config->getListenPort();

   // Create a StdioController object.
   StdioControllerPtr controller;
   HwStdioControllerPtr hwController;
   SimStdioControllerPtr simController;

   if (bgcios::isHardware(simId)) {
      hwController = HwStdioControllerPtr(new HwStdioController(config));
      controller = std::tr1::static_pointer_cast<StdioController>(hwController);

      // The last cpu is handling I/O link message traffic so stay off of it.
      size_t numCpus = controller->getNumCpus();
      controller->stayOffCpu(numCpus-1);

      LOG_CIOS_TRACE_MSG("running on hardware");
   }
   else {
      simController = SimStdioControllerPtr(new SimStdioController(config));
      controller = std::tr1::static_pointer_cast<StdioController>(simController);
      if (!(bgcios::isIONSimulation(simId))) {
         port = 0;
      }
      LOG_CIOS_INFO_MSG("simulation mode enabled for id " << simId);
   }

   // Create a pid file for the daemon.
   bgcios::PidFile pidFile("stdiod", controller->isHardware());
   LOG_CIOS_INFO_MSG("started " << controller->getVersionString("stdiod", (int)ProtocolVersion) << " in process " << pidFile.getPid());

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
   err = controller->sendReady(20002,0,controller->getIosdCmdChannelPath(),bgcios::IosctlService);//special service id
   if (err != 0) {
      LOG_FATAL_MSG("error sending Ready message to iosd: " << bgcios::errorString(err));
      exit(EXIT_FAILURE);
   }
   LOG_CIOS_DEBUG_MSG("stdiod services are ready");

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

