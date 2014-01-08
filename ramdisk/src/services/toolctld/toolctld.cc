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

//! \file  toolctld.cc
//! \brief Tool control daemon.

// Includes
#include "ToolctlConfig.h"
#include "ToolController.h"
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include <ramdisk/include/services/common/PidFile.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <stdlib.h>

using namespace bgcios::toolctl;

LOG_DECLARE_FILE("cios.toolctld");

int main(int argc, char *argv[])
{

   bgcios::SignalHandler sigsegvHandler(SIGSEGV);
   bgcios::SignalHandler sigfpeHandler(SIGFPE);
   bgcios::SignalHandler sigbusHandler(SIGBUS);
   bgcios::SignalHandler sigillHandler(SIGILL);
   bgcios::SignalHandler sigsysHandler(SIGSYS);
   bgcios::SignalHandler sigtrapHandler(SIGTRAP);
   bgcios::SignalHandler sigabrtHandler(SIGABRT);

   // Set working directory.
   if (chdir("/") != 0) {
      LOG_ERROR_MSG("error changing working directory to '/': " << bgcios::errorString(errno));
   }

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
            LOG_ERROR_MSG( "cios toolctld sigaction ignored: " << strerror(errno) );
        }
    }

   // Create configuration from command-line arguments and properties.
   ToolctlConfig config(argc, argv);

   LOGGING_DECLARE_PID_MDC( getpid() );
   LOGGING_DECLARE_SERVICE_MDC( "toolctld" );

   // Create a ToolController object.
   ToolControllerPtr controller = ToolControllerPtr(new ToolController());

   // Create a pid file for the daemon.
   std::ostringstream name;
   name << "toolctld." << config.getServiceId();
   bgcios::PidFilePtr pidFile = bgcios::PidFilePtr(new bgcios::PidFile(name.str(), controller->isHardware()));
   LOG_CIOS_INFO_MSG("started " << controller->getVersionString(name.str(), (int)ProtocolVersion) << " in process " << pidFile->getPid());

   // Start channels for handling messages.
   int err = controller->startup(config.getServiceId());
   if (err != 0) {
      LOG_FATAL_MSG("error starting channels for handling messages: " << bgcios::errorString(err));
      pidFile.reset();
      bgcios::RasEvent rasEvent(bgcios::DaemonInitFailed);
      rasEvent << err;
      rasEvent.send();
      exit(EXIT_FAILURE);
   }

   // let jobctld know we are ready
   std::ostringstream jobctldCmdChannelPath;
   jobctldCmdChannelPath << controller->getWorkDirectory() << bgcios::JobctlCommandChannelName;
   err = controller->sendReady(config.getServiceId(),
                 controller->getRdmaListener()->getLocalPort(),
                 jobctldCmdChannelPath.str(),bgcios::ToolctlService);
   if (err != 0) {
      LOG_FATAL_MSG("error sending Ready message to jobctld: " << bgcios::errorString(err));
      pidFile.reset();
      exit(EXIT_FAILURE);
   }
   LOG_CIOS_DEBUG_MSG("initialization complete in pid " << pidFile->getPid());

   // Monitor for events on all of the channels until told to stop.
   LOG_CIOS_DEBUG_MSG("starting to monitor for events ...");
   while (!controller->isTerminated()) {
      controller->eventMonitor();
   }
   LOG_CIOS_DEBUG_MSG("stopped monitoring for events");

   // Finish cleaning up and exit.
   controller.reset();
   pidFile.reset();
   LOG_CIOS_INFO_MSG("exited normally");
   exit(EXIT_SUCCESS);
}

