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

#include "CurlHandle.hpp"

#include "utility.hpp"

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using boost::lexical_cast;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.command" );


namespace bgws {
namespace command {


CurlHandle::CtorArgs::CtorArgs(
        const BgwsOptions& bgws_options,
        const capena::http::uri::Uri& uri,
        capena::http::Method http_method
    ) :
        bgws_options(bgws_options),
        http_method(http_method),
        uri(uri),
        range_str(),
        request_data_val_ptr()
{
    // Nothing to do.
}


CurlHandle::CtorArgs::CtorArgs(
        const BgwsOptions& bgws_options,
        const capena::http::uri::Uri& uri,
        const std::string& range_str
    ) :
        bgws_options(bgws_options),
        http_method(http_method),
        uri(uri),
        range_str(range_str),
        request_data_val_ptr()
{
    // Nothing to do.
}


CurlHandle::CtorArgs::CtorArgs(
        const BgwsOptions& bgws_options,
        const capena::http::uri::Uri& uri,
        json::ValuePtr request_data_val_ptr,
        capena::http::Method http_method
    ) :
        bgws_options(bgws_options),
        http_method(http_method),
        uri(uri),
        range_str(),
        request_data_val_ptr(request_data_val_ptr)
{
    // Nothing to do.
}


CurlHandle::CurlHandle(
        const CtorArgs& args
    ) :
        _bgws_options(args.bgws_options)
{
    _chandle = curl_easy_init();

    if ( _chandle == NULL ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to initialize the curl handle" ) );
    }

    CURLcode crc;

    if ( (crc = curl_easy_setopt( _chandle, CURLOPT_ERRORBUFFER, _error_buffer )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the ERRORBUFFER option on the curl handle, " + curl_easy_strerror( crc ) ) );
    }

    if ( curl_easy_setopt( _chandle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1 ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the HTTP version option on the curl handle, " + _error_buffer ) );
    }


    _url = args.uri.calcString();

    LOG_DEBUG_MSG( args.http_method << " '" << _url << "'" );

    if ( curl_easy_setopt( _chandle, CURLOPT_URL, _url.c_str() ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the URL option on the curl handle, " + _error_buffer ) );
    }

    if ( args.http_method == capena::http::Method::DELETE ) {
        if ( curl_easy_setopt( _chandle, CURLOPT_CUSTOMREQUEST, "DELETE" ) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the CUSTOMREQUEST option on the curl handle, " + _error_buffer ) );
        }
    }

    _setSslOptions();

    _setHeaders(
            args.request_data_val_ptr ? true : false,
            args.range_str
        );

    if ( args.request_data_val_ptr ) {
        string json_text(json::Formatter()( *args.request_data_val_ptr ));

        _request_data_ptr.reset( new RequestData(
                json_text,
                *this,
                args.http_method
            ) );
    }

    _response_ptr.reset( new Response( *this ) );
}


void CurlHandle::perform()
{
    if ( curl_easy_perform( _chandle ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "curl operation failed, " + _error_buffer ) );
    }
}


const Response& CurlHandle::getResponse() const
{
    if ( ! _response_ptr ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "no response" ) );
    }

    return *_response_ptr;
}


void CurlHandle::_setSslOptions()
{
    LOG_DEBUG_MSG( "Setting SSL options." );

    const bgq::utility::SslConfiguration &ssl_config(_bgws_options.getSslConfiguration());

    if ( (curl_easy_setopt( _chandle, CURLOPT_SSLCERT, ssl_config.getMyCertFilename().c_str() )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the SSLCERT option on the curl handle, " + _error_buffer ) );
    }

    if ( (curl_easy_setopt( _chandle, CURLOPT_SSLKEY, ssl_config.getMyPrivateKeyFilename().c_str() )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the SSLKEY option on the curl handle, " + _error_buffer ) );
    }

    if ( ssl_config.getCaCertificatesPath() ) {
        if ( (curl_easy_setopt( _chandle, CURLOPT_CAPATH, ssl_config.getCaCertificatesPath()->c_str() )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the CAPATH option on the curl handle, " + _error_buffer ) );
        }
    }

    if ( ssl_config.getCaCertificateFilename() ) {
        if ( (curl_easy_setopt( _chandle, CURLOPT_CAINFO, ssl_config.getCaCertificateFilename()->c_str() )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the CAINFO option on the curl handle, " + _error_buffer ) );
        }
    }

    if ( (curl_easy_setopt( _chandle, CURLOPT_SSL_VERIFYHOST, 0L )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the SSL_VERIFYHOST option on the curl handle, " + _error_buffer ) );
    }
}


void CurlHandle::_setHeaders(
        bool posting_set_content_type_json,
        const std::string& range_str
    )
{
    curl_slist *headers_slist(NULL);

    bgq::utility::UserId user_id;

    string user_id_serialized(user_id.serialize());

    string user_id_b64(utility::base64Encode( user_id_serialized ));

    static const std::string BGWS_USER_ID_HEADER_NAME( "X-Bgws-User-Id" );

    headers_slist = curl_slist_append( headers_slist,
            (BGWS_USER_ID_HEADER_NAME + ": " + user_id_b64).c_str() // NOTE: curl_slist_append makes a copy
        );

    if ( posting_set_content_type_json ) {
        headers_slist = curl_slist_append( headers_slist,
                (capena::http::header::CONTENT_TYPE + ": " + capena::http::media_type::JSON).c_str() // NOTE: curl_slist_append makes a copy
            );
        headers_slist = curl_slist_append( headers_slist,
                (capena::http::header::TRANSFER_ENCODING + ": " + capena::http::TRANSFER_CODING_CHUNKED).c_str() // NOTE: curl_slist_append makes a copy
            );
    }

    if ( ! range_str.empty() ) {

        headers_slist = curl_slist_append( headers_slist,
                (capena::http::header::RANGE + ": " + range_str).c_str() // NOTE: curl_slist_append makes a copy
            );

    }

    // Must call curl_slist_free_all w/ headers_slist
    _header_guard = _HeaderGuard( headers_slist, curl_slist_free_all );

    if ( curl_easy_setopt( _chandle, CURLOPT_HTTPHEADER, headers_slist ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( string() + "failed to set the HTTPHEADER option on the curl handle, " + _error_buffer ) );
    }
}


CurlHandle::~CurlHandle()
{
    curl_easy_cleanup( _chandle );
}


} } // namespace bgws::command
