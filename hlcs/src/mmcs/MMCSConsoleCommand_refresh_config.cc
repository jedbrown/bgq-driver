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

#include "MMCSConsoleCommand_refresh_config.h"

#include "ConsoleController.h"
#include "MMCSCommandProcessor.h"
#include "MMCSProperties.h"

#include <utility/include/ScopeGuard.h>

#include <boost/assign/std/vector.hpp>

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <algorithm>

#include <stdlib.h> // system

MMCSConsoleCommand_refresh_config::MMCSConsoleCommand_refresh_config(
        const char* name,
        const char* description,
        const MMCSCommandAttributes& attributes
        ) :
    MMCSCommand( name, description, attributes ),
    _servers()
{
    using namespace boost::assign;
    _servers += "bgmaster_server", "bgws_server", "mmcs_server", "runjob_mux", "runjob_server";
    std::sort( _servers.begin(), _servers.end() );
    usage = "refresh_config [server [/path/to/properties]]";
}

MMCSConsoleCommand_refresh_config*
MMCSConsoleCommand_refresh_config::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);
    commandAttributes.requiresConnection(false);
    commandAttributes.requiresTarget(false);
    commandAttributes.mmcsConsoleCommand(true);
    commandAttributes.helpCategory(ADMIN);
    commandAttributes.internalAuth(true);
    return new MMCSConsoleCommand_refresh_config("refresh_config", "refresh_config [server [/path/to/properties]]", commandAttributes);
}

void
MMCSConsoleCommand_refresh_config::execute(
            std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* controller,
            BlockControllerTarget* target
            )
{
    std::string file;
    std::string server;
    if ( args.size() == 2 ) {
        file = args[1];
        server = args[0];
    } else if ( args.size() == 1 ) {
        server = args[0];
    }

    // update our PATH environment variable using the command_path from the
    // properties file so system(3) can find the appropriate refresh_config
    // command when it is executed
    const std::string old_path( getenv("PATH") ? getenv("PATH") : "" );
    std::string new_path( old_path );
    try {
        new_path += ":";
        new_path += MMCSProperties::getProperties()->getValue( "bg_console", "command_path" );
    } catch ( const std::exception& e ) {
        new_path = old_path;
    }
    setenv( "PATH", new_path.c_str(), 1 );

    // reset PATH to what it was when we return
    bgq::utility::ScopeGuard pathGuard(
            boost::bind(
                &setenv,
                "PATH",
                old_path.c_str(),
                1
                )
            );

    // refresh a specific server
    if ( !server.empty() ) {
        const ServerContainer::const_iterator i = std::find(
                _servers.begin(),
                _servers.end(),
                server
                );
        if ( i == _servers.end() ) {
            reply << FAIL << "unknown server: " << server << DONE;
            return;
        }

        this->refresh( server, file, reply, controller );
        return;
    }

    // refresh all servers
    BOOST_FOREACH( const std::string& server, _servers ) {
        this->refresh( server, file, reply, controller );
    }
}

void
MMCSConsoleCommand_refresh_config::refresh(
        const std::string& server,
        const std::string& file,
        MMCSCommandReply& reply,
        ConsoleController* controller
        )
{
    // special case for mmcs_server, this command is executed on the server which is why
    // we need the mmcs_server_cmd scaffolding
    if ( server == "mmcs_server" ) {
        MMCSCommandProcessor* const processor = controller->getCommandProcessor();
        processor->execute(
                boost::assign::list_of
                    ( std::string("mmcs_server_cmd") )
                    ( std::string("refresh_config") )
                    ( file ),
                reply,
                controller
                );

        return;
    }

    // Append _refresh_config to the server name and assume that the command name.
    // Also append whatever our properties filename is to this command, that way
    // we'll be talking to the right server
    const std::string command(
            server +
            "_refresh_config "
            " --properties " +
            MMCSProperties::getProperties()->getFilename() +
            " " +
            file
            );

    // reply success or failure is based off non-zero exit status of the command
    const int rc = system( command.c_str() );
    reply << ( rc ? FAIL : OK ) << DONE;
}

void
MMCSConsoleCommand_refresh_config::help(
        std::deque<std::string> args,
        MMCSCommandReply& reply
        )
{
    reply << OK << description()
        << ";Refresh the bg.properties file on all servers, or a specific server"
        << ";with the first positional argument. Specify an optional filename"
        << ";with the second positional argument to reload configuration from"
        << ";specific file for a specific server. Otherwise, configuration"
        << ";from the existing file is reloaded."
        << ";"
        << ";Possible servers:"
        << ";";
    BOOST_FOREACH( const std::string& i, _servers ) {
        reply << i << ";";
    }

    reply << DONE;
}
