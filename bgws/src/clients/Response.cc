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

#include "Response.hpp"

#include "capena-http/http/http.hpp"

#include <utility/include/Log.h>

#include <curl/curl.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using boost::lexical_cast;
using boost::regex;
using boost::regex_match;
using boost::smatch;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.clients" );


namespace bgws_clients {


size_t Response::_handle_header( void* ptr, size_t size, size_t nmemb, void* stream )
{
    Response *r(reinterpret_cast<Response*> ( stream ));

    r->_processHeader( string( reinterpret_cast<const char*> ( ptr ), size * nmemb ) );

    return (size * nmemb);
}


size_t Response::_handle_data( void* ptr, size_t size, size_t nmemb, void* stream )
{
    Response *r(reinterpret_cast<Response*> ( stream ));

    r->_processData( string( reinterpret_cast<const char*> ( ptr ), size * nmemb ) );

    return (size * nmemb);
}


Response::Response(
        CURL *chandle,
        char *error_buffer
    )
{
    CURLcode crc;

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_HEADERFUNCTION, &_handle_header )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the CURLOPT_HEADERFUNCTION option on the curl handle, " + error_buffer ) );
    }

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_WRITEHEADER, this )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the CURLOPT_HEADERFUNCTION option on the curl handle, " + error_buffer ) );
    }

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_WRITEFUNCTION, &_handle_data )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the WRITEFUNCTION option on the curl handle, " + error_buffer ) );
    }

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_WRITEDATA, this )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the WRITEDATA option on the curl handle, " + error_buffer ) );
    }
}


void Response::_processHeader( const string& s )
{
    static const regex status_line_re( "\\s*HTTP/1\\.1\\s+((\\d+)\\s+[^\\r\\n]+).*" );
    static const regex content_range_re( "\\s*Content-Range\\s*:\\s*([^\\r\\n]+).*" );
    static const regex content_type_re( "\\s*Content-Type\\s*:\\s*([^\\r\\n]+).*" );
    static const regex location_re( "\\s*Location\\s*:\\s*([^\\r\\n]+).*" );

    smatch match;

    LOG_TRACE_MSG( "Received header: " << s );

    if ( regex_match( s, match, status_line_re ) ) {
        const string &status_code_str(match[2]);

        _status_code = capena::http::Status(lexical_cast<unsigned>( status_code_str ));
        _status_text = match[1];

        return;
    }
    if ( regex_match( s, match, content_range_re ) ) {
        _content_range = match[1];
        return;
    }
    if ( regex_match( s, match, content_type_re ) ) {
        _content_type = match[1];
        return;
    }
    if ( regex_match( s, match, location_re ) ) {
        _location = match[1];
        return;
    }
}


void Response::_processData( const std::string& s )
{
    LOG_TRACE_MSG( "Received: " << s );
    _data += s;
}


} // namespace bgws_clients
