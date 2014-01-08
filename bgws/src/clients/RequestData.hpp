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

#ifndef CLIENTS_REQUEST_DATA_HPP_
#define CLIENTS_REQUEST_DATA_HPP_


#include "capena-http/http/http.hpp"

#include <curl/curl.h>

#include <string>


class RequestData
{
public:

    RequestData(
            const std::string& data_str,
            CURL *chandle,
            char error_buffer[],
            capena::http::Method http_method = capena::http::Method::POST
        );

private:

    static size_t _readFn( void* ptr, size_t size, size_t nmemb, void* stream );


    std::string _data;
    std::string::iterator _data_i;


    size_t _read( void* ptr, size_t buf_size );
};


#endif
