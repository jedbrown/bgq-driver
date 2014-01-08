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

//! \file  gdbtool.cc
//! \brief GDB server tool.

// Includes
#include "GdbtoolConfig.h"
#include "GdbCommandChannel.h"
#include "GdbController.h"
#include <ramdisk/include/services/common/SignalHandler.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <stdlib.h>

using namespace bgcios::gdbtool;

LOG_DECLARE_FILE("cios.gdbtool");

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

   // Create configuration from command-line arguments and properties.
   GdbtoolConfig config(argc, argv);

   // Create a GdbController object.
   GdbControllerPtr controller = GdbControllerPtr(new GdbController());

   // Start channels for handling messages.
   int err = controller->startup(INADDR_ANY, config.getListenPort(), config.getRank());
   if (err != 0) {
      LOG_FATAL_MSG("error starting channels for handling messages: " << bgcios::errorString(err));
      exit(EXIT_FAILURE);
   }

   // Monitor for events on all of the channels until told to stop.
   LOG_DEBUG_MSG("starting to monitor for events ...");
   controller->eventMonitor(sigtermHandler);
   LOG_DEBUG_MSG("stopped monitoring for events");

   // Finish cleaning up and exit.
   controller->cleanup();
   LOG_INFO_MSG("exited normally");
   exit(EXIT_SUCCESS);
}

