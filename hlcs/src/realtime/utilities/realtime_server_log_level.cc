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
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <stdint.h>


using bgq::utility::ClientPortConfiguration;
using bgq::utility::Connector;
using bgq::utility::LoggingProgramOptions;
using bgq::utility::Properties;
using bgq::utility::portConfig::SocketPtr;

using std::cerr;
using std::cout;
using std::istream;
using std::istringstream;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;


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
                realtime::common::DefaultCommandServiceName,
                ClientPortConfiguration::ConnectionType::Administrative
            );

        po::options_description all_options( "" );

        LoggingProgramOptions::Strings logging_strings;

        all_options.add_options()
            ( "logging_pos", po::value( &logging_strings ), "new logging settings" )
            ;


        po::options_description visible_options( "Options" );

        port_config.addTo( visible_options );

        Properties::ProgramOptions properties_program_options;


        properties_program_options.addTo( visible_options );

        const string default_logger( "ibm.realtime" );
        LoggingProgramOptions logging_program_options( default_logger );

        logging_program_options.addTo( visible_options );

        visible_options.add_options()
                ( "help,h", "print help text" )
            ;

        all_options.add( visible_options );

        po::positional_options_description p;
        p.add( "logging_pos", -1 );


        po::variables_map vm;

        try {
            po::store(
                    po::command_line_parser( argc, argv ).options( all_options ).positional( p ).run(),
                    vm
                );

            po::notify( vm );

            if ( vm.count( "help" ) ) {
                cout << "Usage: " << argv[0] << " [OPTION]... [LOG_SETTING...]\n"
                        "Real-time server logging utility.\n"
                        "\n"
                     << visible_options << "\n";
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


        // Validate the logging options.

        LoggingProgramOptions verify_logging_program_options(
                "ibm.realtime.server"
            );

        verify_logging_program_options.notifier(
                logging_strings
            );


        boost::asio::io_service io_service;

        Connector connector(
                io_service,
                port_config
            );

        SocketPtr socket_ptr(
                connect( connector )
            );

        ostringstream oss;
        oss << realtime::common::LoggingCommandName;

        BOOST_FOREACH( const string& s, logging_strings ) {
            oss << ' ' << s;
        }

        string msg( oss.str() + "\n" );

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

        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( " " );
        tokenizer tokens( response_str, sep );

        if ( tokens.begin() == tokens.end() ) {
            throw runtime_error( "no response from server" );
        }

        string msg_name(*tokens.begin());

        vector<string> args( ++tokens.begin(), tokens.end() );

        // expect either success or failed.

        if ( msg_name == "success" ) {
            exit( 0 );
        }

        if ( msg_name == "failed" ) {
            cerr << "error,";

            BOOST_FOREACH( const string& arg, args ) {
                cerr << ' ' << arg;
            }

            cerr << "\n";

            exit( 1 );
        }

        throw runtime_error( string() + "unexpected response from server, '" + msg_name + "'" );

    } catch ( std::exception& e ) {
        cerr << argv[0] << ": error, " << e.what() << "\n";

        exit( 1 );
    }
}


/*! \page loggingUtility Logging utility
 *

This command will typically be run from the bg_console.

It sets the logging configuration of the RTS dynamically.

It's installed to <b>&lt;ppcfloor&gt;/hlcs/sbin/realtime_server_logging</b>.

This program can typically only be run by the Blue Gene administrator.

If successful, doesn't print anything and exits with exit status 0.

Otherwise, prints output to stderr and exits with non-zero exit status.

 */
