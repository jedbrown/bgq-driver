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

#include "utility.hpp"

#include "capena-http/http/http.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/filesystem.hpp>
#include <boost/throw_exception.hpp>

#include <boost/filesystem/fstream.hpp>

#include <iostream>
#include <stdexcept>

#include <stdlib.h>


namespace fs = boost::filesystem;

using std::string;


LOG_DECLARE_FILE( "bgws.clients" );


namespace bgws_clients {
namespace utility {


void calcSessionId(
        const BgwsOptions& bgws_options,
        std::string& session_id_out
    )
{
    session_id_out = bgws_options.getSessionId();

    if ( session_id_out.empty() && bgws_options.useSessionFile() ) {
        getSessionIdFromSessionFile(
                bgws_options.getSessionFilename(),
                session_id_out
            );
    }
}


boost::filesystem::path calcDefaultSessionPath()
{
    const string DEFAULT_SESSION_FILE_NAME( ".bgws_session" );

    const char *home_env(getenv( "HOME" ));

    if ( ! home_env ) {
        return fs::path();
    }

    return (fs::path( home_env ) / DEFAULT_SESSION_FILE_NAME);
}


void getSessionIdFromSessionFile(
        const boost::filesystem::path session_path,
        std::string& session_id_out
    )
{
    fs::path use_session_path;
    if ( session_path == fs::path() ) {
        use_session_path = utility::calcDefaultSessionPath();
    } else {
        use_session_path = session_path;
    }

    fs::ifstream ifs( use_session_path );

    if ( ! ifs ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() +
                "failed to open session file " + use_session_path.string()
            ) );
    }

    // read the stream into a string
    string file_contents;
    std::getline( ifs, file_contents, '\0' /* delim, '\0' should cause whole file to be read */ );

    if ( ! ifs ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() +
                "failed to read session file " + use_session_path.string()
            ) );
    }

    json::ValuePtr val_ptr;

    try {
        val_ptr = json::Parser()( file_contents );
    } catch ( std::exception& e ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() +
                "failed to parse session file " + use_session_path.string()
            ) );
    }

    try {
        session_id_out = val_ptr->getObject().getString( "session" );
    } catch ( std::exception& e ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() +
                "session file " + use_session_path.string() + " is not valid, it doesn't contain the session ID"
            ) );
    }

    LOG_DEBUG_MSG( "Read session from " << use_session_path << ", session is " << session_id_out );
}


HeaderGuard setHeaders(
        CURL* chandle,
        char error_buffer[],
        const std::string& session_id,
        bool posting_set_content_type_json
    )
{
    curl_slist *headers_slist(NULL);

    if ( ! session_id.empty() ) {
        LOG_DEBUG_MSG( "Setting session ID to " << session_id );

        static const std::string SESSION_ID_HEADER_NAME( "X-Bgws-Session-Id" );

        headers_slist = curl_slist_append( headers_slist,
                (SESSION_ID_HEADER_NAME + ": " + session_id).c_str() // NOTE: curl_slist_append makes a copy
            );
    }

    if ( posting_set_content_type_json ) {
        headers_slist = curl_slist_append( headers_slist,
                (capena::http::header::CONTENT_TYPE + ": " + capena::http::media_type::JSON).c_str() // NOTE: curl_slist_append makes a copy
            );
    }

    // Must call curl_slist_free_all w/ headers_slist
    HeaderGuard header_guard( headers_slist, curl_slist_free_all );

    if ( curl_easy_setopt( chandle, CURLOPT_HTTPHEADER, headers_slist ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() + "failed to set the HTTPHEADER option on the curl handle, " + error_buffer ) );
    }

    return header_guard;
}


void setSslOptions(
        const BgwsOptions& bgws_options,
        CURL *chandle,
        char error_buffer[]
    )
{
    CURLcode crc;

    if ( bgws_options.getCaCertFilename().empty() ) {
        if ( (crc = curl_easy_setopt( chandle, CURLOPT_SSL_VERIFYPEER, 0L )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( string() + "failed to set the SSL_VERIFYPEER option on the curl handle, " + error_buffer ) );
        }

        if ( (crc = curl_easy_setopt( chandle, CURLOPT_SSL_VERIFYHOST, 0L )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( string() + "failed to set the SSL_VERIFYHOST option on the curl handle, " + error_buffer ) );
        }
    } else {
        if ( (crc = curl_easy_setopt( chandle, CURLOPT_CAINFO, bgws_options.getCaCertFilename().c_str() )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( string() + "failed to set the CAINFO option on the curl handle, " + error_buffer ) );
        }
    }
}


} // namespace bgws_clients::utility
} // namespace bgws_clients
