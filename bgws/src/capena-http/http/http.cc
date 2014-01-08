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

#include "http.hpp"

#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>


using std::ostringstream;
using std::string;


namespace capena {
namespace http {


static const string OPTIONS_STR( "OPTIONS" );
static const string GET_STR( "GET" );
static const string HEAD_STR( "HEAD" );
static const string POST_STR( "POST" );
static const string PUT_STR( "PUT" );
static const string DELETE_STR( "DELETE" );
static const string TRACE_STR( "TRACE" );
static const string CONNECT_STR( "CONNECT" );


std::istream& operator>>( std::istream& is, Method& m )
{
    string str;
    is >> str;
    if ( str == OPTIONS_STR )  m = Method::OPTIONS;
    else if ( str == GET_STR )  m = Method::GET;
    else if ( str == HEAD_STR )  m = Method::HEAD;
    else if ( str == POST_STR )  m = Method::POST;
    else if ( str == PUT_STR )  m = Method::PUT;
    else if ( str == DELETE_STR )  m = Method::DELETE;
    else if ( str == TRACE_STR )  m = Method::POST;
    else if ( str == CONNECT_STR )  m = Method::CONNECT;
    else {
        is.setstate( std::ios::failbit );
    }
    return is;
}


std::ostream& operator<<( std::ostream& os, Method m )
{
    switch ( m ) {
    case Method::OPTIONS: os << OPTIONS_STR; break;
    case Method::GET: os << GET_STR; break;
    case Method::HEAD: os << HEAD_STR; break;
    case Method::POST: os << POST_STR; break;
    case Method::PUT: os << PUT_STR; break;
    case Method::DELETE: os << DELETE_STR; break;
    case Method::TRACE: os << TRACE_STR; break;
    case Method::CONNECT: os << CONNECT_STR; break;
    }
    return os;
}


std::string formatMethods( const Methods& methods )
{
    ostringstream oss;
    for ( http::Methods::const_iterator i(methods.begin()) ; i != methods.end() ; ++i ) {
        if ( i != methods.begin() ) {
            oss << ", ";
        }
        oss << *i;
    }
    return oss.str();
}


std::string getText( Status s )
{
    ostringstream oss;
    oss << int(s);

    switch ( s ) {
    case Status::Continue:
        oss << " Continue";
        break;
    case Status::SwitchingProtocols:
        oss << " Switching Protocols";
        break;
    case Status::OK:
        oss << " OK";
        break;
    case Status::Created:
        oss << " Created";
        break;
    case Status::Accepted:
        oss << " Accepted";
        break;
    case Status::NonAuthoritativeInformation:
        oss << " Non-Authoritative Information";
        break;
    case Status::NoContent:
        oss << " No Content";
        break;
    case Status::ResetContent:
        oss << " Reset Content";
        break;
    case Status::PartialContent:
        oss << " Partial Content";
        break;
    case Status::MultiStatus:
        oss << " Multi Status";
        break;
    case Status::MultipleCoices:
        oss << " Multiple Choices";
        break;
    case Status::MovedPermanently:
        oss << " Moved Permanently";
        break;
    case Status::Found:
        oss << " Found";
        break;
    case Status::SeeOther:
        oss << " See Other";
        break;
    case Status::NotModified:
        oss << " Not Modified";
        break;
    case Status::UseProxy:
        oss << " Use Proxy";
        break;
    case Status::SwitchProxy:
        oss << " Switch Proxy";
        break;
    case Status::TemporaryRedirect:
        oss << " Temporary Redirect";
        break;
    case Status::BadRequest:
        oss << " Bad Request";
        break;
    case Status::Unauthorized:
        oss << " Unauthorized";
        break;
    case Status::PaymentRequired:
        oss << " Payment Required";
        break;
    case Status::Forbidden:
        oss << " Forbidden";
        break;
    case Status::NotFound:
        oss << " Not Found";
        break;
    case Status::MethodNotAllowed:
        oss << " Method Not Allowed";
        break;
    case Status::NotAcceptable:
        oss << " Not Acceptable";
        break;
    case Status::ProxyAuthenticationRequired:
        oss << " Proxy Authentication Required";
        break;
    case Status::RequestTimeout:
        oss << " Request Timeout";
        break;
    case Status::Conflict:
        oss << " Conflict";
        break;
    case Status::Gone:
        oss << " Gone";
        break;
    case Status::LengthRequired:
        oss << " Length Required";
        break;
    case Status::PreconditionFailed:
        oss << " Precondition Failed";
        break;
    case Status::RequestEntityTooLarge:
        oss << " Request Entity Too Large";
        break;
    case Status::RequestURITooLong:
        oss << " Request-URI Too Long";
        break;
    case Status::UnsupportedMediaType:
        oss << " Unsupported Media Type";
        break;
    case Status::RequestedRangeNotSatisfiable:
        oss << " Requested Range Not Satisfiable";
        break;
    case Status::ExpectationFailed:
        oss << " Expectation Failed";
        break;
    case Status::ImATeapot:
        oss << " I'm a Teapot";
        break;
    case Status::UnprocessableEntity:
        oss << " Unprocessable Entity";
        break;
    case Status::Locked:
        oss << " Locked";
        break;
    case Status::FailedDependency:
        oss << " Failed Dependency";
        break;
    case Status::UnorderedCollection:
        oss << " Unordered Collection";
        break;
    case Status::UpgradeRequired:
        oss << " Upgrade Required";
        break;
    case Status::RetryWith:
        oss << " Retry With";
        break;
    case Status::BlockedByWindowsParentalControls:
        oss << " Blocked By Windows Parental Controls";
        break;
    case Status::InternalServerError:
        oss << " Internal Server Error";
        break;
    case Status::NotImplemented:
        oss << " Not Implemented";
        break;
    case Status::BadGateway:
        oss << " Bad Gateway";
        break;
    case Status::ServiceUnavailable:
        oss << " Service Unavailable";
        break;
    case Status::GatewayTimeout:
        oss << " Gateway Timeout";
        break;
    case Status::HTTPVersionNotSupported:
        oss << " HTTP Version Not Supported";
        break;
    case Status::VariantAlsoNegotiates:
        oss << " Variant Also Negotiates";
        break;
    case Status::InsufficientStorage:
        oss << " Insufficient Storage";
        break;
    case Status::BandwidthLimitExceeded:
        oss << " Bandwidth Limit Exceeded";
        break;
    case Status::NotExtended:
        oss << " Not Extended";
        break;

    default:
        if ( unsigned(s) >= 100 && unsigned(s) < 200 ) {
            oss << " Informational";
        } else if ( unsigned(s) < 300 ) {
            oss << " Successful";
        } else if ( unsigned(s) < 400 ) {
            oss << " Redirection";
        } else if ( unsigned(s) < 500 ) {
            oss << " Client Error";
        } else if ( unsigned(s) < 600 ) {
            oss << " Server Error";
        } else {
            // Do nothing.
        }
        break;
    }

    return oss.str();
}


std::ostream& operator<<( std::ostream& os, Status s )
{
    os << "'" << getText( s ) << "'";
    return os;
}


bool isSpHt( int c )
{
    return ((c == SP) || (c == HT));
}

const std::string PROTOCOL_VERSION("HTTP/1.1");
const std::string CONTINUE_RESPONSE_ENTITY(PROTOCOL_VERSION + " " + getText( Status::Continue ) + CRLF + CRLF);
const std::string TRANSFER_CODING_CHUNKED("chunked");


std::string formatHeaderLine( const std::string& name, const std::string& value )
{
    return (name + ": " + value + CRLF);
}


void parseHeaderLine(
        const std::string& header_line,
        bool *is_continuation_out,
        std::string *name_norm_out,
        std::string *value_out
    )
{
    if ( isSpHt( header_line[0] ) ) {
        // It's a continuation line.

        *is_continuation_out = true;
        *value_out = boost::algorithm::trim_left_copy_if( header_line, &isSpHt ); // append after trimming off leading SP & HT
        return;

    }

    *is_continuation_out = false;

    string::size_type split_loc(header_line.find( ':' ));

    // RFC 2616, Section 2.2, says the header value can have a continuation line but doesn't say the name can.

    if ( split_loc == string::npos ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( string() + "Header line didn't have a separator, line='" + header_line + "'" ) );
    }

    *name_norm_out = boost::algorithm::to_upper_copy( header_line.substr( 0, split_loc ) ); // Normalize header name by uppercasing it.
    *value_out = boost::algorithm::trim_left_copy( header_line.substr( split_loc + 1 ) ); // Remove any extra whitespace from front of the header line.
}


} } // namespace capena::http
