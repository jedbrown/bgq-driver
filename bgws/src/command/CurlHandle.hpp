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

#ifndef BGWS_COMMAND_CURLHANDLE_HPP_
#define BGWS_COMMAND_CURLHANDLE_HPP_


#include "BgwsOptions.hpp"

#include "RequestData.hpp"
#include "Response.hpp"

#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Uri.hpp"

#include "chiron-json/json.hpp"

#include <boost/shared_ptr.hpp>

#include <curl/curl.h>

#include <string>


namespace bgws {
namespace command {


class CurlHandle
{
public:

    struct CtorArgs {
    public:

        // Simple GET / DELETE
        CtorArgs(
                const BgwsOptions& bgws_options,
                const capena::http::uri::Uri& uri,
                capena::http::Method http_method = capena::http::Method::GET
            );

        // GET with item range.
        CtorArgs(
                const BgwsOptions& bgws_options,
                const capena::http::uri::Uri& uri,
                const std::string& range_str // if empty stirng then will not set Range.
            );

        // POST / PUT
        CtorArgs(
                const BgwsOptions& bgws_options,
                const capena::http::uri::Uri& uri,
                json::ValuePtr request_data_val_ptr,
                capena::http::Method http_method = capena::http::Method::POST
            );


        const BgwsOptions &bgws_options;

        capena::http::Method http_method;
        capena::http::uri::Uri uri;
        std::string range_str;
        json::ValuePtr request_data_val_ptr;
    };


    CurlHandle(
            const CtorArgs& args
        );

    void perform();

    const Response& getResponse() const;

    CURL* get()  { return _chandle; }
    char* getErrorBuffer()  { return _error_buffer; }


    ~CurlHandle();


private:

    typedef boost::shared_ptr<void> _HeaderGuard;


    const BgwsOptions &_bgws_options;

    CURL *_chandle;

    char _error_buffer[CURL_ERROR_SIZE];

    std::string _url;

    _HeaderGuard _header_guard;

    boost::shared_ptr<RequestData> _request_data_ptr;
    boost::shared_ptr<Response> _response_ptr;


    void _setSslOptions();

    void _setHeaders(
            bool posting_set_content_type_json,
            const std::string& range_str
        );

};


} } // namespace bgws::command

#endif
