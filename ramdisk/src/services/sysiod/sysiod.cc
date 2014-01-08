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

//! \file  sysiod.cc
//! \brief System I/O daemon.

// Includes
#include "SysioConfig.h"
#include "SysioController.h"
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/SignalHandler.h>

#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>

using namespace bgcios::sysio;

LOG_DECLARE_FILE("cios.sysiod");

int main(int argc, char *argv[])
{

   bgcios::SignalHandler sigsegvHandler(SIGSEGV);
   bgcios::SignalHandler sigfpeHandler(SIGFPE);
   bgcios::SignalHandler sigbusHandler(SIGBUS);
   bgcios::SignalHandler sigillHandler(SIGILL);
   bgcios::SignalHandler sigsysHandler(SIGSYS);
   bgcios::SignalHandler sigabrtHandler(SIGABRT);
   bgcios::SigtermHandler sigtermHandler;
   bgcios::Sigusr2Handler sigusr2Handler;

    
    // Ignore these signals
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    const int signals_ignored[] = { SIGHUP, SIGUSR1, SIGPIPE, SIGTRAP };
    const int num_signals_ignored = sizeof(signals_ignored) / sizeof(signals_ignored[0]);
    sa.sa_handler=SIG_IGN;
    for (int i = 0; i < num_signals_ignored; i++)
    {
        if (sigaction(signals_ignored[i], &sa, NULL))
        {
            LOG_ERROR_MSG( "cios sysiod sigaction ignored: " << strerror(errno) );
        }
    }


   // Set working directory.
   if (chdir("/") != 0) {
      LOG_ERROR_MSG("error changing working directory to '/': " << bgcios::errorString(errno));
   }

   setFlightLogSize(1024);

   // Create configuration from command-line arguments and properties.
   SysioConfigPtr config = SysioConfigPtr(new SysioConfig(argc, argv));

   LOGGING_DECLARE_PID_MDC( getpid() );
   LOGGING_DECLARE_SERVICE_MDC( "sysiod" );

   // Create a SysioController object.
   SysioControllerPtr controller = SysioControllerPtr(new SysioController());

   // Start channels for handling messages.
   int err = controller->startup(config);
   if (err != 0) {
      LOG_FATAL_MSG("error starting channels for handling messages: " << bgcios::errorString(err));

      bgcios::RasEvent rasEvent(bgcios::DaemonInitFailed);
      rasEvent << err;
      rasEvent.send();
      exit(EXIT_FAILURE);
   }

   // let jobctld know we are ready
   std::ostringstream jobctldCmdChannelPath;
   jobctldCmdChannelPath << controller->getWorkDirectory() << bgcios::JobctlCommandChannelName;

   err = controller->sendReady(config->getServiceId(),
    controller->getClientMonitor()->getRdmaListener()->getLocalPort(),
   jobctldCmdChannelPath.str(),bgcios::SysioService);
   if (err != 0) {
      LOG_FATAL_MSG("error sending Ready message to jobctld: " << bgcios::errorString(err));
      exit(EXIT_FAILURE);
   }

   // Monitor for events on all of the channels until told to stop.
   LOG_CIOS_DEBUG_MSG("starting to monitor for events ...");
   while (!controller->isTerminated()) {
      controller->eventMonitor();
   }
   LOG_CIOS_DEBUG_MSG("stopped monitoring for events");

   // Finish cleaning up and exit.
   controller.reset();
   LOG_CIOS_INFO_MSG("exited normally");
   exit(EXIT_SUCCESS);
}

