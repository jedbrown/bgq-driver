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

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

#include <algorithm>
#include <string>


using boost::lexical_cast;
using boost::regex;
using boost::regex_match;
using boost::shared_ptr;
using boost::smatch;

using std::min;
using std::string;


//---------------------------------------------------------------------
// class Response

size_t Response::_handleHeader( void *ptr, size_t  size,  size_t  nmemb, void *stream )
{
    Response &response(*(reinterpret_cast<Response*> ( stream )));

    response._checkHeader( string( reinterpret_cast<const char*> ( ptr ), size * nmemb ) );

    return (size * nmemb);
}


size_t Response::_receiveString( void *ptr, size_t  size,  size_t  nmemb, void  *stream )
{
    string *str_p(reinterpret_cast<string*> ( stream ));

    *str_p +=  string( reinterpret_cast<const char*> ( ptr ), size * nmemb );

    return (size * nmemb);
}


void Response::setup( CURL* chandle )
{
    if ( curl_easy_setopt( chandle, CURLOPT_HEADERFUNCTION, &_handleHeader ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_HEADERDATA, this ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_WRITEFUNCTION, &_receiveString ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_WRITEDATA, &_content ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }
}


void Response::_checkHeader( const std::string& s )
{
    static const regex status_line_re( "\\s*HTTP/1\\.1\\s+((\\d+)\\s+[^\\r\\n]+).*" );
    static const regex header_re( "\\s*([^\\s:]+)\\s*:\\s*([^\\r\\n]+).*" );

    smatch match;

    if ( regex_match( s, match, status_line_re ) ) {
        const string &status_code_str(match[2]);

        _http_status = capena::http::Status(lexical_cast<unsigned>( status_code_str ));
        return;
    }
    if ( regex_match( s, match, header_re ) ) {
        _headers[match[1]] = match[2];
        return;
    }
}


//---------------------------------------------------------------------
// class CurlGlobalSetup


CurlGlobalSetup::CurlGlobalSetup()
{
    if ( curl_global_init( CURL_GLOBAL_ALL ) != CURLE_OK ) {
        THROW( "curl_global_init" );
    }
}


CurlGlobalSetup::~CurlGlobalSetup()
{
    curl_global_cleanup();
}


//---------------------------------------------------------------------
// Static functions

static size_t sendString( void *ptr, size_t size, size_t nmemb, void *stream )
{
    string *str_p(reinterpret_cast<string*> ( stream ));

    size_t bytes_to_copy(min( str_p->size(), size * nmemb ));

    copy( str_p->begin(), str_p->begin() + bytes_to_copy, reinterpret_cast<char*> ( ptr ) );

    *str_p = str_p->substr( bytes_to_copy );

    return bytes_to_copy;
}


//---------------------------------------------------------------------
// Exported functions

void http_get(
        const string& url,
        Response& response_out
    )
{
    CURL *chandle(curl_easy_init());

    if ( ! chandle ) {
        THROW( "curl_easy_init" );
    }

    // Must call curl_easy_cleanup w/ chandle
    shared_ptr<void> chandle_guard( chandle, curl_easy_cleanup );

    if ( curl_easy_setopt( chandle, CURLOPT_VERBOSE, 0 ) != CURLE_OK ) { // Enable this for debug!
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    response_out.setup( chandle );

    if ( curl_easy_perform( chandle ) != CURLE_OK ) {
        THROW( "curl_easy_perform" );
    }
}


void http_post(
        const string& url,
        const string& post_data,
        Response& response_out,
        const std::string& content_type
    )
{
    CURL *chandle(curl_easy_init());

    if ( ! chandle ) {
        THROW( "curl_easy_init" );
    }

    // Must call curl_easy_cleanup w/ chandle
    shared_ptr<void> chandle_guard( chandle, curl_easy_cleanup );

    if ( curl_easy_setopt( chandle, CURLOPT_VERBOSE, 0 ) != CURLE_OK ) { // Enable this for debug!
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_POST, 1 ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    // set the Content-Type to the requested content type.
    curl_slist *headers_slist(NULL);

    headers_slist = curl_slist_append( headers_slist,
            (string() + "Content-Type: " + content_type).c_str() // NOTE: copies the string.
        );

    // Must call curl_slist_free_all w/ headers_slist
    shared_ptr<void> headers_slist_guard( headers_slist, curl_slist_free_all );

    if ( curl_easy_setopt( chandle, CURLOPT_HTTPHEADER, headers_slist ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_POSTFIELDSIZE, post_data.size() ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_READDATA, &post_data ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_READFUNCTION, &sendString ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    response_out.setup( chandle );

    if ( curl_easy_perform( chandle ) != CURLE_OK ) {
        THROW( "curl_easy_perform" );
    }
}


void http_delete(
        const std::string& url,
        Response& response_out
    )
{
    CURL *chandle(curl_easy_init());

    if ( ! chandle ) {
        THROW( "curl_easy_init" );
    }

    // Must call curl_easy_cleanup w/ chandle
    shared_ptr<void> chandle_guard( chandle, curl_easy_cleanup );

    if ( curl_easy_setopt( chandle, CURLOPT_VERBOSE, 0 ) != CURLE_OK ) { // Enable this for debug!
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_CUSTOMREQUEST, "DELETE" ) != CURLE_OK ) {
        THROW( "curl_easy_setopt" );
    }

    response_out.setup( chandle );

    if ( curl_easy_perform( chandle ) != CURLE_OK ) {
        THROW( "curl_easy_perform" );
    }
}


void simpleDeleteBlock( const string& block_id )
{
    Response response;

    http_delete(
            calc_block_url( block_id ),
            response
        );

    if ( response.getHttpStatus() != capena::http::Status::NoContent ) {
        THROW( "delete block" );
    }
}


std::string calc_block_url( const std::string& block_id )
{
    return (BLOCKS_URL + "/" + block_id);
}


std::string calc_create_single_midplane_block_json( const std::string& block_id )
{
    string json( "{ \"id\": \"" + block_id + "\", \"midplane\": \"R00-M0\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

    return json;
}
