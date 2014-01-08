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


/*! \page requestDataErrors Request Data Errors

These errors may occur when the server is initially checking the request.

HTTP status: 400 Bad Request
- noData : The request has a method of PUT or POST but doesn't have a body.
- invalidJson : The request body doesn't contain a valid JSON document.

HTTP status: 415 Unsupported Media Type
- If the Content-Type header is present, it must have application/json.

 */


#include "AbstractResponder.hpp"

#include "Error.hpp"
#include "ServerStats.hpp"

#include "utility/utility.hpp"

#include "capena-http/http/MediaType.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Request.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using boost::lexical_cast;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


AbstractResponder::AbstractResponder(
        CtorArgs& args
    ) :
        capena::server::AbstractResponder( args.request_ptr ),
        _requested_resource_path(args.requested_resource_path),
        _user_info(args.user_info),
        _dynamic_configuration_ptr(args.dynamic_configuration_ptr),
        _server_stats(args.server_stats),
        _security_enforcer(_dynamic_configuration_ptr->getSecurityEnforcer()),
        _start_time(boost::posix_time::microsec_clock::local_time())
{
    _server_stats.notifyNewRequest(
            this,
            ServerStats::RequestData::create(
                    _getRequest().getMethod(),
                    _getRequest().getUrlStr(),
                    _start_time,
                    _user_info
                )
        );
}


void AbstractResponder::_checkContentType()
{
    const capena::server::Request &request(_getRequest());

    capena::server::Headers::const_iterator i(request.getHeaders().find( capena::http::header_norm::CONTENT_TYPE ));

    if ( i == request.getHeaders().end() ) {
        LOG_DEBUG_MSG( "Client didn't send Content-Type, assuming application/json" );
        return;
    }

    if ( request.getContentType().equivalent( capena::http::MediaType::JSON ) ) {
        return;
    }

    LOG_WARN_MSG( "Client sent unexpected content type '" << i->second << "'." );

    BOOST_THROW_EXCEPTION(
            capena::server::exception::UnsupportedMediaType( i->second )
        );
}


void AbstractResponder::_handleError( std::exception& e )
{
    {
        Error *error_p(dynamic_cast<Error*>( &e ));
        if ( error_p ) {
            error_p->updateResponse( _getResponse() );
            return;
        }
    }

    _getResponse().setException( e );
}


void AbstractResponder::_postEmptyResult()
{
    _getStrand().post( boost::bind(
            &AbstractResponder::_emptyResult, this,
            capena::server::AbstractResponder::shared_from_this()
        ) );
}


void AbstractResponder::_inCatchPostCurrentExceptionToHandlerFn()
{
    _getStrand().post( boost::bind(
            &AbstractResponder::_handleErrorCb, this,
            capena::server::AbstractResponder::shared_from_this(),
            std::current_exception()
        ) );
}


void AbstractResponder::_processRequest()
{
    const auto &request(_getRequest());

    if ( ! request.isComplete() ) {
        LOG_DEBUG_MSG( "Waiting for request data to arrive..." );
        return;
    }

    string range_str;
    if ( ! request.getRange().empty() ) {
        range_str = string() + " range:" + request.getRange();
    }

    LOG_DEBUG_MSG(
            request.getMethod() << " " <<
            request.getUrl().calcString() <<
            range_str <<
            " [" << _user_info.getUserId().getUser() << "]"
        );

    _handle();
}


bool AbstractResponder::_userHasHardwareRead() const
{
    if ( _isUserAdministrator() ) {
        LOG_DEBUG_MSG( "User has authority because they are administrator." );
        return true;
    }

    if ( ! _isUserAuthenticated() ) {
        LOG_DEBUG_MSG( "User doesn't have authority because not authenticated." );
        return false;
    }

    if ( _getEnforcer().validate(
                 hlcs::security::Object( hlcs::security::Object::Hardware, string() ),
                 hlcs::security::Action::Read,
                 _getRequestUserId()
             )
       )
    {
        LOG_DEBUG_MSG( "Enforcer indicates user has authority." );
        return true;
    }

    LOG_DEBUG_MSG( "Enforcer did not give user authority." );
    return false;
}


void AbstractResponder::_handle()
{
    const capena::server::Request &request(_getRequest());

    if ( request.getMethod() == capena::http::Method::DELETE ) {

        _doDelete();

    } else if ( request.getMethod() == capena::http::Method::GET ) {

        try {

            _doGet();

        } catch ( boost::program_options::multiple_occurrences& me ) {

            BOOST_THROW_EXCEPTION( capena::server::exception::BadRequest( "Multiple occurrences of an option is not allowed" ) );

        }

    } else if ( request.getMethod() == capena::http::Method::HEAD ) {

        _doHead();

    } else if ( request.getMethod() == capena::http::Method::PUT || request.getMethod() == capena::http::Method::POST ) {

        const auto &req_body_opt(request.getBodyOpt());

        if ( ! req_body_opt ) {

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Invalid " + lexical_cast<string>( request.getMethod() ) + " request does not contain a body.",
                    "processRequest", "noData", Error::Data(),
                    capena::http::Status::BadRequest
                ) );

        }

        _checkContentType(); // Throws if the content type is not application/json.

        const std::string &req_body(*req_body_opt);

        json::ConstValuePtr val_ptr;

        try {

            val_ptr = json::Parser()( req_body );

        } catch ( std::exception& e ) {

            LOG_WARN_MSG( "Received invalid JSON document. document:'" << req_body.substr( 0, 100 ) << "'" );

            BOOST_THROW_EXCEPTION( Error(
                    "Invalid request contains invalid JSON.",
                    "processRequest", "invalidJson", Error::Data(),
                    capena::http::Status::BadRequest
                ) );

        }

        if ( request.getMethod() == capena::http::Method::PUT ) {
            _doPut( val_ptr );
        } else {
            _doPost( val_ptr );
        }

    } else {
        BOOST_THROW_EXCEPTION( capena::server::exception::MethodNotAllowed( _getAllowedMethods() ) );
    }
}


AbstractResponder::~AbstractResponder()
{
    string range_str;
    if ( ! _getRequest().getRange().empty() ) {
        range_str = string() + " range:" + _getRequest().getRange();
    }

    boost::posix_time::ptime end_time(boost::posix_time::microsec_clock::local_time());

    boost::posix_time::time_duration time_to_process_request(end_time - _start_time);

    LOG_INFO_MSG(
            _getRequest().getMethod() << " " <<
            _getRequest().getUrl().calcString() <<
            range_str <<
            " [" << _user_info.getUserId().getUser() << "]"
            " '" << capena::http::getText( _getResponse().getStatus() ) << "'"
            " " << (time_to_process_request.total_microseconds() / 1000000.0) << "sec"
        );

    LOG_DEBUG_MSG( "Responder complete. Time: " << time_to_process_request.total_microseconds() << " us" );

    _server_stats.notifyRequestComplete(
            this,
            time_to_process_request
        );
}


void AbstractResponder::_throwMethodNotAllowed()
{
    BOOST_THROW_EXCEPTION( capena::server::exception::MethodNotAllowed( _getAllowedMethods() ) );
}


void AbstractResponder::_emptyResult(
        capena::server::ResponderPtr
    )
{
    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( json::ArrayValue(), response.out() );
}


void AbstractResponder::_handleErrorCb(
        capena::server::ResponderPtr,
        std::exception_ptr exc_ptr
    )
{
    try {
        std::rethrow_exception( exc_ptr );
    } catch ( std::exception& e )
    {
        _handleError( e );
    }
}


} // namespace bgws
