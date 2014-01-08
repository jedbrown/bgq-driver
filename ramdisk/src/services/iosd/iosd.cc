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

//! \file  iosd.cc
//! \brief I/O services daemon.

// Includes
#include "IosConfig.h"
#include "HwIosController.h"
#include "SimIosController.h"
#include "ServiceManager.h"
#include "ServiceMonitor.h"
#include <ramdisk/include/services/common/LocalSocket.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/SignalHandler.h>
#include <ramdisk/include/services/common/PidFile.h>
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/IosctlMessages.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sstream>

LOG_DECLARE_FILE("cios.iosd");

using namespace bgcios::iosctl;

//! \brief  Main 
//! \return 0 when successful, 1 when unsuccessful.

int main(int argc, char *argv[])
{
   // Set actions for signals.
   signal(SIGPIPE, SIG_IGN);

   bgcios::SignalHandler sigsegvHandler(SIGSEGV);
   bgcios::SignalHandler sigfpeHandler(SIGFPE);
   bgcios::SignalHandler sigbusHandler(SIGBUS);
   bgcios::SignalHandler sigillHandler(SIGILL);
   bgcios::SignalHandler sigsysHandler(SIGSYS);
   bgcios::SignalHandler sigtrapHandler(SIGTRAP);
   bgcios::SignalHandler sigabrtHandler(SIGABRT);
   bgcios::SigtermHandler sigtermHandler;
   bgcios::Sigusr2Handler sigusr2Handler;

   // Create configuration from command-line arguments and properties.
   IosConfig config(argc, argv);

   LOGGING_DECLARE_PID_MDC( getpid() );
   LOGGING_DECLARE_SERVICE_MDC( "iosd" );

   uint16_t simId = config.getSimulationId();
   in_port_t port = config.getListenPort();

   // When running on hardware, turn into a daemon but keep using the console for stdout and stderr.
   if (bgcios::isHardware(simId)) {
      if (daemon(0, 1) != 0) {
         LOG_FATAL_MSG("error turning into a daemon: " << bgcios::errorString(errno)); 
         exit(EXIT_FAILURE);
      }
   }

   // Create a service manager object.
   ServiceManagerPtr serviceManager = ServiceManagerPtr(new ServiceManager(config.getMaxServiceRestarts()));

   // Create a IosController object.
   IosControllerPtr controller;
   HwIosControllerPtr hwController;
   SimIosControllerPtr simController;

   if (bgcios::isHardware(simId)) {
      hwController = HwIosControllerPtr(new HwIosController(config, serviceManager));
      controller = std::tr1::static_pointer_cast<IosController>(hwController);

      // The last cpu is handling I/O link message traffic so stay off of it.
      size_t numCpus = controller->getNumCpus();
      controller->stayOffCpu(numCpus-1);

      LOG_CIOS_TRACE_MSG("running on hardware");
   }
   else {
      simController = SimIosControllerPtr(new SimIosController(simId));
      controller = std::tr1::static_pointer_cast<IosController>(simController);
//    addr = 0; // Use ephemeral address for simulation.
      port = 0;
      LOG_CIOS_INFO_MSG("simulation mode enabled for id " << simId);
   }
   serviceManager->setController(controller);

   // Create a pid file for the daemon.
   bgcios::PidFile pidFile("iosd", controller->isHardware());

   // Start channels for handling messages.
   int err = controller->startup(port);
   if (err != 0) {
      LOG_FATAL_MSG("error starting channels for handling messages: " << bgcios::errorString(err));
      bgcios::RasEvent rasEvent(bgcios::DaemonInitFailed);
      rasEvent << err;
      rasEvent.send();
      exit(EXIT_FAILURE);
   }

   // Build the list of arguments for all services.
   std::vector <std::string> jobctldArguments;
   std::vector <std::string> stdiodArguments;

   // Explicitly set the --properties argument for all services.
   std::ostringstream propertiesArg;
   propertiesArg << "--properties=" << config.getProperties()->getFilename();
   jobctldArguments.push_back(propertiesArg.str());
   stdiodArguments.push_back(propertiesArg.str());

   // Set arguments needed for running in simulation mode.
   if (bgcios::isSimulation(simId)) {

      // Explicitly set the --simulationId argument.
      // Note this is required so multiple control system simulators can run simultaneously.
      std::ostringstream simulationIdArg;
      simulationIdArg << "--simulation_id=" << simId;
      jobctldArguments.push_back(simulationIdArg.str());
      stdiodArguments.push_back(simulationIdArg.str());
   }

   LOG_INFO_MSG_FORCED("Compile date="<<__DATE__<<" TIME="<<__TIME__);

   // To begin, start the services that are connected to the service node.  There is one instance of 
   // these services on the I/O node.

   // Start a job control daemon.
   std::ostringstream jobctldCmdChannelPath;
   jobctldCmdChannelPath << controller->getWorkDirectory() << bgcios::JobctlCommandChannelName;
   if (bgcios::isSimulation(simId)) {
      jobctldCmdChannelPath << "." << getpid();
   }
   serviceManager->start(config.getJobctlDaemonPath(), jobctldArguments, jobctldCmdChannelPath.str());

   // Start a standard I/O daemon.
   std::ostringstream stdiodCmdChannelPath;
   stdiodCmdChannelPath << controller->getWorkDirectory() << bgcios::StdioCommandChannelName;
   if (bgcios::isSimulation(simId)) {
      stdiodCmdChannelPath << "." << getpid();
   }
   serviceManager->start(config.getStdioDaemonPath(), stdiodArguments, stdiodCmdChannelPath.str());

   // Get Ready message from all services.
   err = controller->waitForReadyMessages((int)serviceManager->listSize());
   if (err != 0) {
      LOG_FATAL_MSG("error receiving Ready messages from services: " << bgcios::errorString(err));
      exit(EXIT_FAILURE);
   }
   LOG_CIOS_INFO_MSG("jobctld and stdiod services are ready");

   // Start a thread to monitor all services.
   ServiceMonitorPtr monitor = ServiceMonitorPtr(new ServiceMonitor(serviceManager));
   err = monitor->start();
   if (err != 0) {
      LOG_ERROR_MSG("error starting service monitor thread: " << bgcios::errorString(err));
   }

   // Monitor for events on all of the channels until told to stop.
   LOG_CIOS_DEBUG_MSG("starting to monitor for events ...");
   while (!sigtermHandler.isCaught()) {
      controller->eventMonitor(sigtermHandler);
   }

   // Tell the monitor thread to stop.
   LOG_CIOS_DEBUG_MSG("stopped monitoring for events");
   monitor->stop();

   // Tell all services to terminate now.
   if (sigtermHandler.isCaught()) {
     //do nothing--place holder
   }
   // Finish cleaning up and exit.
   controller->cleanup();
   LOG_CIOS_INFO_MSG("exited normally");
   exit(EXIT_SUCCESS);

}
