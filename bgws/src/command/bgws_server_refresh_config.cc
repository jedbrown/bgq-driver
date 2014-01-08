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

#include "BgwsClient.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <stdexcept>
#include <string>


using bgws::command::BgwsClient;

using std::cerr;
using std::cout;
using std::exception;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.command" );


static void bgws_server_refresh_config(
        BgwsClient& bgws_client,
        const boost::optional< std::string >& filename_opt
    )
{
    try {

        ::bgws::common::RefreshBgwsServerConfiguration refresh_bgws_server_configuration(
                filename_opt && *filename_opt == std::string() ?  ::bgws::common::RefreshBgwsServerConfiguration::ReadDefault :
                filename_opt ? ::bgws::common::RefreshBgwsServerConfiguration::ReadNew( *filename_opt ) :
                ::bgws::common::RefreshBgwsServerConfiguration::RereadCurrent
            );

        bgws_client.refreshBgwsServerConfiguration( refresh_bgws_server_configuration );

    } catch ( ::bgws::common::RefreshBgwsServerConfiguration::PathNotComplete& e ) {

        BOOST_THROW_EXCEPTION( std::runtime_error( "the supplied filename must be a fully-qualified path" ) );

    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );


        po::options_description visible_desc( "Options" );
        visible_desc.add( bgws_client.getDesc() );
        visible_desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( visible_desc );

        po::options_description all_opts_desc;
        all_opts_desc.add( visible_desc );
        all_opts_desc.add_options()
                ( "filename", po::value<string>() )
            ;

        po::positional_options_description p;
        p.add( "filename", -1 );

        po::variables_map vm;


        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {

                cout <<
"Usage: " << argv[0] << " [OPTIONS] [FILENAME]\n"
"\n"
"Refresh the Blue Gene Web Services (BGWS) server configuration.\n"
"\n"
"Requires administrative authority.\n"
"\n"
"FILENAME is the new properties file name to use. By default the current properties file will be re-read.\n"
"\n"
"Only certain configuration options can be changed using this command, consult the documentation in the bg.properties template file.\n"
"\n"
"The caller must be a Blue Gene administrator.\n"
"\n"
<< visible_desc << "\n";

                return 0;
            }

        } catch ( exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }

        boost::optional<string> filename_opt;

        if ( vm.count( "filename" ) ) {
            filename_opt = vm["filename"].as<string>();
        }

        bgws_client.notifyOptionsSet();

        bgq::utility::initializeLogging(
                *bgws_client.getBgProperties(),
                logging_program_options
            );

        bgws_client.notifyLoggingIsInitialized();


        bgws_server_refresh_config(
                bgws_client,
                filename_opt
            );

        return 0;
    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;
    }
}
