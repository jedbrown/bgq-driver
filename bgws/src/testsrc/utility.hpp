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

#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include "capena-http/http/http.hpp"

#include <boost/lexical_cast.hpp>

#include <curl/curl.h>

#include <map>
#include <stdexcept>
#include <string>


#define THROW( msg_expr ) \
    throw std::runtime_error( std::string() + msg_expr + " @ " + boost::lexical_cast<std::string> ( __LINE__ ) )


const std::string BLOCKS_URL( "http://localhost/bg/blocks" );


const std::string CONTENT_TYPE_JSON("application/json");


class Response
{
public:
    typedef std::map<std::string,std::string> Headers;


    void setup( CURL* chandle );

    capena::http::Status getHttpStatus() const  { return _http_status; }

    const Headers& getHeaders() const  { return _headers; }

    const std::string& getContentType() const  { return _headers.at( "Content-Type" ); }

    const std::string& getContent() const  { return _content; }


private:

    static size_t _handleHeader( void *ptr, size_t  size,  size_t  nmemb, void *stream );

    static size_t _receiveString( void *ptr, size_t  size,  size_t  nmemb, void  *stream );


    capena::http::Status _http_status;
    Headers _headers;

    std::string _content;


    void _checkHeader( const std::string& s );
};


struct CurlGlobalSetup
{
    CurlGlobalSetup();

    ~CurlGlobalSetup();
};


void http_get(
        const std::string& url,
        Response& response_out
    );


void http_post(
        const std::string& url,
        const std::string& post_data,
        Response& response_out,
        const std::string& content_type = CONTENT_TYPE_JSON
    );


void http_delete(
        const std::string& url,
        Response& response_out
    );


void simpleDeleteBlock( const std::string& block_id );

std::string calc_block_url( const std::string& block_id );

std::string calc_create_single_midplane_block_json( const std::string& block_id );

#endif
