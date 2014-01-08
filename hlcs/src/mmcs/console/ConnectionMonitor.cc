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

#include "ConnectionMonitor.h"

#include "command/MmcsServerCmd.h"
#include "common/ConsoleController.h"

#include "libmmcs_client/ConsolePort.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.console" );

namespace mmcs {
namespace console {

ConnectionMonitor::ConnectionMonitor(
        common::ConsoleController* p
        ) :
    _controller(p)
{

}

void*
ConnectionMonitor::threadStart()
{
    LOG_DEBUG_MSG("Starting");
    while (!isThreadStopping()) {
        try {
            if (_controller->getConsolePort()) {
                _controller->getConsolePort()->checkConnection();
                sleep(1);
            } else {
                LOG_INFO_MSG("Connecting to mmcs_server");
                mmcs_client::CommandReply rep;
                console::command::MmcsServerCmd::reconnect_to_server(rep, _controller);
                if ( rep.getStatus() ) {
                    sleep(5);
                }
            }
        } catch (const mmcs_client::ConsolePort::Error& e) {
            delete _controller->getConsolePort();
            _controller->setConsolePort(NULL);
            LOG_ERROR_MSG("Console disconnected from server. " << e.what() );
        }
    }
    LOG_DEBUG_MSG("Ending");
    return 0;
};

} } // namespace mmcs::console
