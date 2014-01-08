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

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>


using bgws::command::BgwsClient;

using boost::regex;
using boost::regex_match;
using boost::smatch;

using std::cout;
using std::exception;
using std::map;
using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws.command" );


typedef map<string,string> LoggingSettings;


static void printLoggingConfiguration(
        BgwsClient& bgws_client
    )
{
    json::ObjectValuePtr value_ptr(bgws_client.getLogging());
    const json::Object &logging_obj(value_ptr->get());

    if ( logging_obj.empty() ) {
        cout << "No loggers configured";
    } else {
        for ( json::Object::const_iterator i(logging_obj.begin()) ; i != logging_obj.end() ; ++i ) {
            cout << i->first << "=" << i->second->getString() << "\n";
        }
    }
}


static void bgws_server_logging(
        const BgwsClient::LoggingSettings& logging_settings,
        BgwsClient& bgws_client
    )
{
    if ( ! logging_settings.empty() ) {
        bgws_client.setLogging( logging_settings );
    }
    printLoggingConfiguration(
            bgws_client
        );
}


int main( int argc, char *argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);
        typedef vector<string> LoggingSettingsStrings;

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        LoggingSettingsStrings logging_setting_strs;

        BgwsClient::LoggingSettings logging_settings;


        po::options_description visible_desc( "Options" );
        visible_desc.add( bgws_client.getDesc() );
        visible_desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( visible_desc );

        po::options_description all_opts_desc;
        all_opts_desc.add( visible_desc );
        all_opts_desc.add_options()
                ( "logging_setting", po::value( &logging_setting_strs ), "New logging settings" )
            ;

        po::positional_options_description p;
        p.add( "logging_setting", -1 );

        po::variables_map vm;

        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS] [LOGGER=LEVEL]...\n"
                     << "\n"
                        "Prints the logging configuration for the Blue Gene Web Services, optionally sets the logging configuration.\n"
                        "\n"
                        "Requires administrative authority.\n"
                        "\n"
                     << visible_desc << "\n";
                return 0;
            }

            for ( LoggingSettingsStrings::const_iterator i(logging_setting_strs.begin()) ; i != logging_setting_strs.end() ; ++i ) {
                static const regex re( "([^=]+)=(.*)" );

                smatch match;

                if ( ! regex_match( *i, match, re ) ) {
                    BOOST_THROW_EXCEPTION( std::invalid_argument(
                            string() + "logging setting '" + *i + "' was not in a valid format" )
                        );
                }

                logging_settings[match[1]] = match[2];
            }
        } catch ( exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }


        bgws_client.notifyOptionsSet();

        bgq::utility::initializeLogging(
                *bgws_client.getBgProperties(),
                logging_program_options
            );

        bgws_client.notifyLoggingIsInitialized();


        bgws_server_logging(
                logging_settings,
                bgws_client
            );


    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;
    }
}
