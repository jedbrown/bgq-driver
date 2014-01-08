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

/*! \mainpage
 *

The Blue Gene real-time server
monitors the database transaction logs
and sends events to any connected clients.

Throughout this doc, I'll use the following abbreviations:
- real-time server = RTS
- database = DB
- transaction = xact

The RTS binary is <b>&lt;ppcfloor&gt;/hlcs/sbin/bg_realtime_server</b> .

The RTS must have read access
to the files that store the DB xact logs.
This is typically handled by having the bg_realtime_server
file owned by bgqsysdb and setuid.
Setting the authority correctly is handled by the RPMs.

Once the server has started,
it accepts connections from clients.
The client is typically a scheduler.
A sample client is available as part of the
scheduler APIs, in &lt;ppcfloor&gt;/hlcs/bin/realtime_client .

The server also processes requests from the utilities, which are typically run from bg_console:
- \subpage statusUtility
- \subpage loggingUtility

The RTS consists of the following components:
- \ref clients -- Accepts connections, handles messages from clients and sends messages to clients.
- \subpage dbm

Before running the real-time server, the database must be configured as described in \ref databaseSetup.

The server is configured as described in \ref serverConfiguration.

If you have a problem, check out \ref problems.


\section daemon Running as a daemon

The real-time server is typically started by BGMaster, but
can also be configured to be started as a daemon started by init.
The real-time server is typically configured to be started by init when using failover.
An init script is provided here: /bgsys/drivers/ppcfloor/hlcs/etc/init/realtime_server.
To run the real-time server as a daemon, perform the following steps:

1. Get the init script into /etc/init.d:

<pre>
 # ln -s /bgsys/drivers/ppcfloor/hlcs/etc/init/realtime_server /etc/init.d/
 # chkconfig --add realtime_server
</pre>

2. Create a configuration file, /etc/sysconfig/bg_realtime_server:

<pre>
 DAEMON_ARGS="--user=bgqsysdb"
 REALTIME_SERVER_ARGS="--log=/bgsys/logs/BGQ/$HOSTNAME-realtime_server.log"
</pre>

By default, the real-time server isn't started automatically at any runlevel.
You can use chkconfig to override the defaults.


\section clients Client handling

There are two types of clients:

- \subpage realtimeClients
- \subpage commandClient


\section databaseSetup Database Setup

The database must be set up with archive logging. To do this,
after creating the database and before running createBGQSchema:

1. Switch user to bgqsysdb and update the database configuration with the
following commands:

<pre>
   su - bgqsysdb
   mkdir -p /dbhome/bgqsysdb/dblogs/archive/bgdb0
   db2 "update database configuration for bgdb0 using logarchmeth1
     DISK:/dbhome/bgqsysdb/dblogs/archive/bgdb0"
</pre>

NOTE: The preceeding db2 "update..." command is a single command.

2. Next stop and restart DB2 so the changes will take effect:

<pre>
    db2 force application all
    db2stop
    db2start
</pre>

3. After the database restarts you must make a backup, use the command:

<pre>
   db2 "BACKUP DATABASE BGDB0"
</pre>

4. Create a cron job (logged in as root) to remove old archive log files.
Create a file /etc/cron.daily/bluegene-db-log-cleaner.sh with the following
contents:

<pre>
#!/bin/bash
find /dbhome/bgqsysdb/dblogs/archive/bgdb0/ -type f -mtime +1 -exec rm {} \;

Make sure the new file is executable by running the following command:

    chmod +x /etc/cron.daily/bluegene-db-log-cleaner.sh
</pre>

\section serverConfiguration Configuration

RTS takes configuration options from the bg.properties file and the command line.

\subsection properties bg.properties

RTS reads configuration options from the bg.properties file.

RTS reads configuration specific to the real-time server
from the <b>[%realtime.server]</b> section:

- listen_ports : The ports that the RTS listens for real-time connections on, defaults to 'localhost:32061'.
- command_listen_ports : The ports that the RTS listens for command connections on, defaults to 'localhost:32062'.
- workers : The number of worker threads to start. The server will pick a value for itself if 0 is given.
- maximum_transaction_size : The number of operations in a transaction before the DBM enters large-transaction mode and disconnects clients.

RTS reads database configuration options from the <b>[database]</b> section:

- name : database name
- user : database user name
- schema_name : schema to use


\subsection commandline Command Line Options

RTS accepts the following
standard Blue Gene program options:
- --listen-port
- --command-listen-port
- --properties
- --verbose
- --help,-h
- --log <i>log-file-name</i>

If the --log option is given, the server daemonizes itself, sending its log output to the given file.


\section problems Common problems

\subsection logsaccess Can't access the database transaction logs

The real-time server must have access
to the database transaction logs directory
in order to work.
For security reasons,
the database transaction logs cannot be read by just anybody.
For this reason,
BGMaster should be configured to start the real-time server as bgqsysdb.
To fix it,
change the BGMaster configuration to start the real-time server as bgqsysdb,
e.g., in the bg.properties file :

<pre>
[master.user]
realtime_server = bgqsysdb
</pre>

 *
 */

#include "BgRealtimeServer.h"

#include "common/common.h"

#include "utility/include/Log.h"
#include "utility/include/LoggingProgramOptions.h"
#include "utility/include/Properties.h"
#include <utility/include/version.h>

#include "utility/include/portConfiguration/ServerPortConfiguration.h"

#include <unistd.h>

#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>


using bgq::utility::Properties;
using bgq::utility::ServerPortConfiguration;

using boost::algorithm::join;

using realtime::server::BgRealtimeServer;

using std::cout;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "realtime.server" );


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {
        const string DEFAULT_SERVICE_NAME( "32061" );
        const string DEFAULT_COMMAND_SERVICE_NAME( "32062" );

        ServerPortConfiguration port_config(
                DEFAULT_SERVICE_NAME,
                ServerPortConfiguration::ConnectionType::AdministrativeOnly
            );

        ServerPortConfiguration command_port_config(
                realtime::common::DefaultCommandServiceName,
                "command", "command"
            );

        po::options_description all_options( "" );
        po::options_description visible_options( "Options" );

        port_config.addTo( visible_options, all_options );
        command_port_config.addTo( visible_options, all_options );

        Properties::ProgramOptions properties_program_options;

        properties_program_options.addTo( visible_options );

        const string default_logger( "ibm.realtime" );
        bgq::utility::LoggingProgramOptions logging_program_options( default_logger );

        logging_program_options.addTo( visible_options );

        string log_file_name;

        visible_options.add_options()
                ( "log", po::value( &log_file_name ), "daemonize and log file name" )
                ( "help,h", "print help text" )
            ;

        all_options.add( visible_options );

        po::variables_map vm;

        try {
            po::store( po::parse_command_line( argc, argv, all_options ), vm );

            po::notify( vm );

            if ( vm.count( "help" ) ) {
                cout << "Usage: " << argv[0] << " [OPTION]...\n"
                        "Real-time server for Blue Gene/Q.\n"
                        "\n"
                     << visible_options << "\n";
                return 0;
            }
        } catch ( std::exception& e ) {
            cout << argv[0] << ": failed parsing arguments, " << e.what() << ".\n"
                    "Try `" << argv[0] << " --help' for more information.\n";
            return 1;
        }

        if ( ! log_file_name.empty() ) {
            int log_fd(open( log_file_name.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR |  S_IWUSR | S_IRGRP | S_IWGRP ));
            if ( log_fd == -1 ) {
                int open_errno(errno);
                std::cerr << "Failed to open log file " << log_file_name << " error is " << strerror( open_errno ) << "\n";
                exit( 1 );
            }

            int nochdir(0);
            int noclose(0);
            int rc = daemon( nochdir, noclose );

            if ( rc == -1 ) {
                int daemon_errno(errno);
                std::cerr << "Failed to daemonize. errno=" << strerror( daemon_errno ) << "\n";
                exit( 1 );
            }

            if ( log_fd != STDOUT_FILENO ) {
                if ( dup2( log_fd, STDOUT_FILENO ) < 0 ) {
                    exit( 1 );
                }
            }
            if ( log_fd != STDERR_FILENO ) {
                if ( dup2( log_fd, STDERR_FILENO ) < 0 ) {
                    exit( 1 );
                }
            }
            if ( log_fd != STDOUT_FILENO && log_fd != STDERR_FILENO ) {
                close( log_fd );
            }
        }

        Properties::Ptr properties_ptr( Properties::create( properties_program_options.getFilename() ) );

        bgq::utility::initializeLogging(
                *properties_ptr,
                logging_program_options
            );

        LOG_INFO_MSG_FORCED( "Real-time server[" << ::getpid() << "]: starting: " << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")" << " " << __DATE__ << " " << __TIME__ );
        LOG_INFO_MSG_FORCED( "built against DB2 " << DB2VERSION );
        LOG_INFO_MSG_FORCED( string() + "Startup parameters: " + join( vector<string>( argv, argv + argc ), " " ) );

        BgRealtimeServer bg_realtime_server( argv[0] );
        bg_realtime_server.setConfigProperties( properties_ptr );
        bg_realtime_server.run(
                port_config,
                command_port_config
            );

    } catch ( std::exception& e ) {
        cout << argv[0] << ": failed, " << e.what() << "\n";
        exit( 1 );
    } catch ( ... ) {
        cout << argv[0] << ": caught unexpected exception." << "\n";
        exit( 1 );
    }
} // main()
