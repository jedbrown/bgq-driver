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

#include "RequestData.hpp"

#include "capena-http/http/http.hpp"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>


using boost::shared_ptr;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.clients" );


size_t RequestData::_readFn( void* ptr, size_t size, size_t nmemb, void* stream )
{
    RequestData &request_data(*(reinterpret_cast<RequestData*> ( stream )));

    return request_data._read( ptr, size * nmemb );
}


RequestData::RequestData(
        const std::string& data_str,
        CURL *chandle,
        char error_buffer[],
        capena::http::Method http_method
    ) :
        _data(data_str),
        _data_i(_data.begin())
{
    LOG_DEBUG_MSG( "Data to post:\n" << _data << "\n" );

    if ( ! (http_method == capena::http::Method::POST || http_method == capena::http::Method::PUT) ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( "http method must be POST or PUT" ) );
    }

    if ( http_method == capena::http::Method::POST ) {
        if ( curl_easy_setopt( chandle, CURLOPT_POST, 1 ) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the POST option on the curl handle, " + error_buffer ) );
        }
    } else {
        if ( curl_easy_setopt( chandle, CURLOPT_UPLOAD, 1 ) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the UPLOAD option on the curl handle, " + error_buffer ) );
        }
    }

    if ( http_method == capena::http::Method::POST ) {
        if ( curl_easy_setopt( chandle, CURLOPT_POSTFIELDSIZE, _data.size() ) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the POSTFIELDSIZE option on the curl handle, " + error_buffer ) );
        }
    } else {
        if ( curl_easy_setopt( chandle, CURLOPT_INFILESIZE, _data.size() ) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the INFILESIZE option on the curl handle, " + error_buffer ) );
        }
    }

    if ( curl_easy_setopt( chandle, CURLOPT_READFUNCTION, &_readFn ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the READFUNCTION option on the curl handle, " + error_buffer ) );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_READDATA, this ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the READDATA option on the curl handle, " + error_buffer ) );
    }
}


size_t RequestData::_read( void* ptr, size_t buf_size ) {
    size_t remaining_to_write(_data.end() - _data_i);
    size_t write_this_time(std::min( buf_size, remaining_to_write ));

    std::copy( _data_i, _data_i + write_this_time, reinterpret_cast<char*> ( ptr ) );

    _data_i += write_this_time;

    return write_this_time;
}
