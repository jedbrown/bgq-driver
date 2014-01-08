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

#ifndef BGWS_CLIENTS_BGWS_HPP_
#define BGWS_CLIENTS_BGWS_HPP_


#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Path.hpp"

#include <string>

#include <stdint.h>


namespace bgws_clients {

class Response;


namespace bgws {


extern const capena::http::uri::Path BLOCKS_URL_PATH;
extern const capena::http::uri::Path JOBS_URL_PATH;
extern const capena::http::uri::Path LOGGING_URL_PATH;
extern const capena::http::uri::Path SESSIONS_URL_PATH;


void checkResponse(
        const Response& response,
        capena::http::Status expected_status
    );


void parseContentRange(
        const std::string& content_range_str,
        uint64_t *start_out,
        uint64_t *end_out,
        uint64_t *count_out
    );


}} // namespace bgws_clients::bgws


#endif
