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

#include "ConsoleConnectionThread.h"

#include "CommandProcessor.h"
#include "ConsoleMonitorParameters.h"
#include "DBConsoleController.h"

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

void*
ConsoleConnectionThread::threadStart()
{
    // Get the parameters
    boost::shared_ptr<ConsoleMonitorParameters> parms;
    this->getArg( parms );

    mmcs_client::ConsolePortClient* const connection( dynamic_cast<mmcs_client::ConsolePortClient*>(parms->consolePort) );
    if ( !connection ) {
        LOG_WARN_MSG( "Unable to obtain connection." );
        return NULL;
    }

    MMCSCommandMap* const commandMap( parms->commandMap );

    CxxSockets::SockAddr local;
    CxxSockets::SockAddr remote;
    connection->getSock()->getSockName(local);
    connection->getSock()->getPeerName(remote);
    LOG_TRACE_MSG("Connected to " << remote.getHostAddr()
            << ":" << remote.getServicePort() << " on port "
            << local.getServicePort());

    // Perform SSL handshake
    bgq::utility::ServerPortConfiguration port_config(0, bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand);
    port_config.setProperties(common::Properties::getProperties(), "");
    port_config.notifyComplete();
    const CxxSockets::SecureTCPSocketPtr socket(
            new CxxSockets::SecureTCPSocket(
                connection->getSock(),
                port_config
                )
            );
    connection->setSock( socket );

    // Get user on the other end
    bgq::utility::UserId uid;
    CxxSockets::UserType utype( CxxSockets::Normal );
    const CxxSockets::SecureTCPSocketPtr sock(
            boost::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>( connection->getSock() )
            );
    if (sock) {
        uid = sock->getUserId();
        utype = sock->getUserType();
    } else {
        LOG_WARN_MSG( "Could not get remote user." );
        return NULL;
    }

    log4cxx::MDC _userid_mdc_( "user", std::string("[") + uid.getUser() + "] " );

    // Create a midplane controller object
    CommandProcessor commandProcessor(commandMap); // MMCS command processor
    DBConsoleController midplaneController(&commandProcessor, uid, utype, connection);
    setThreadName("console");
    midplaneController.setMMCSThread(this);

    LOG_DEBUG_MSG("Started as " << (utype == CxxSockets::Administrator ? "administrator" : "command") );

    // Execute commands received on this connection
    midplaneController.run();

    return NULL;
}

} } // namespace mmcs::server
