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
#include "RequestData.hpp"
#include "Response.hpp"
#include "utility.hpp"

#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>
#include <boost/throw_exception.hpp>

#include <boost/filesystem/fstream.hpp>

#include <curl/curl.h>

#include <openssl/evp.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <stdint.h>
#include <unistd.h>


using namespace bgws_clients;

namespace fs = boost::filesystem;

using std::cout;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.clients" );


typedef std::vector<uint8_t> Bytes;


static std::string base64Encode( const Bytes& bytes )
{
    // use openssl to base-64 encode the bytes.

    std::vector<char> b64_chars( ((bytes.size() + 2) / 3 * 4) + 1 );

    int bytes_written(EVP_EncodeBlock(
            reinterpret_cast<unsigned char*>( b64_chars.data() ),
            bytes.data(),
            bytes.size()
        ));

    string ret( b64_chars.begin(), b64_chars.begin() + bytes_written );

    return ret;
}


static void listSessions(
        const BgwsOptions& bgws_options
    )
{
    CURLcode crc;

    string url(bgws_options.getBase() + bgws::SESSIONS_URL_PATH.toString());

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

    bgws_clients::utility::setSslOptions(
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

    const json::Array &sessions_arr(value_ptr->getArray());

    for ( json::Array::const_iterator i(sessions_arr.begin()) ; i != sessions_arr.end() ; ++i ) {
        cout << (*i)->getObject().getString( "id" ) << "\n";
    }
}


static void endSession(
        const BgwsOptions& bgws_options
    )
{
    string session_id;

    utility::calcSessionId(
            bgws_options,
            session_id
        );

    CURLcode crc;

    capena::http::uri::Path url_path(bgws::SESSIONS_URL_PATH / session_id);

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

    utility::HeaderGuard hg(utility::setHeaders( chandle, error_buffer, session_id ));

    Response response( chandle, error_buffer );

    if ( (crc = curl_easy_perform( chandle )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "curl operation failed, " + error_buffer ) );
    }

    // response should now hold the server's response

    // Check to make sure that the response looks OK, throws if it's not.
    bgws::checkResponse( response, capena::http::Status::NoContent );
}


static void startSession(
        const std::string& username,
        const std::string& password,
        const BgwsOptions& bgws_options,
        std::string& session_id_out
    )
{
    CURLcode crc;

    string auth_str( username + ":" + password );

    string auth_b64_str(base64Encode( Bytes( auth_str.begin(), auth_str.end() )));

    json::ObjectValuePtr auth_obj_ptr(json::Object::create());
    auth_obj_ptr->get().set( "auth", auth_b64_str );

    string url(bgws_options.getBase() + bgws::SESSIONS_URL_PATH.toString());

    LOG_DEBUG_MSG( "Posting to '" << url << "'" );

    CURL *chandle(curl_easy_init());

    if ( chandle == NULL ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to initialize the curl handle" ) );
    }

    BOOST_SCOPE_EXIT( (&chandle) ) { curl_easy_cleanup( chandle ); } BOOST_SCOPE_EXIT_END;

    char error_buffer[CURL_ERROR_SIZE] = { 0 };

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_ERRORBUFFER, error_buffer )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the ERRORBUFFER option on the curl handle, " + curl_easy_strerror( crc ) ) );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the URL option on the curl handle, " + error_buffer ) );
    }

    bgws_clients::utility::setSslOptions(
            bgws_options,
            chandle,
            error_buffer
        );

    utility::HeaderGuard hg(utility::setHeaders( chandle, error_buffer, bgws_options.getSessionId(), true ));

    RequestData request_data(
            json::Formatter()( *auth_obj_ptr ),
            chandle, error_buffer
        );

    Response response( chandle, error_buffer );

    if ( curl_easy_perform( chandle ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "curl operation failed, " + error_buffer ) );
    }

    // response should now hold the server's response.

    // Check to make sure that the response looks OK, throws if it's not.
    bgws::checkResponse( response, capena::http::Status::Created );

    const string &location(response.getLocation());

    capena::http::uri::Path location_url_path( location );

    session_id_out = location_url_path.back();
}


static void writeSessionFile(
        const boost::filesystem::path& session_path,
        const std::string& session_id
    )
{
    fs::path use_session_path;
    if ( session_path == fs::path() ) {
        use_session_path = utility::calcDefaultSessionPath();
    } else {
        use_session_path = session_path;
    }

    umask( 0066 ); // Set the umask so only the user can read.
    fs::ofstream ofs( use_session_path, std::ios::trunc );

    json::ObjectValuePtr obj_val_ptr(json::Object::create());
    obj_val_ptr->get().set( "session", session_id );

    json::Formatter()( *obj_val_ptr, ofs );
}


static void deleteSessionFile(
        const boost::filesystem::path& session_path
    )
{
    fs::path use_session_path;
    if ( session_path == fs::path() ) {
        use_session_path = utility::calcDefaultSessionPath();
    } else {
        use_session_path = session_path;
    }

    fs::remove( use_session_path );
}


int main( int argc, char *argv[] )
{
    namespace po = boost::program_options;

    try {

        bool end_session(false);
        bool help(false);
        bool list_sessions(false);
        string username, password;

        bool read_password(false);

        BgwsOptions bgws_options;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description visible_desc( "Options" );
        visible_desc.add( bgws_options.getDesc() );
        visible_desc.add_options()
                ( "end", po::bool_switch( &end_session ), "End session" )
                ( "list", po::bool_switch( &list_sessions ), "List sessions" )
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( visible_desc );

        po::options_description all_opts_desc;
        all_opts_desc.add( visible_desc );
        all_opts_desc.add_options()
                ( "username", po::value( &username ), "User name" )
                ( "password", po::value( &password ), "User password" )
            ;

        po::positional_options_description positional_options_desc;
        positional_options_desc.add( "username", 1 );
        positional_options_desc.add( "password", 1 );

        po::variables_map vm;

        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( positional_options_desc ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS] USERNAME [PASSWORD] | --end | --list\n"
                     << "\n"
                        "Perform BGWS session operations.\n"
                        "\n"
                     << visible_desc << "\n";
                return 0;
            }

            // Username and password are ignored if not starting session and required if starting session.
            if ( end_session || list_sessions ) {
                if ( vm.count( "username" ) != 0 || vm.count( "password" ) != 0 )  cout << "Ignoring username and password arguments because ending session.\n";
            } else {
                if ( vm.count( "username" ) == 0 ) {
                    BOOST_THROW_EXCEPTION( runtime_error( string() + "username is required" ) );
                }

                if ( vm.count( "password" ) == 0 ) {
                    read_password = true;
                }
            }
        } catch ( std::exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }

        bgq::utility::Properties bg_properties( bgws_options.getBgPropertiesFileName() );

        bgq::utility::initializeLogging(
                bg_properties,
                logging_program_options
            );


        bgws_options.setBgProperties( bg_properties );


        if ( read_password ) {
            password = getpass( "Password: " );
        }

        CURLcode crc;

        if ( (crc = curl_global_init( CURL_GLOBAL_ALL )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to initialize the curl library, " + curl_easy_strerror( crc ) ) );
        }

        BOOST_SCOPE_EXIT() { curl_global_cleanup(); } BOOST_SCOPE_EXIT_END;

        if ( list_sessions || end_session ) {

            if ( list_sessions ) {
                listSessions(
                        bgws_options
                    );
            }

            if ( end_session ) {
                endSession(
                        bgws_options
                    );

                if ( bgws_options.useSessionFile() ) {
                    deleteSessionFile(
                            bgws_options.getSessionFilename()
                        );
                }
            }

        } else {
            // Start new session.

            string session_id;

            startSession(
                    username,
                    password,
                    bgws_options,
                    session_id
                );

            if ( bgws_options.useSessionFile() ) {
                writeSessionFile(
                        bgws_options.getSessionFilename(),
                        session_id
                    );
            } else {
                cout << "Session ID: " << session_id << "\n";
            }

        }

        return 0;

    } catch ( std::exception& e ) {

        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;

    }
}
