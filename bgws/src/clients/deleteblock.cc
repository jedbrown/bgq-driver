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

#include "capena-http/http/uri/Path.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>
#include <boost/throw_exception.hpp>

#include <curl/curl.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


using namespace bgws_clients;

using std::cerr;
using std::cout;
using std::exception;
using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws.clients" );


static void deleteBlock(
        const string& block_id,
        const BgwsOptions& bgws_options
    )
{
    CURLcode crc;

    capena::http::uri::Path url_path(bgws::BLOCKS_URL_PATH / block_id);

    string url(bgws_options.getBase() + url_path.toString());

    LOG_DEBUG_MSG( "DELETE '" << url << "'" );

    CURL *chandle(curl_easy_init());

    if ( chandle == NULL ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to initialize the curl handle" ) );
    }

    BOOST_SCOPE_EXIT( (&chandle) ) { curl_easy_cleanup( chandle ); } BOOST_SCOPE_EXIT_END

    char error_buffer[CURL_ERROR_SIZE] = { 0 };

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_ERRORBUFFER, error_buffer )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the ERRORBUFFER option on the curl handle, " + curl_easy_strerror( crc ) ) );
    }

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the URL option on the curl handle, " + error_buffer ) );
    }

    bgws_clients::utility::setSslOptions(
            bgws_options,
            chandle,
            error_buffer
        );

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_CUSTOMREQUEST, "DELETE" )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the CUSTOMREQUEST option on the curl handle, " + error_buffer ) );
    }

    string session_id;
    utility::calcSessionId(
            bgws_options,
            session_id
        );

    utility::HeaderGuard hg(utility::setHeaders( chandle, error_buffer, session_id ));

    Response response( chandle, error_buffer );

    if ( (crc = curl_easy_perform( chandle )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "curl operation failed, " + error_buffer ) );
    }

    // response should now hold the server's response

    // Check to make sure that the response looks OK, throws if it's not.
    bgws::checkResponse( response, capena::http::Status::NoContent );
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);
        vector<string> block_ids;

        BgwsOptions bgws_options;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description all_opts_desc;

        all_opts_desc.add_options()
                ( "id", po::value( &block_ids ), "Block ID" )
            ;

        po::options_description desc( "Options" );
        desc.add( bgws_options.getDesc() );
        desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( desc );

        all_opts_desc.add( desc );

        try {
            po::positional_options_description p;
            p.add( "id", -1 );

            po::variables_map vm;
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS] BLOCKID\n"
                     << "\n"
                        "Delete the block.\n"
                        "\n"
                     << desc << "\n";
                return 0;
            }

            if ( vm.count( "id" ) == 0 ) {
                BOOST_THROW_EXCEPTION( runtime_error( "must specify a block" ) );
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

        BOOST_FOREACH( const string& block_id, block_ids ) {
            deleteBlock( block_id, bgws_options );
        }
    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;
    }
}
