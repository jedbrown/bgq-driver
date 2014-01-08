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

#ifndef CURL_HANDLE_HPP_
#define CURL_HANDLE_HPP_


#include <utility/include/Properties.h>

#include <boost/asio.hpp>

#include <string>

#include <curl/curl.h>


class CurlHandle
{
public:

    CurlHandle(
            const boost::asio::ip::tcp::endpoint& endpoint,
            const bgq::utility::Properties &properties,
            const std::string &resource = std::string()
        );


    void setClientCert(
            const std::string& cert_filename,
            const std::string& key_filename
        );

    void setPostWithContentLength( const std::string& data );

    void setPostChunked( const std::string& data );

    void setPut( const std::string& data );

    void setOperation( const std::string& operation );

    void setResult( std::string* result_out );

    void perform();


    int getResponseCode() const { return _response_status_code; }


    ~CurlHandle();


private:

    CURL *_curl_p;

    std::string *_result_out;

    std::string _send_data;
    size_t _send_data_pos;

    struct curl_slist *_request_headers_slist_p;

    bool _expect_status_line;

    int _response_status_code;


    size_t _doWrite( void* data_ptr, size_t size, size_t nmemb );
    size_t _doRead( void* ptr, size_t size, size_t nmemb );

    void _setChunked();

    size_t _header( void* ptr, size_t size, size_t nmemb );


    static size_t _writeFunctionStatic( void* data_ptr, size_t size, size_t nmemb, void* stream );

    static size_t _readFunctionStatic( void* ptr, size_t size, size_t nmemb, void* stream );

    static size_t _headerFunctionStatic( void* ptr, size_t size, size_t nmemb, void* stream );

};


#endif
