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

#include "exception.hpp"

#include "Response.hpp"

#include <sstream>


using std::string;


namespace capena {
namespace server {
namespace exception {


//-------------------------------------------------------------------------
// class Error


Error::Error(
        http::Status http_status,
        const std::string& what_str,
        const std::string& response_str
    ) :
        std::runtime_error( what_str ),
        _http_status(http_status),
        _response_str(response_str)
{
    /* nothing to do */
}


void Error::updateResponse( Response& r ) const
{
    r.setStatus( _http_status );
    _setHeaders( r );

    // if headers doesn't have a ContentType, set it to text/plain.

    if ( r.getHeaders().find( http::header::CONTENT_TYPE ) == r.getHeaders().end() ) {
        r.setContentType( "text/plain" );
    }

    r.headersComplete();

    if ( _response_str.empty() ) {
        r.out() << what() << "\n\n";
    } else {
        r.out() << _response_str << "\n\n";
    }

    r.notifyComplete();
}


//-------------------------------------------------------------------------
// class MethodNotAllowed

MethodNotAllowed::MethodNotAllowed( const http::Methods& allowed_methods )
    : Error( http::Status::MethodNotAllowed, "method not allowed" ),
      _methods(http::formatMethods( allowed_methods ))
{
    // Nothing to do
}


void MethodNotAllowed::_setHeaders( Response& r ) const
{
    r.setHeader( "Allow", _methods );
}


//-------------------------------------------------------------------------
// class UnsupportedMediaType


UnsupportedMediaType::UnsupportedMediaType( const std::string& content_type )
    : Error(
            http::Status::UnsupportedMediaType,
            string() + "unexpected content type " + content_type
        )
{
    // Nothing to do.
}


//-------------------------------------------------------------------------
// class NotFound


void NotFound::_setHeaders( Response& r ) const
{
    r.setContentTypeJson();
}


} } } // namespace capena::server::exception
