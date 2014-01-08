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
#include "HwStdioController.h"
#include <ramdisk/include/services/common/InetSocket.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/common/PidFile.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <stdlib.h>
#include <sstream>

using namespace bgcios::stdio;

LOG_DECLARE_FILE("cios.stdiod");

uint32_t logChanEvent(uint32_t ID,struct rdma_cm_event * event)
{
    printf("logChanEvent(%d  %p)\n", ID, event);
    return 0;
}

int main(int argc, char *argv[])
{
    printf("main(%d, %s)\n", argc, argv[0]);
    // Create a StdioController object.
    HwStdioControllerPtr controller;
    
    controller = HwStdioControllerPtr(new HwStdioController());
    // Start channels for handling messages.
    int err = controller->startup();
    if (err != 0) 
    {
        printf("failure\n");
        exit(EXIT_FAILURE);
   }

   // Monitor for events on all of the channels until told to stop.
   while (!controller->isTerminated()) 
   {
      controller->eventMonitor();
   }

   // Finish cleaning up and exit.
   controller->cleanup();
   printf("exiting normally\n");
   exit(EXIT_SUCCESS);
}
