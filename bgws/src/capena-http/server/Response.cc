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

#include "Response.hpp"

#include "exception.hpp"
#include "AbstractResponder.hpp"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "capena-http" );


namespace capena {
namespace server {


Response::Response(
        NotifyStatusHeadersFn notify_status_headers_fn, //!< [copy]
        NotifyDataFn notify_data_fn //!< [copy]
    ) :
        _notify_status_headers_fn(notify_status_headers_fn),
        _notify_data_fn(notify_data_fn),
        _status(http::Status::OK),
        _headers_complete(false),
        _complete(false),
        _body_presense(BodyPresense::NO_BODY),
        _body_streambuf( notify_data_fn ),
        _body( &_body_streambuf )
{
    // Nothing to do.
}


void Response::setPartialContent( const std::string& content_range )
{
    setStatus( http::Status::PartialContent );
    setHeader( http::header::CONTENT_RANGE, content_range );
}


void Response::setCreated(
        const http::uri::Path& url_path
    )
{
    setStatus( capena::http::Status::Created );
    setLocationHeader( url_path );
    headersComplete();
}


void Response::setStatus( http::Status status )
{
    if ( _headers_complete ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "trying to set status when status is already complete" ) );
    }

    _status = status;

    LOG_DEBUG_MSG( "Set status to " << http::getText( _status ) );
}


void Response::setHeader( const std::string& header_name, const std::string& value )
{
    if ( _headers_complete ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "trying to set header when headers are already complete" ) );
    }

    _headers[header_name] = value;

    string header_name_norm(boost::algorithm::to_upper_copy( header_name ));

    if ( (header_name_norm == http::header_norm::CONTENT_TYPE) ||
         (header_name_norm == http::header_norm::CONTENT_RANGE)
       )
    {
        _body_presense = BodyPresense::EXPECT_BODY;
    }

}


void Response::headersComplete()
{
    if ( _headers_complete ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "headersComplete called twice" ) );
    }

    _headers_complete = true;
    if ( ! hasBody() )  _complete = true;

    _notify_status_headers_fn(
            _status,
            _headers,
            _body_presense
        );

    _notify_status_headers_fn = NotifyStatusHeadersFn();
}


void Response::notifyComplete()
{
    _complete = true;
}


void Response::setException( const std::exception& e )
{
    if ( dynamic_cast<const exception::Error*>( &e ) ) {

        const exception::Error *afe_p(dynamic_cast<const exception::Error*>( &e ));

        LOG_WARN_MSG( "capena::server::exception::Error handling request, diagnostic info:\n" << boost::diagnostic_information( *afe_p ) );

        if ( _headers_complete ) {
            LOG_WARN_MSG( "Cannot send exception response to client because already sent headers." );
        } else {
            afe_p->updateResponse( *this );
        }

    } else {
        // It's a regular exception

        LOG_WARN_MSG( "Exception handling request, diagnostic info:\n" << boost::diagnostic_information( e ) );

        if ( _headers_complete ) {

            LOG_WARN_MSG( "Cannot send exception response to client because already sent headers." );

        } else {

            setStatus( http::Status::InternalServerError );
            setContentType( http::media_type::PLAIN_TEXT );
            headersComplete();

            out() << "Exception handling the request. Check the BGWS server log.\n\n";

            notifyComplete();
        }

    }
}


std::ostream& Response::out()
{
    if ( ! _headers_complete ) {
        BOOST_THROW_EXCEPTION(std::runtime_error( "Application called Response::out() before headersComplete()." ));
    }
    if ( _body_presense == BodyPresense::NO_BODY ) {
        BOOST_THROW_EXCEPTION(std::runtime_error( "Application called Response::out() when no body can be present." ));
    }

    return _body;
}


Response::~Response()
{
    LOG_DEBUG_MSG( "Destroying Response. has_body=" << (_body_presense == BodyPresense::EXPECT_BODY) );

    if ( ! _headers_complete ) {
        headersComplete();
    }

    if ( _body_presense == BodyPresense::EXPECT_BODY ) {
        _body.flush();
        _notify_data_fn( string(), DataContinuesIndicator::END_OF_DATA );
        _notify_data_fn = NotifyDataFn();
    }
}


} } // namespace capena::server
