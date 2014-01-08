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

#include <curl/curl.h>

#include "capena-http/http/http.hpp"

#include <string>

#ifndef BGWS_CLIENTS_RESPONSE_HPP_
#define BGWS_CLIENTS_RESPONSE_HPP_


namespace bgws_clients {


class Response
{
public:

    Response(
            CURL *chandle,
            char *error_buffer
        );


    capena::http::Status getStatusCode() const  { return _status_code; }

    const std::string& getStatusText() const  { return _status_text; }

    const std::string& getContentRange() const  { return _content_range; }
    const std::string& getContentType() const  { return _content_type; }
    const std::string& getLocation() const  { return _location; }

    const std::string& getData() const  { return _data; }


private:

    // Callback for curl to process header.
    static size_t _handle_header( void* ptr, size_t size, size_t nmemb, void* stream );


    // Callback for curl when response arrives.
    static size_t _handle_data( void* ptr, size_t size, size_t nmemb, void* stream );


    capena::http::Status _status_code;
    std::string _status_text;

    std::string _content_range;
    std::string _content_type;
    std::string _location;

    std::string _data;


    void _processHeader( const std::string& s );


    void _processData( const std::string& s );

};


} // namespace bgws_clients

#endif
