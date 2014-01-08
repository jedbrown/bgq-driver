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

#ifndef CAPENA_SERVER_RESPONSE_HPP_
#define CAPENA_SERVER_RESPONSE_HPP_


#include "fwd.hpp"
#include "ResponderStreambuf.hpp"

#include "../http/http.hpp"
#include "../http/uri/Path.hpp"

#include <boost/utility.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>


namespace capena {
namespace server {


/*! \brief The response to be filled in by the Responder.
 *
 * The application first sets the status and headers.
 *
 * Then calls headersComplete().
 *
 * If the response can have a body,
 * the application calls out() and sends the body data to the stream.
 *
 * A response can only have a body if the
 * Content-Type or Content-Range header has been set.
 * (Note that setPartialContent() sets the Content-Range header).
 *
 */
class Response : boost::noncopyable
{
public:

    Response(
            NotifyStatusHeadersFn notify_status_headers_fn, //!< [copy]
            NotifyDataFn notify_data_fn //!< [copy]
        );


    /*! \brief Sets the response to Partial Content.
     *
     *  Sets the status to PartialContent.
     *
     *  Sets the Content-range header.
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setPartialContent( const std::string& content_range );

    /*! \brief Set the response to Created.
     *
     *  Sets the status to <em>201 Created</em> and the Location header to the given URL path.
     *
     *  The response is complete after calling this.
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setCreated(
            const http::uri::Path& url_path
        );

    /*! \brief Set the HTTP status for the response.
     *
     *  Defaults to <em>100 OK</em>.
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setStatus( http::Status status );

    /*! \brief Set the Content-Type header.
     *
     *  Indicates that the response can have a body.
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setContentType( const std::string& content_type )  { setHeader( http::header::CONTENT_TYPE, content_type ); }

    /*! \brief Sets the Content-Type header to the JSON media type.
     *
     *  Indicates that the response can have a body.
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setContentTypeJson()  { setContentType( http::media_type::JSON ); }

    /*! \brief Sets the Content-Type header to the XML media type.
     *
     *  Indicates that the response can have a body.
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setContentTypeXml()  { setContentType( http::media_type::XML ); }

    /*! \brief Set the Location header
     *
     *  Throws an exception if headersComplete() has been called.
     */
    void setLocationHeader( const http::uri::Path& url_path )  { setHeader( http::header::LOCATION, url_path.toString() ); }

    /*! \brief Set the header to the value.
     *
     *  If headersComplete() has been called then throws an exception.
     *
     *  If the header has already been set then it's reset to the new value.
     */
    void setHeader( const std::string& header_name, const std::string& value );


    /*! \brief Notify that the headers are complete.
     *
     *  After calling this, you cannot call setStatus or any method that sets a header.
     */
    void headersComplete();


    /*! \brief Get the stream for the entity-body data.
     *
     * \throws runtime_error if headersComplete() has not been called or hasBody is NO_BODY.
     */
    std::ostream& out();


    /*! \brief Call this to indicate that all output is complete,
     *         otherwise output is complete when Responder goes away.
     */
    void notifyComplete();


    /*! \brief Get the status that was set in the response. */
    http::Status getStatus() const  { return _status; }

    /*! \brief Get the headers (NOT NORMALIZED). */
    const Headers& getHeaders() const  { return _headers; }

    /*! \brief Returns true iff headersComplete() has been called. */
    bool isHeadersComplete() const  { return _headers_complete; }

    /*! \brief Returns true if the response is complete. */
    bool isComplete() const  { return _complete; }

    /*! \brief Returns true iff this response can and should have a body. */
    bool hasBody() const  { return (_body_presense == BodyPresense::EXPECT_BODY); }

    /*! \brief Set the response from the exception.
     *
     * If headersComplete() has been called,
     * this method can't do much besides report the error.
     *
     * If e is a exception::Error, the response is set from the exception data.
     *
     * Otherwise, the status is set to <em>501 Internal Server Error</em>,
     * the Content-Type is "text/plain",
     * and the entity-body is set to the what() for the exception.
     *
     */
    void setException( const std::exception& e );

    ~Response();


private:

    NotifyStatusHeadersFn _notify_status_headers_fn;
    NotifyDataFn _notify_data_fn;

    http::Status _status;
    Headers _headers;
    bool _headers_complete;
    bool _complete;

    BodyPresense _body_presense;

    ResponderStreambuf _body_streambuf;
    std::ostream _body;
};


} } // namespace capena::server


#endif
