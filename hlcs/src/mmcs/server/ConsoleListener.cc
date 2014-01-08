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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "ConsoleListener.h"

#include "ConsoleConnectionThread.h"
#include "ConsoleMonitorParameters.h"

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

void*
ConsoleListener::threadStart()
{
    bool portWorking = true; // false == permanent socket error on accept

    // Get the parameters
    boost::shared_ptr<ConsoleMonitorParameters> parms;
    this->getArg( parms );
    mmcs_client::ConsolePortServer* consolePort = (mmcs_client::ConsolePortServer*) parms->consolePort;
    MMCSCommandMap* commandMap = (MMCSCommandMap*) parms->commandMap;

    while (!isThreadStopping() && portWorking) {
        mmcs_client::ConsolePortClient* connection = NULL;
        // Wait for a connection from a client
        try {
            connection = consolePort->accept();
        } catch (const mmcs_client::ConsolePort::Error& e) {
            connection = NULL; // we didn't get a connection
            if (e.errcode == EINTR || e.errcode == ECONNABORTED || e.errcode == EMFILE ||
                    e.errcode == ENFILE || e.errcode == ENOBUFS || e.errcode == ENOMEM ||
                    e.errcode == -1 || e.errcode == 0); // recoverable error, retry accept
            else {
                if (e.errcode != EINVAL && e.errcode != 0) { // we normally receive EINVAL on shutdown
                    LOG_ERROR_MSG("Error occurred listening for console clients to connect: " << e.what() << " " << e.errcode);
                }
                portWorking = false;
            }
        }

        if (connection) {
            // Create a thread to service the connection
            ConsoleConnectionThread* serverThread = new ConsoleConnectionThread;
            serverThread->setArg(
                    boost::shared_ptr<ConsoleMonitorParameters>(
                        new ConsoleMonitorParameters(connection, commandMap)
                        )
                    );
            serverThread->setJoinable(false);
            serverThread->setDeleteOnExit(true); // delete this object when the thread exits
            serverThread->start();
        }
    }

    // Close the interactive console port
    delete consolePort;
    consolePort = NULL;

    // Return when done
    return NULL;
}

} } // namespace mmcs::server
