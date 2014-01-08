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
 * \file MMCSCommand_mmcs_server_connect.cc
 */

#include <boost/scope_exit.hpp>
#include "MMCSCommand_mmcs_server_connect.h"
#include "MMCSCommandProcessor.h"
#include "MMCSProperties.h"
#include "MMCSConsolePort.h"
#include "ConsoleController.h"
#include "MMCSCommand_mmcs_server_cmd.h"

#include <utility/include/Log.h>
#include <utility/include/ScopeGuard.h>

LOG_DECLARE_FILE( "mmcs" );

MMCSCommand_mmcs_server_connect*
MMCSCommand_mmcs_server_connect::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.internalAuth(true);
    commandAttributes.helpCategory(DEFAULT);
    return new MMCSCommand_mmcs_server_connect("mmcs_server_connect", "mmcs_server_connect [<retry>]", commandAttributes);
}

void
MMCSCommand_mmcs_server_connect::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget)
{
    bool connected = false;
    bool error_reported = false;
    bool wait_for_connection = false;
    MMCSConsolePortClient* serverPort = NULL;

    // validate arguments
    if (args.size() > 0)
    {
        if (args[0] == "retry")
            wait_for_connection = true;
        else
        {
            reply << FAIL << "args? " << usage <<  DONE;
            return;
        }
    }

    if (pController->getConsolePort()) // delete any existing server connection
    {
        reply << FAIL << "already connected" << DONE;
        return;
    }

    // create a console port for communicating with the MMCS server
    while (!connected && !MMCSCommand_mmcs_server_cmd::ending())
    {
        try {
            bgq::utility::PortConfiguration::Pairs portpairs;
            std::string servname = "32031";
            std::string portstr = MMCSProperties::getProperty(HOST_PORTS);

            bgq::utility::ClientPortConfiguration port_config(servname);
            port_config.setProperties( MMCSProperties::getProperties(), "bg_console");
            port_config.notifyComplete();
            if(portstr == "127.0.0.1:32031" ) {
                // Using the config file
                portpairs = port_config.getPairs();
            } else {
                // We got a command line parameter, so use that instead.
                bgq::utility::PortConfiguration::parsePortsStr(portstr, servname, portpairs);
            }

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
                        LOG_WARN_MSG( "could not seteuid(" << real << ") " << strerror(errno) );
                    } else {
                        LOG_TRACE_MSG( "reduced euid to " << geteuid() );
                    }
                }
            } BOOST_SCOPE_EXIT_END

            serverPort = new MMCSConsolePortClient(portpairs);

            connected = true;
            pController->setConsolePort(serverPort); // save the server port in the ConsoleController
        }
        catch (MMCSConsolePort::Error &e)
        {
            if (e.errcode == ECONNREFUSED)
            {
                if (wait_for_connection)
                {
                    if (!error_reported)
                    {
                        LOG_WARN_MSG("mmcs_server is not started, retrying. Type quit or ctl-c to quit");
                        error_reported = true;
                    }
                    sleep(5);
                }
                else
                {
                    reply << FAIL << e.what() << "... mmcs_server is not started" << DONE;
                    return;
                }
            }
            else if (e.errcode == CxxSockets::SSL_ERROR)
            {
                LOG_FATAL_MSG( e.what() );
                pController->quit(EXIT_FAILURE);
                MMCSCommand_mmcs_server_cmd::ending(true);
            }
            else
            {
                reply << FAIL << e.what() << DONE;
                return;
            }
        }
    }

    MMCSConsoleCommandProcessor mmcsCommandProcessor(*(MMCSConsoleCommandProcessor*)(pController->getCommandProcessor())); // MMCS command processor
    mmcsCommandProcessor.logFailures(false); // don't clutter the console with log messages
    pController->setReplyFormat(0); // new connections start out in reply format 0

    // set the reply format
    deque<string> replyformat_cmd = MMCSCommandProcessor::parseCommand("replyformat 1");
    mmcsCommandProcessor.execute(replyformat_cmd, reply, pController);
    if (reply.getStatus() != 0)
    {
        delete pController->getConsolePort();
        pController->setConsolePort(NULL);
        return;
    }
    reply << OK << DONE;
}

void
MMCSCommand_mmcs_server_connect::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Establish a tcp connection to the mmcs server."
        << ";retry indicates that the connection attempt is repeated until"
        << ";the connection is established or a SIGINT is received."
        << ";Once established, the username and replyformat are sent to the server."
        << ";This command is used internally by bg_console."
        << DONE;
}

void
MMCSCommand_mmcs_server_connect::raiseUid() const
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

    // raise priviliges to read private key
    if ( effective != saved ) {
        if ( seteuid(saved) ) {
            LOG_WARN_MSG( "could not seteuid(" << saved << ") " << strerror(errno) );
        } else {
            LOG_TRACE_MSG( "raised euid to " << geteuid() );
        }
    }
}

