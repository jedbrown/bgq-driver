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

#ifndef CAPENA_SERVER_REQUEST_HPP_
#define CAPENA_SERVER_REQUEST_HPP_


#include "fwd.hpp"

#include "../http/fwd.hpp"
#include "../http/http.hpp"
#include "../http/uri/Uri.hpp"

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>

#include <map>
#include <sstream>
#include <string>

#include <stdint.h>


namespace capena {
namespace server {


/*! \brief Represents the HTTP request.
 *
 * The application can get the HTTP request from the AbstractResponder.
 *
 */
class Request
{
public:

    typedef boost::optional<std::string> OptionalString;


    Request(
            const std::string& request_line
        );


    //! \brief The HTTP method.
    http::Method getMethod() const  { return _method; }

    //! \brief The requested URL, parsed.
    const http::uri::Uri& getUrl() const  { return *_url_ptr; }


    //! \brief Get the content type for the request, may be empty.
    const http::MediaType& getContentType() const;

    /*! \brief Requested range, or an empty string.
     *
     * \return empty string if range not present
     */
    const std::string& getRange() const;

    /*! \brief Request headers.
     *
     *  Note that the header names are normalized to uppercase.
     */
    const Headers& getHeaders() const  { return _headers; }


    /*! \brief Returns true when the request is complete (Headers and (optional) Body). */
    bool isComplete() const;

    /*! \brief Get the entity-body if present, don't call until isComplete returns true. */
    const OptionalString& getBodyOpt() const  { return _body_opt; }


    //! \brief The full requested URL
    const std::string& getUrlStr() const  { return _url_str; }

    /*! \brief (internal) Returns true if the <em>Expect: 100-continue</em> header was present on the request. */
    bool getExpectsContinue() const  { return _expects_100_continue; }

    /*! \brief (internal) Returns the Content-Length header if present or -1 if not. */
    int64_t getContentLength() const  { return _content_length; }

    /*! \brief (internal) Returns the Transfer-Coding header if present or an empty string. */
    const std::string& getTransferCoding() const;


    /*! \brief (internal) Post a new request header, when read a header line. */
    void postHeader(
            const std::string& name_norm,
            const std::string& value
        );

    /*! \brief (internal) Post more body data to the request, when read body data. */
    void postData(
            const std::string& data,
            DataContinuesIndicator data_continues
        );


    ~Request();


private:

    http::Method _method;
    boost::scoped_ptr<http::uri::Uri> _url_ptr;

    Headers _headers;

    std::string _url_str;

    boost::scoped_ptr<http::MediaType> _content_type_ptr;
    int64_t _content_length;
    bool _expects_100_continue;

    bool _is_complete;

    OptionalString _body_opt;

};

} } // namespace capena::server

#endif
