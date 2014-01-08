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

#include "bgws.hpp"
#include "BgwsOptions.hpp"
#include "Response.hpp"
#include "utility.hpp"

#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Query.hpp"
#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>
#include <boost/throw_exception.hpp>

#include <curl/curl.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <cstdlib>
#include <stdio.h>


using namespace bgws_clients;

using namespace boost::assign;

using std::cerr;
using std::cout;
using std::exception;
using std::ostream;
using std::ostringstream;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.clients" );


string blockStatusCodeToText( const std::string & s )
{
    if ( s == "F" )  return "Free";
    if ( s == "A" )  return "Allocated";
    if ( s == "B" )  return "Booting";
    if ( s == "I" )  return "Initializing";
    if ( s == "T" )  return "Terminating";
    return string() + "Unknown(" + s + ")";
}


static json::ValuePtr retrieveBlocksJsonValue(
        const string& block_id,
        const string& status,
        const BgwsOptions& bgws_options
    )
{
    CURLcode crc;

    capena::http::uri::Path path(bgws::BLOCKS_URL_PATH);

    if ( ! block_id.empty() ) {
        path /= block_id;
    }

    string url(bgws_options.getBase() + path.toString());

    if ( ! status.empty() ) {
        capena::http::uri::Query::Parameters params;
        params += capena::http::uri::Query::Parameter( "status", status );
        url += capena::http::uri::Query( params ).calcString();
    }

    LOG_DEBUG_MSG( "GET from '" << url << "'" );

    CURL *chandle(curl_easy_init());

    if ( chandle == NULL ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to initialize the curl handle" ) );
    }

    BOOST_SCOPE_EXIT( (&chandle) ) { curl_easy_cleanup( chandle ); } BOOST_SCOPE_EXIT_END

    char error_buffer[CURL_ERROR_SIZE] = { 0 };

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_ERRORBUFFER, error_buffer )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the ERRORBUFFER option on the curl handle, " + curl_easy_strerror( crc ) ) );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the URL option on the curl handle, " + error_buffer ) );
    }

    utility::setSslOptions(
            bgws_options,
            chandle,
            error_buffer
        );


    string session_id;
    utility::calcSessionId(
            bgws_options,
            session_id
        );

    utility::HeaderGuard hg(utility::setHeaders( chandle, error_buffer, session_id ));

    Response response( chandle, error_buffer );

    if ( curl_easy_perform( chandle ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "curl operation failed, " + error_buffer ) );
    }

    // response should now hold the server's response

    // Check to make sure that the response looks OK, throws if it's not.
    bgws::checkResponse( response, capena::http::Status::OK );

    json::ValuePtr value_ptr(json::Parser()( response.getData() ));
    return value_ptr;
}


static json::Array retrieveBlocksJsonArray(
        const string& status,
        const BgwsOptions& bgws_options
    )
{
    json::ValuePtr value_ptr(retrieveBlocksJsonValue( string(), status, bgws_options ));
    json::Array arr(value_ptr->getArray());
    return arr;
}


static json::Object retrieveBlockJsonObject(
        const string& block_id,
        const BgwsOptions& bgws_options
    )
{
    json::ValuePtr value_ptr(retrieveBlocksJsonValue( block_id, string(), bgws_options ));
    json::Object obj(value_ptr->getObject());
    return obj;
}


static void printBlockSummary(
        const json::Array& blocks,
        ostream& os
    )
{
    if ( blocks.empty() ) {
        os << "No blocks.\n";
        return;
    }

    BOOST_FOREACH( const json::Array::value_type& block_val_ptr, blocks ) {
        const json::Object &block_obj( block_val_ptr->getObject() );

        os << boost::format( "%1%\n" ) % block_obj.getString( "id" )
           << boost::format( "  %1%: %2%\n" ) % "Status" % block_obj.getString( "status" );

        if ( block_obj.contains( "user" ) ) {
            os << boost::format( "  %1%: %2%\n" ) % "User" % block_obj.getString( "user" );
        }

        os << "\n";
    }
}


static void printBlockDetails(
        const json::Object& block,
        ostream& os
    )
{
    boost::format NQ_LINE_FMT( "%1%: %2%\n" );
    boost::format Q_LINE_FMT( "%1%: '%2%'\n" );

    os <<
str( NQ_LINE_FMT % "Status" % blockStatusCodeToText( block.getString( "status" ) ) );

if ( block.contains( "user" ) ) {
    os <<
str( NQ_LINE_FMT % "User" % block.getString( "user" ) );
}

    if ( block.contains( "computeNodeCount" ) ) {
        os <<
str( NQ_LINE_FMT % "Compute nodes" % block.getDouble( "computeNodeCount" ) ) <<
str( NQ_LINE_FMT % "Shape" % block.getString( "shape" ) ) <<
str( NQ_LINE_FMT % "Job count" % block.getDouble( "jobCount" ) );
    }
    if ( block.contains( "ioNodeCount" ) ) {
        os <<
str( NQ_LINE_FMT % "I/O nodes" % block.getDouble( "ioNodeCount" ) );
    }

    os <<
str( Q_LINE_FMT % "Description" % block.getString( "description" ) );

    if ( block.contains( "bootOptions" ) )  os << str( Q_LINE_FMT % "Boot options" % block.getString( "bootOptions" ) );
    if ( block.contains( "microloaderImage" ) )  os << str( Q_LINE_FMT % "Microloader image" % block.getString( "microloaderImage" ) );

    os <<
str( Q_LINE_FMT % "Node configuration" % block.getString( "nodeConfiguration" ) );

    if ( block.contains( "options" ) ) os << str( Q_LINE_FMT % "Options" % block.getString( "options" ) );

    if ( block.contains( "torus" ) ) {
        string torus_dimensions_str(block.getString( "torus" ));
        if ( torus_dimensions_str.empty() )  torus_dimensions_str = "none";

        os << str( NQ_LINE_FMT % "Torus dimensions" % torus_dimensions_str );
    }

    if ( block.contains( "midplanes" ) ) {

        os << boost::format( "%1%:\n" ) % "Midplanes";

        BOOST_FOREACH( const json::Array::value_type& mp_val_ptr, block.getArray( "midplanes" ) ) {
            os << boost::format( "\t%1%\n" ) % mp_val_ptr->getString();
        }

        if ( block.contains( "passthrough" ) ) {

            os << boost::format( "%1%:\n" ) % "Passthrough midplanes";

            BOOST_FOREACH( const json::Array::value_type& mp_val_ptr, block.getArray( "passthrough" ) ) {
                os << boost::format( "\t%1%\n" ) % mp_val_ptr->getString();
            }

        }

    } else if ( block.contains( "nodeBoards" ) ) {

        os << boost::format( "%1%:\n" ) % "Node boards";

        BOOST_FOREACH( const json::Array::value_type& nb_val_ptr, block.getArray( "nodeBoards" ) ) {
            os << boost::format( "\t%1%-%2%\n" ) % block.getString( "midplane" ) % nb_val_ptr->getString();
        }

    } else if ( block.contains( "locations" ) ) {

        os << boost::format( "%1%:\n" ) % "Locations";

        BOOST_FOREACH( const json::Array::value_type& loc_val_ptr, block.getArray( "locations" ) ) {
            os << boost::format( "\t%1%\n" ) % loc_val_ptr->getString();
        }

    }

    os << "\n";
}


static void listBlocks(
        const string& block_id,
        const string& status,
        const BgwsOptions& bgws_options
    )
{
    if ( block_id.empty() ) {

        json::Array arr(retrieveBlocksJsonArray(
                status,
                bgws_options
            ));

        printBlockSummary( arr, cout );

    } else {

        json::Object obj(retrieveBlockJsonObject(
                block_id,
                bgws_options
            ));

        printBlockDetails( obj, cout );

    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);
        string block_id;
        string status;

        BgwsOptions bgws_options;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description all_opts_desc;

        all_opts_desc.add_options()
                ( "id", po::value<string>( &block_id ), "Block ID for block details" )
            ;

        po::options_description filter_desc( "Filter options" );
        filter_desc.add_options()
                ( "status", po::value( &status ), "Block status" )
            ;

        po::options_description desc( "Options" );

        desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( desc );

        desc.add( filter_desc );
        desc.add( bgws_options.getDesc() );

        all_opts_desc.add( desc );

        po::positional_options_description p;
        p.add( "id", 1 );

        po::variables_map vm;

        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS] [BLOCKID]\n"
                     << "\n"
                        "Prints a summary of all blocks, or details for a single block.\n"
                        "\n"
                     << desc << "\n";
                return EXIT_SUCCESS;
            }
        } catch ( exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }

        bgq::utility::Properties bg_properties( bgws_options.getBgPropertiesFileName() );

        bgq::utility::initializeLogging(
                bg_properties,
                logging_program_options
            );


        bgws_options.setBgProperties( bg_properties );


        CURLcode crc;

        if ( (crc = curl_global_init( CURL_GLOBAL_ALL )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to initialize the curl library, " + curl_easy_strerror( crc ) ) );
        }

        BOOST_SCOPE_EXIT() { curl_global_cleanup(); } BOOST_SCOPE_EXIT_END

        listBlocks(
                block_id,
                status,
                bgws_options
            );

    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
