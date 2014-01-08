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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "CurlHandle.hpp"

#include <boost/lexical_cast.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <iostream>

#include <assert.h>
#include <string.h>


using boost::lexical_cast;

using namespace boost::xpressive;

using std::cout;
using std::string;


CurlHandle::CurlHandle(
        const boost::asio::ip::tcp::endpoint& endpoint,
        const bgq::utility::Properties &properties,
        const std::string &resource
    ) :
        _request_headers_slist_p(NULL)
{
    assert( curl_global_init( CURL_GLOBAL_SSL ) == CURLE_OK );
        // You might ask yourself why curl_global_init every time rather than once in the process?
        // There seems to be a problem in the curl library where the next request uses the same cert and key files!
        // The work around is to curl_global_init/curl_global_cleanup every time.

    _curl_p = curl_easy_init();

    assert( _curl_p );


    const string url(string() + "https://" + lexical_cast<string>( endpoint ) + "/" + resource);

    cout << "Request URL: " << url << "\n";

    assert( curl_easy_setopt( _curl_p, CURLOPT_URL, url.c_str() ) == CURLE_OK );

    assert( curl_easy_setopt( _curl_p, CURLOPT_CAINFO, properties.getValue( "security.ca", "certificate" ).c_str() ) == CURLE_OK );

    assert( curl_easy_setopt( _curl_p, CURLOPT_SSL_VERIFYHOST, 0L ) == CURLE_OK );


    assert( curl_easy_setopt( _curl_p, CURLOPT_HEADERFUNCTION, &_headerFunctionStatic ) == CURLE_OK );
    assert( curl_easy_setopt( _curl_p, CURLOPT_HEADERDATA, this ) == CURLE_OK );
}


void CurlHandle::setClientCert(
        const std::string& cert_filename,
        const std::string& key_filename
    )
{
    // cout << "Using SSLCERT=" << cert_filename << "\n";
    // cout << "Using SSLKEY=" << key_filename << "\n";

    assert( curl_easy_setopt( _curl_p, CURLOPT_SSLCERT, cert_filename.c_str() ) == CURLE_OK );
    assert( curl_easy_setopt( _curl_p, CURLOPT_SSLKEY, key_filename.c_str() ) == CURLE_OK );
}


void CurlHandle::setPostWithContentLength( const std::string& data )
{
    _send_data = data;

    assert( curl_easy_setopt( _curl_p, CURLOPT_POST, 1 ) == CURLE_OK );
    assert( curl_easy_setopt( _curl_p, CURLOPT_POSTFIELDS, _send_data.data() ) == CURLE_OK );
    assert( curl_easy_setopt( _curl_p, CURLOPT_POSTFIELDSIZE, _send_data.size() ) == CURLE_OK );
}


void CurlHandle::setPostChunked( const std::string& data )
{
    _send_data = data;

    _setChunked();

    assert( curl_easy_setopt( _curl_p, CURLOPT_POST, 1 ) == CURLE_OK );
}


void CurlHandle::setPut( const std::string& data )
{
    _send_data = data;

    _setChunked();

    assert( curl_easy_setopt( _curl_p, CURLOPT_UPLOAD, 1 ) == CURLE_OK );
}


void CurlHandle::setOperation( const std::string& operation )
{
    assert( curl_easy_setopt( _curl_p, CURLOPT_CUSTOMREQUEST, operation.c_str() ) == CURLE_OK );
}


void CurlHandle::setResult( std::string* result_out )
{
    _result_out = result_out;
    assert( curl_easy_setopt( _curl_p, CURLOPT_WRITEFUNCTION, &_writeFunctionStatic ) == CURLE_OK );
    assert( curl_easy_setopt( _curl_p, CURLOPT_WRITEDATA, this ) == CURLE_OK );
}


void CurlHandle::perform()
{
    _expect_status_line = true;

    assert( curl_easy_perform( _curl_p ) == CURLE_OK );
}


CurlHandle::~CurlHandle()
{
    if ( _request_headers_slist_p ) {
        curl_slist_free_all( _request_headers_slist_p );
    }

    curl_easy_cleanup( _curl_p );

    curl_global_cleanup();
}


size_t CurlHandle::_doWrite( void* data_ptr, size_t size, size_t nmemb )
{
    _result_out->append( string( static_cast<char*>(data_ptr), size * nmemb ) );
    return (size * nmemb);
}


size_t CurlHandle::_doRead( void* ptr, size_t size, size_t nmemb )
{
    // Transfer data from _send_data to ptr.
    size_t bytes_remaining_in_data(_send_data.size() - _send_data_pos);
    size_t max_bytes_in_buffer(size * nmemb);
    size_t bytes_to_copy(std::min( bytes_remaining_in_data, max_bytes_in_buffer));
    memcpy( ptr, _send_data.data() + _send_data_pos, bytes_to_copy );
    _send_data_pos += bytes_to_copy;
    return bytes_to_copy;
}


void CurlHandle::_setChunked()
{
    _send_data_pos = 0;

    _request_headers_slist_p = curl_slist_append( _request_headers_slist_p, "Transfer-Encoding: chunked" );
    assert( _request_headers_slist_p != NULL );

    assert( curl_easy_setopt( _curl_p, CURLOPT_HTTPHEADER, _request_headers_slist_p ) == CURLE_OK );

    assert( curl_easy_setopt( _curl_p, CURLOPT_READFUNCTION, &_readFunctionStatic ) == CURLE_OK );
    assert( curl_easy_setopt( _curl_p, CURLOPT_READDATA, this ) == CURLE_OK );
}


size_t CurlHandle::_header( void* ptr, size_t size, size_t nmemb )
{
    size_t sz(size * nmemb);
    string header_line( static_cast<char*>( ptr ), sz );
    // cout << "Got header(" << _expect_status_line << "): " << header_line << "\n";

    if ( _expect_status_line ) {

        static const sregex status_line_re = as_xpr("HTTP/1.1 ") >> (s1=+_d) >> " " >> +_;

        smatch matches;

        assert( regex_search( header_line, matches, status_line_re ) );
        _response_status_code = lexical_cast<int>( matches[1] );

        _expect_status_line = false;
    }

    return sz;
}


size_t CurlHandle::_writeFunctionStatic( void* data_ptr, size_t size, size_t nmemb, void* stream )
{
    CurlHandle *curl_handle_p(static_cast<CurlHandle*>( stream ));

    return curl_handle_p->_doWrite( data_ptr, size, nmemb );
}


size_t CurlHandle::_readFunctionStatic( void* ptr, size_t size, size_t nmemb, void* stream )
{
    CurlHandle *curl_handle_p(static_cast<CurlHandle*>( stream ));

    return curl_handle_p->_doRead( ptr, size, nmemb );
}


size_t CurlHandle::_headerFunctionStatic( void* ptr, size_t size, size_t nmemb, void* stream )
{
    CurlHandle *curl_handle_p(static_cast<CurlHandle*>( stream ));
    return curl_handle_p->_header( ptr, size, nmemb );
}
