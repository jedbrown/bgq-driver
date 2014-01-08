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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include "MMCSConnectionMonitor.h"
#include "MMCSConsolePort.h"
#include "MMCSCommand_mmcs_server_cmd.h"

LOG_DECLARE_FILE("mmcs");

void*
ConnectionMonitor::threadStart() {
    sleep(5);  // Wait a bit to start
    while(isThreadStopping() == false) {
        try {
            if(_controller->getConsolePort()) {
                LOG_TRACE_MSG("Checking connection to mmcs_server");
                _controller->getConsolePort()->checkConnection();
            } else {
                LOG_INFO_MSG("Connecting to mmcs_server");
                MMCSCommandReply rep;
                MMCSCommand_mmcs_server_cmd::reconnect_to_server(rep, _controller);
            }
        }
        catch(MMCSConsolePort::Error& e) {
            LOG_ERROR_MSG("Console disconnected from server.  " << e.what()
                          << "  Attempting to reconnect.");
            MMCSCommandReply rep;
            MMCSCommand_mmcs_server_cmd::reconnect_to_server(rep, _controller);
        }
        sleep(1);
    }
    LOG_DEBUG_MSG("Ending Connection Monitor");
    return 0;
};
