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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file MmcsServerConnect.cc
 */

#include "MmcsServerConnect.h"

#include "MmcsServerCmd.h"

#include "../CommandProcessor.h"

#include "common/ConsoleController.h"

#include "libmmcs_client/ConsolePort.h"

#include <utility/include/Log.h>
#include <utility/include/ScopeGuard.h>

#include <boost/scope_exit.hpp>

LOG_DECLARE_FILE( "mmcs.console" );

using namespace std;

namespace mmcs {
namespace console {
namespace command {

MmcsServerConnect*
MmcsServerConnect::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(common::DEFAULT);
    return new MmcsServerConnect("mmcs_server_connect", "mmcs_server_connect [<retry>]", commandAttributes);
}

void
MmcsServerConnect::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    bool connected = false;
    bool error_reported = false;
    bool wait_for_connection = false;

    // validate arguments
    if (!args.empty()) {
        if (args[0] == "retry")
            wait_for_connection = true;
        else {
            reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
            return;
        }
    }

    LOG_DEBUG_MSG("Wait for connection: " << wait_for_connection);

    if (pController->getConsolePort()) { // delete any existing server connection
        reply << mmcs_client::FAIL << "Already connected" << mmcs_client::DONE;
        return;
    }

    // create a console port for communicating with the MMCS server
    while (!connected && !command::MmcsServerCmd::ending()) {
        try {
            // raise uid to read SSL private key if needed
            this->raiseUid();

            // when we go out of scope, drop euid back to real uid
            BOOST_SCOPE_EXIT( ) {
                uid_t real( 0 );
                uid_t effective( 0 );
                uid_t saved( 0 );
                (void)getresuid( &real, &effective, &saved );
                if ( effective != real ) {
                    if ( seteuid(real) ) {
                        LOG_WARN_MSG("Could not seteuid(" << real << ") " << strerror(errno) );
                    } else {
                        LOG_TRACE_MSG("Reduced euid to " << geteuid());
                    }
                }
            } BOOST_SCOPE_EXIT_END

            mmcs_client::ConsolePortClient* const serverPort(
                    new mmcs_client::ConsolePortClient(*pController->getPortConfiguration())
                    );

            connected = true;
            pController->setConsolePort(serverPort); // save the server port in the common::ConsoleController
        } catch (const mmcs_client::ConsolePort::Error &e) {
            LOG_TRACE_MSG( e.what() );
            if (e.errcode == ECONNREFUSED) {
                if (wait_for_connection) {
                    if (!error_reported) {
                        LOG_WARN_MSG("mmcs_server is not started, retrying. Type quit or ctl-c to quit.");
                        error_reported = true;
                    }
                    sleep(5);
                } else {
                    reply << mmcs_client::FAIL << e.what() << "... mmcs_server is not started" << mmcs_client::DONE;
                    return;
                }
            } else {
                reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
                return;
            }
        }
    }

    CommandProcessor mmcsCommandProcessor(*(CommandProcessor*)(pController->getCommandProcessor())); // MMCS command processor
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
MmcsServerConnect::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";Establish a TCP connection to the mmcs server."
        << ";retry indicates that the connection attempt is repeated until"
        << ";the connection is established or a SIGINT is received."
        << ";Once established, the username is sent to the server."
        << ";This command is used internally by bg_console."
        << mmcs_client::DONE;
}

void
MmcsServerConnect::raiseUid() const
{
    // get real, effective, and saved uid to determine if we need to raise effective
    // uid to read private key for SSL handshaking
    uid_t real( 0 );
    uid_t effective( 0 );
    uid_t saved( 0 );
    (void)getresuid( &real, &effective, &saved );
    LOG_TRACE_MSG( "real uid:       " << real );
    LOG_TRACE_MSG( "effective uid:  " << effective );
    LOG_TRACE_MSG( "saved uid:      " << saved );

    // raise privileges to read private key
    if ( effective != saved ) {
        if ( seteuid(saved) ) {
            LOG_WARN_MSG("Could not seteuid(" << saved << ") " << strerror(errno));
        } else {
            LOG_TRACE_MSG("Raised euid to " << geteuid());
        }
    }
}

} } } // namespace mmcs::console::command
