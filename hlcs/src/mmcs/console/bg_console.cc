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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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
 * \file bg_console.cc
 * \brief main driver for bg_console.
 */


#include "CommandProcessor.h"
#include "ConnectionMonitor.h"
#include "Options.h"

#include "command/MmcsServerCmd.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>
#include <utility/include/version.h>

#include <boost/assign/list_of.hpp>

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

#include <editline/readline.h>

#include <cstring>

#include <signal.h>
#include <stdlib.h>


LOG_DECLARE_FILE( "mmcs.console" );


using namespace std;

using mmcs::MMCSCommandMap;
using mmcs::MMCSCommandProcessor;

using mmcs::common::AbstractCommand;
using mmcs::common::ConsoleController;
using mmcs::common::Properties;

using mmcs::console::ConnectionMonitor;
using mmcs::console::Options;

using mmcs::console::command::MmcsServerCmd;


void
signal_handler(const int signum)
{
    if (signum == SIGUSR1)
    {
        return;
    }
}

void
readHistoryFile()
{
    using_history();
    // use process name for unique history file
    std::string result = "." + Properties::getProperty(MMCS_PROCESS) + "_history";
    if ( getenv("HOME") ) {
        std::string home( getenv("HOME") );
        home.append( "/" );
        home.append( result );
        result = home;
    }

    LOG_TRACE_MSG( "using '" << result << "' for editline history" );
    const std::string file( result );
    if ( read_history( file.c_str() ) != 0 ) {
        if ( errno == ENOENT ) {
            // file does not exist, this is not a warning since it will not exists when first creating the
            // history file
        } else {
            LOG_WARN_MSG( "could not read history file '" << file << "' " << strerror(errno) );
        }
    }
}

int
main(int argc, char *argv[])
{
    try {
        // parse options
        Options options( argc, argv );

        std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
        ostringstream version;
        version << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
        version << " " << __DATE__ << " " << __TIME__;
        Properties::setProperty(MMCS_VERSION, version.str());
        Properties::setProperty(MMCS_PROCESS, basename);

        // create the list of mmcs_console commands
        AbstractCommand::Attributes attr;  attr.mmcsConsoleCommand(true); attr.externalCommand(true);
        AbstractCommand::Attributes mask;  mask.mmcsConsoleCommand(true); mask.externalCommand(true);
        MMCSCommandMap* mmcsCommands(
                MMCSCommandProcessor::createCommandMap(attr, mask, Properties::getExternalCmds())
                );

        // Create the mmcs_console command processor
        boost::scoped_ptr<mmcs::console::CommandProcessor> commandProcessor;
        commandProcessor.reset( new mmcs::console::CommandProcessor(mmcsCommands) );

        // ignore SIGPIPE
        struct sigaction sa;
        memset( &sa, 0, sizeof(sa) );
        sa.sa_handler = SIG_IGN;
        if ( sigaction( SIGPIPE, &sa, NULL ) != 0 ) {
            LOG_WARN_MSG( "could not ignore SIGPIPE" );
        }

        // install handler for SIGUSR1
        sa.sa_handler = &signal_handler;
        if ( sigaction( SIGUSR1, &sa, NULL ) != 0 ) {
            LOG_WARN_MSG( "could not install SIGUSR1 handler" );
        }


        // Create the ConsoleController
        boost::scoped_ptr<ConsoleController> pController;
        bgq::utility::UserId uid;
        pController.reset(new ConsoleController( commandProcessor.get(), uid ));
        pController->setPortConfiguration( options.getPortConfigurationPtr() );

        // connect to the mmcs server
        {
            LOG_INFO_MSG("connecting to mmcs_server");
            deque<string> mmcs_connect = MMCSCommandProcessor::parseCommand("mmcs_server_connect");
            mmcs_client::CommandReply reply(1, pController->getReplyFormat(), false);
            commandProcessor->execute(mmcs_connect, reply, pController.get());
            if (reply.getStatus() == 0) {
                LOG_INFO_MSG("connected to mmcs_server");
            } else {
                // we are running disconnected from the server
                LOG_ERROR_MSG(reply.str());
                LOG_ERROR_MSG("server down or still initializing. try again in a few minutes. if the problem persists, contact the system administrator. ");
            }
        }

        ConnectionMonitor connmon;
        connmon.setConsole(pController.get());

        if(Properties::getProperty(AUTO_RECONNECT) == "true") {
            LOG_TRACE_MSG("Starting server connection monitor");
            connmon.start();
        }

        // drop privileges after SSL private key and certificate have been read
        // so the editline history file can be read and written
        if ( getuid() != geteuid() ) {
            LOG_TRACE_MSG( "euid : " << geteuid() );
            LOG_TRACE_MSG( "uid  : " << getuid() );
            if ( seteuid(getuid()) ) {
                LOG_WARN_MSG( "could not seteuid(" << getuid() << ") " << strerror(errno) );
            } else {
                LOG_TRACE_MSG( "dropped euid to " << geteuid() );
            }
        } else {
            LOG_TRACE_MSG( "euid and uid match" );
        }

        readHistoryFile();

        // loop reading and processing commands
        pController->run();

        // cleanup and exit
        cerr  << "terminating, please wait..." << endl;

        MmcsServerCmd::ending(true);
        connmon.stop(SIGUSR1);
        _exit( EXIT_SUCCESS );
    } catch ( const exception& e ) {
        cerr << e.what() << endl;
        exit( EXIT_FAILURE );
    }
}
