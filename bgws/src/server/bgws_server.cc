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

/*! \page server BGWS HTTP Server

The server is installed to /bgsys/drivers/ppcfloor/bgws/sbin/bgws_server,
with a symlink in /bgsys/drivers/ppcfloor/sbin.

The BGWS server is typically started by BGMaster.
It should be configured to run as the %bgws system user so that it can run the pwauth executable.

The %bgws system user must have access to the Blue Gene administrative key file and diagnostics logs directory,
this is typically done by making it a member of the bgqadmin group.


\section serverSsl Transport layer security (SSL)

The BGWS server supports the normal Blue Gene SSL security.
Clients can connect with the Administrative certificate, the Command certificate, or no certificate.

If the client connects with the Administrative certificate,
it doesn't have to send a User ID.

If the client connects with the Command certificate,
it must send a User ID.

If the client connects with no certificate,
then it uses session authentication,
where it must create a session and use that session ID on subsequent requests.


The commands connect directly to the BGWS server and will present either the command or administrative certificate.

The Apache server connects directly to the BGWS server so that it can proxy requests from the Navigator and other external clients.
The Apache server connects with no certificate, so external clients use session authentication.


\section serverArguments Command-line Arguments

The arguments that the server supports are as follows:

- --properties: the bg.properties file name, uses the default if not specified.
- --listen-port: overrides the listen_ports from the bg.properties file, see the bg.properties configuration for information.
- --verbose: logging configuration, as described in bgq::utility::LoggingProgramOptions.

\section serverConfiguration Configuration

The BGWS server uses the following configuration options in the BG configuration file (/bgsys/local/etc/bg.properties by default):

In the [%bgws] section:

- machine_name : The name of the BG machine. UIs can use this to display the name the users know the machine as. The default is the hostname.
- measurement_system : The measurement system to pass on to clients (e.g., the Navigator). Valid values are "US" and "SI", by default no measurement system is passed on to clients.
- listen_ports: Ports to listen on, defaults to localhost:32071, see bgq::utility::ServerPortConfiguration for documentation.
- path_base : The BGWS server will handle requests under this path, must be a valid escaped URL path starting with /, defaults to /bg.
- thread_pool_size : The number of threads in a thread pool for handling requests. Must be a number greater than 0 or "auto". If auto then the server will pick a thread pool size based on number of hardware threads available on the machine. Defaults to auto.
- blocking_operations_thread_pool_size : The number of threads in a thread pool for handling blocking operations such as database queries. Must be a number greater than 0 or "auto". If auto then the server will pick a thread pool size based on number of hardware threads available on the machine. Defaults to auto.
- check_user_admin_exe : The program to run to check if the user has administrative authority, see \ref checkUserAdmin. If not set then all users will be regular users (not administrators).
- diagnostics_exe : The program to run to run diagnostics. Defaults to /bgsys/drivers/ppcfloor/diags/bin/rundiags.py.
- service_action_exe : The program to run for service actions. Defaults to /bgsys/drivers/ppcfloor/baremetal/bin/ServiceAction.
- user_authentication_exe : The program to call to authenticate users. Defaults to /usr/local/libexec/pwauth, see \ref serverPwauth.
- connection_pool_size: Maximum number of connections held in the BGWS server database connection pool, defaults to 10.
- teal_chalert_exe: Program to run for tlchalert.
- teal_rmalert_exe: Program to run for tlrmalert.

The server also uses the SSL and system security configuration from the [security] section.


\section serverPwauth pwauth

When a user requests a new session, the BGWS server executes an external program to authenticate users.
The program to run can be configured in the BG configuration file (bg.properties).
The program must work like pwauth, described in
http://code.google.com/p/pwauth/ .

The BGWS server will execute the pwauth program and write the supplied username and password to its stdin, each ended by newline.
If the program exits with a 0 exit status, authentication was successful, otherwise authentication failed.


\section checkUserAdmin

This program checks that a user is an administrator.
The BGWS server calls this program whenever a session is started.

It's installed in /bgsys/drivers/ppcfloor/bgws/libexec/checkUserAdmin.

It works by swapping to the user (setuid) and checking if they have authority to read the Blue Gene administrative certificate key file as configured in the bg.properties file.

Since it swaps to the user it must be owned by root and have the setuid bit set.
For security, the executable's group should be bgqadmin and authority 750 so that only admins (like the %bgws user that the BGWS server is running as) can run it.

*/


#include "BgwsServer.hpp"
#include "dbConnectionPool.hpp"
#include "DynamicConfiguration.hpp"
#include "UserInfo.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>
#include <utility/include/version.h>

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread/thread.hpp>

#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace bgws;

using boost::bind;
using boost::lexical_cast;

using boost::algorithm::join;

using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws" );


static void printHelp(
        const std::string& program_name,
        const boost::program_options::options_description& visible_options_desc
    )
{
    std::cout << "Usage: " << program_name << " [OPTIONS]\n"
                 "\n"
                 "The Blue Gene Web Services server.\n"
                 "\n"
              << visible_options_desc << "\n";
}


static void runThread(
        boost::asio::io_service& io_service
    )
{
    while ( true ) {
        try {
            io_service.run();
            break;
        } catch ( boost::exception& e ) {
            LOG_ERROR_MSG( "Exception escaped from event loop. Diagnostic Info:\n" << boost::diagnostic_information( e ) );
            sleep( 10 );
        } catch ( std::exception& e ) {
            LOG_ERROR_MSG( "Exception escaped from event loop. The exception is " << e.what() );
            sleep( 10 ); // wait a little while before calling run again to avoid a tight loop.
        }
    }
}


int main( int argc, char *argv[] )
{
    namespace po = boost::program_options;

    po::options_description visible_options_desc( "Options" );

    try {
        bgq::utility::Properties::ProgramOptions properties_program_options;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description all_options_desc( "Options" );

        properties_program_options.addTo( visible_options_desc );
        logging_program_options.addTo( visible_options_desc );

        static const string DEFAULT_SERVICE_NAME( "32071" );

        bgq::utility::ServerPortConfiguration server_port_configuration(
                DEFAULT_SERVICE_NAME,
                bgq::utility::ServerPortConfiguration::ConnectionType::Optional
            );

        server_port_configuration.addTo(
                visible_options_desc,
                all_options_desc
            );


        bool help(false);

        visible_options_desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;


        all_options_desc.add( visible_options_desc );

        po::variables_map vm;
        po::store( po::command_line_parser( argc, argv ).options( all_options_desc ).run(), vm );
        po::notify( vm );

        if ( help ) {
            printHelp( argv[0], visible_options_desc );
            return EXIT_SUCCESS;
        }


        bgq::utility::Properties::Ptr bg_properties_ptr( bgq::utility::Properties::create( properties_program_options.getFilename() ) );

        bgq::utility::initializeLogging(
                *bg_properties_ptr,
                logging_program_options
            );

        try {

            LOG_INFO_MSG_FORCED( "BGWS server[" << ::getpid() << "]: starting: " << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")" << " " << __DATE__ << " " << __TIME__ );
            LOG_INFO_MSG_FORCED( string() + "Startup parameters: " + join( vector<string>( argv, argv + argc ), " " ) );


            // If the JAVA_HOME environment variable isn't set, log a message and set it.
            const char *java_home_str(getenv( "JAVA_HOME" ));

            if ( ! java_home_str ) {
                LOG_WARN_MSG( "JAVA_HOME environment variable isn't set, setting it to a default value /usr/lib/jvm/jre" );
                setenv( "JAVA_HOME", "/usr/lib/jvm/jre", 0 /* do not overwrite, but it wasn't set anyways */ );
            }


            UserInfo::initialize();


            server_port_configuration.setProperties( bg_properties_ptr, DynamicConfiguration::PROPERTIES_SECTION_NAME );

            server_port_configuration.notifyComplete();

            bool calculate_threads(true);
            unsigned thread_pool_size(2);

            try {
                string thread_pool_size_str;

                static const string THREAD_POOL_SIZE_CONFIGURATION_OPTION_NAME("thread_pool_size");

                thread_pool_size_str = bg_properties_ptr->getValue( DynamicConfiguration::PROPERTIES_SECTION_NAME, THREAD_POOL_SIZE_CONFIGURATION_OPTION_NAME);
                LOG_INFO_MSG( "Thread pool size in config file is '" << thread_pool_size_str << "'." );

                static const string THREAD_POOL_SIZE_AUTO_VALUE( "auto" );

                if ( thread_pool_size_str == THREAD_POOL_SIZE_AUTO_VALUE ) {

                    calculate_threads = true;

                } else {
                    try {
                        if ( (! thread_pool_size_str.empty()) && thread_pool_size_str[0] == '-' ) {
                            BOOST_THROW_EXCEPTION( std::runtime_error( "the thread pool size must be > 0" ) ); // expect to get caught a few lines from here.
                        }

                        thread_pool_size = lexical_cast<unsigned>( thread_pool_size_str );

                        if ( thread_pool_size < 1 ) {
                            BOOST_THROW_EXCEPTION( std::runtime_error( "the thread pool size must be > 0" ) ); // expect to get caught a couple lines from here.
                        }

                        calculate_threads = false;

                    } catch ( std::exception& e ) {
                        LOG_WARN_MSG( string() +
                                "Invalid value for [" + DynamicConfiguration::PROPERTIES_SECTION_NAME + "]." + THREAD_POOL_SIZE_CONFIGURATION_OPTION_NAME + " in bg.properties file."
                                " Will use the default."
                                " The value is '" << thread_pool_size_str << "'."
                                " The exception message is " << e.what() << "."
                            );
                        calculate_threads = true;
                    }
                }

            } catch ( std::exception& e ) {
                LOG_DEBUG_MSG( "Failed to get thread pool size from properties. Will use the default. The exception message is '" << e.what() << "'." );
                calculate_threads = true;
            }

            if ( calculate_threads ) {
                unsigned hardware_threads = boost::thread::hardware_concurrency();

                if ( hardware_threads > 2 ) {
                    thread_pool_size = (hardware_threads / 2) + 1;
                } else {
                    thread_pool_size = 2;
                }

                LOG_INFO_MSG( "Calculated thread pool size is " << thread_pool_size );
            }


            dbConnectionPool::initialize( bg_properties_ptr );


            boost::asio::io_service io_service;


            bgq::utility::SignalHandler<SIGCHLD> sig_handler( io_service );

            BgwsServer bgws_server(
                    bg_properties_ptr,
                    server_port_configuration,
                    io_service,
                    sig_handler
                );

            bgws_server.start();

            boost::thread_group threads;

            for (  ; (threads.size() + 1) <= thread_pool_size ; ) {
                threads.create_thread( bind( &runThread, boost::ref( io_service ) ) );
            }

            runThread( io_service );

            LOG_INFO_MSG( "Main thread exited event loop, waiting for other threads to end." );

            threads.join_all();

            LOG_INFO_MSG( "Other threads have ended." );

        } catch ( std::exception& e ) {
            LOG_ERROR_MSG( "Caught exception: " << e.what() );
            exit( EXIT_FAILURE );
        } catch ( ... ) {
            LOG_ERROR_MSG( "Caught exception!" );
            exit( EXIT_FAILURE );
        }
    } catch ( po::error& poe ) {
        std::cout << "Error processing arguments: " << poe.what() << "\n\n";

        printHelp( argv[0], visible_options_desc );

        exit( EXIT_FAILURE );
    } catch ( std::exception& e ) {
        std::cout << "Caught exception: " << e.what() << "\n";
        exit( EXIT_FAILURE );
    } catch ( ... ) {
        std::cout << "Caught exception!\n";
        exit( EXIT_FAILURE );
    }
}
