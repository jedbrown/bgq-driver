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
#include "utility.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/format.hpp>
#include <boost/throw_exception.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include <stdint.h>


using bgws::command::BgwsClient;

using std::cout;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.command" );


static void bgwsServerStatus(
        BgwsClient& bgws_client,
        bool details
    )
{
    json::ObjectValuePtr value_ptr(bgws_client.getBgwsServerStatus());

    const json::Object &obj(value_ptr->get());

    const string &status_str(obj.getString( "status" ));

    if ( status_str == "OK" ) {
        cout << "BGWS server is running and available.\n";
    } else {
        cout << "BGWS server is not available.\n";
    }

    if ( details ) {

        const json::Object &database_connection_pool_obj(obj.getObject( "databaseConnectionPool" ));

        cout << "Server started: " << bgws::command::utility::formatTimestamp( obj.getString( "startTime" ) ) << "\n"
                "Requests in progress: " << obj.as<uint64_t>( "requestsInProgress" ) << "\n"
                "Requests handled: " << obj.as<uint64_t>( "requestsHandled" ) << "\n"
                "Max request time: " << obj.as<double>( "maxRequestTime" ) << " seconds\n"
                "Avg request time: " << obj.as<double>( "avgRequestTime" ) << " seconds\n"
                "\n"
                "Database connection pool:\n"
                "  Available: " << database_connection_pool_obj.as<unsigned>( "available" ) << "\n"
                "  Used: " << database_connection_pool_obj.as<unsigned>( "used" ) << "\n"
                "  Max: " << database_connection_pool_obj.as<unsigned>( "max" ) << "\n"
                "  Size: " << database_connection_pool_obj.as<unsigned>( "size" ) << "\n"
                "\n"
            ;

        if ( obj.contains( "requests" ) ) {

            const json::Array &requests(obj.getArray( "requests" ));

            const unsigned time_field_width(26);
            unsigned user_field_width(4);
            unsigned operation_field_width(9);

            for ( json::Array::const_iterator i(requests.begin()) ; i != requests.end() ; ++i ) {
                const json::Object &req_obj((*i)->getObject());
                unsigned user_len(req_obj.getString( "user" ).size());
                user_field_width = std::max( user_field_width, user_len );

                unsigned operation_len(req_obj.getString( "method" ).size() + 1 + req_obj.getString( "url" ).size());
                operation_field_width = std::max( operation_field_width, operation_len );
            }


            cout << "Current Requests:\n";

            const string format_str((boost::format( "%%-%ds  %%-%ds  %%-%ds" ) % time_field_width % user_field_width % operation_field_width).str());

            cout << boost::format( format_str ) % "Start Time" % "User" % "Operation" << "\n";

            for ( json::Array::const_iterator i(requests.begin()) ; i != requests.end() ; ++i ) {

                const json::Object &req_obj((*i)->getObject());

                cout << boost::format( format_str )
                    % bgws::command::utility::formatTimestamp( req_obj.getString( "startTime" ) )
                    % req_obj.getString( "user" )
                    % (req_obj.getString( "method" ) + " " + req_obj.getString( "url" ))
                    << "\n";

            }

        } else {

            cout << "No current requests\n";

        }

        cout << "\n";
    }
}


int main( int argc, char *argv[] )
{
    namespace po = boost::program_options;


    try {

        bool details(false);
        bool help(false);

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description desc( "Options" );

        desc.add_options()
                ( "details,d", po::bool_switch( &details ), "Print server status details" )
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;

        logging_program_options.addTo( desc );
        desc.add( bgws_client.getDesc() );

        po::variables_map vm;

        try {
            po::store( po::command_line_parser( argc, argv ).options( desc ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS]\n"
                     << "\n"
                        "Displays the BGWS server status.\n"
                        "\n"
                        "Requires administrative authority.\n"
                        "\n"
                     << desc << "\n";
                return EXIT_SUCCESS;
            }
        } catch ( std::exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }


        bgws_client.notifyOptionsSet();

        bgq::utility::initializeLogging(
                *bgws_client.getBgProperties(),
                logging_program_options
            );

        bgws_client.notifyLoggingIsInitialized();


        bgwsServerStatus(
                bgws_client,
                details
            );

        return EXIT_SUCCESS;

    } catch ( std::exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
