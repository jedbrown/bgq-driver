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

#include "common/common.h"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include <utility/include/portConfiguration/Connector.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <stdint.h>


using bgq::utility::ClientPortConfiguration;
using bgq::utility::Connector;
using bgq::utility::Properties;
using bgq::utility::portConfig::SocketPtr;

using std::cerr;
using std::cout;
using std::istream;
using std::istringstream;
using std::string;


static SocketPtr connect( Connector& connector )
{
    try {
        return connector.connect();
    } catch ( std::exception& e ) {
        throw std::runtime_error( string("failed to connect to the real-time server: ") + e.what() );
    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {
        ClientPortConfiguration port_config(
                realtime::common::DefaultCommandServiceName
            );

        po::options_description options( "Options" );

        port_config.addTo( options );

        Properties::ProgramOptions properties_program_options;

        properties_program_options.addTo( options );

        const string default_logger( "ibm.realtime" );
        bgq::utility::LoggingProgramOptions logging_program_options( default_logger );

        logging_program_options.addTo( options );

        options.add_options()
                ( "help,h", "print help text" )
            ;


        po::variables_map vm;

        try {
            po::store( po::parse_command_line( argc, argv, options ), vm );

            po::notify( vm );

            if ( vm.count( "help" ) ) {
                cout << "Usage: " << argv[0] << " [OPTION]...\n"
                        "Real-time server status utility.\n"
                        "\n"
                     << options << "\n";
                exit( 0 );
            }
        } catch ( std::exception& e ) {
            cerr << argv[0] << ": failed parsing arguments, " << e.what() << ".\n"
                    "Try `" << argv[0] << " --help' for more information.\n";
            exit( 1 );
        }

        const string PROPERTIES_SECTION_NAME( "realtime.server.command" );

        Properties::Ptr bg_properties_ptr(Properties::create( properties_program_options.getFilename() ));

        bgq::utility::initializeLogging(
                *bg_properties_ptr,
                logging_program_options
            );

        port_config.setProperties(
                bg_properties_ptr,
                PROPERTIES_SECTION_NAME
            );

        port_config.notifyComplete();


        boost::asio::io_service io_service;

        Connector connector(
                io_service,
                port_config
            );

        SocketPtr socket_ptr(
                connect( connector )
            );

        string msg( realtime::common::StatusCommandName + "\n" );

        boost::asio::write(
                *socket_ptr,
                boost::asio::buffer( msg )
            );

        boost::asio::streambuf in_buf;

        boost::asio::read_until(
                *socket_ptr,
                in_buf,
                '\n'
            );

        istream is( &in_buf );

        string response_str;

        std::getline( is, response_str );

        istringstream iss( response_str );

        string status;

        iss >> status;

        uint32_t connected_clients;

        iss >> connected_clients;

        string db_monitor_state_str;

        iss >> db_monitor_state_str;

        string db2_version;
        
        iss >> db2_version;


        cout << "Server status: " << status << "\n"
                "Connected clients: " << connected_clients << "\n"
                "DB monitor state: " << db_monitor_state_str << "\n"
                "Compiled against DB2 " << db2_version << "\n";

        exit( 0 );
    } catch ( std::exception& e ) {
        cerr << argv[0] << ": error, " << e.what() << "\n";

        exit( 1 );
    }
}


/*! \page statusUtility Status utility
 *

This command will typically be run from the bg_console.

It provides the status of the RTS.

It's installed to <b>&lt;ppcfloor&gt;/hlcs/sbin/realtime_server_status</b>.

The output is like this:

<pre>
Server status: <i>running</i>
Connected clients: <i>number</i>
DB monitor state: <i>idle|monitoring|maxXact</i>
</pre>


 */
