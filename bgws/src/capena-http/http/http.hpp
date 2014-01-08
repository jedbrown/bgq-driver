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

#ifndef CAPENA_HTTP_HTTP_HPP_
#define CAPENA_HTTP_HTTP_HPP_


#include <iosfwd>
#include <string>
#include <vector>


namespace capena {
namespace http {


enum class Method
{
    OPTIONS,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT
};

std::istream& operator>>( std::istream& is, Method& m );

std::ostream& operator<<( std::ostream& os, Method m );


typedef std::vector<Method> Methods;


std::string formatMethods( const Methods& methods );


enum class StatusClass
{
    Informational = 1,
    Successful = 2,
    Redirection = 3,
    ClientError = 4,
    ServerError = 5
};


enum class Status {
    Continue = 100,
    SwitchingProtocols = 101,

    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultiStatus = 207,

    MultipleCoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    UseProxy = 305,
    SwitchProxy = 306,
    TemporaryRedirect = 307,

    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    RequestEntityTooLarge = 413,
    RequestURITooLong = 414,
    UnsupportedMediaType = 415,
    RequestedRangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    ImATeapot = 418,
    UnprocessableEntity = 422,
    Locked = 423,
    FailedDependency = 424,
    UnorderedCollection = 425,
    UpgradeRequired = 426,
    RetryWith = 449,
    BlockedByWindowsParentalControls = 450,

    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    InsufficientStorage = 507,
    BandwidthLimitExceeded = 509,
    NotExtended = 510

};


std::string getText( Status s );


std::ostream& operator<<( std::ostream& os, Status s );


inline StatusClass codeToClass( Status s )  { return StatusClass( unsigned(s) / 100 ); }


/*! \brief HTTP header field names */
namespace header {

// general
const std::string CACHE_CONTROL( "Cache-Control" );
const std::string CONNECTION( "Connection" );
const std::string DATE( "Date" );
const std::string PRAGMA( "Pragma" );
const std::string TRAILER( "Trailer" );
const std::string TRANSFER_ENCODING( "Transfer-Encoding" );
const std::string UPGRADE( "Upgrade" );
const std::string VIA( "Via" );
const std::string WARNING( "Warning" );

// request
const std::string ACCEPT( "Accept" );
const std::string ACCEPT_CHARSET( "Accept-Charset" );
const std::string ACCEPT_ENCODING( "Accept-Encoding" );
const std::string ACCEPT_LANGUAGE( "Accept-Language" );
const std::string AUTHORIZATION( "Authorization" );
const std::string EXPECT( "Expect" );
const std::string FROM( "From" );
const std::string HOST( "Host" );
const std::string IF_MATCH( "If-Match" );
const std::string IF_MODIFIED_SINCE( "If-Modified-Since" );
const std::string IF_NONE_MATCH( "If-None-Match" );
const std::string IF_RANGE( "If-Range" );
const std::string IF_UNMODIFIED_SINCE( "If-Unmodified-Since" );
const std::string MAX_FORWARDS( "Max-Forwards" );
const std::string PROXY_AUTHORIZATION( "Proxy-Authorization" );
const std::string RANGE( "Range" );
const std::string REFERER( "Referer" );
const std::string TE( "TE" );
const std::string USER_AGENT( "User-Agent" );

// response
const std::string ACCEPT_RANGES( "Accept-Ranges" );
const std::string AGE( "Age" );
const std::string ETAG( "ETag" );
const std::string LOCATION( "Location" );
const std::string PROXY_AUTHENTICATE( "Proxy-Authenticate" );
const std::string RETRY_AFTER( "Retry-After" );
const std::string SERVER( "Server" );
const std::string VARY( "Vary" );
const std::string WWW_AUTHENTICATE( "WWW-Authenticate" );

// entity
const std::string ALLOW( "Allow" );
const std::string CONTENT_ENCODING( "Content-Encoding" );
const std::string CONTENT_LANGUAGE( "Content-Language" );
const std::string CONTENT_LENGTH( "Content-Length" );
const std::string CONTENT_MD5( "Content-MD5" );
const std::string CONTENT_RANGE( "Content-Range" );
const std::string CONTENT_TYPE( "Content-Type" );
const std::string EXPIRES( "Expires" );
const std::string LAST_MODIFIED( "Last-Modified" );

}


/*! \brief HTTP header field names normalized to uppercase. */
namespace header_norm {

// general
const std::string CACHE_CONTROL( "CACHE-CONTROL" );
const std::string CONNECTION( "CONNECTION" );
const std::string DATE( "DATE" );
const std::string PRAGMA( "PRAGMA" );
const std::string TRAILER( "TRAILER" );
const std::string TRANSFER_ENCODING( "TRANSFER-ENCODING" );
const std::string UPGRADE( "UPGRADE" );
const std::string VIA( "VIA" );
const std::string WARNING( "WARNING" );

// request
const std::string ACCEPT( "ACCEPT" );
const std::string ACCEPT_CHARSET( "ACCEPT-CHARSET" );
const std::string ACCEPT_ENCODING( "ACCEPT-ENCODING" );
const std::string ACCEPT_LANGUAGE( "ACCEPT-LANGUAGE" );
const std::string AUTHORIZATION( "AUTHORIZATION" );
const std::string EXPECT( "EXPECT" );
const std::string FROM( "FROM" );
const std::string HOST( "HOST" );
const std::string IF_MATCH( "IF-MATCH" );
const std::string IF_MODIFIED_SINCE( "IF-MODIFIED-SINCE" );
const std::string IF_NONE_MATCH( "IF-NONE-MATCH" );
const std::string IF_RANGE( "IF-RANGE" );
const std::string IF_UNMODIFIED_SINCE( "IF-UNMODIFIED-SINCE" );
const std::string MAX_FORWARDS( "MAX-FORWARDS" );
const std::string PROXY_AUTHORIZATION( "PROXY-AUTHORIZATION" );
const std::string RANGE( "RANGE" );
const std::string REFERER( "REFERER" );
const std::string TE( "TE" );
const std::string USER_AGENT( "USER-AGENT" );

// response
const std::string ACCEPT_RANGES( "ACCEPT-RANGES" );
const std::string AGE( "AGE" );
const std::string ETAG( "ETAG" );
const std::string LOCATION( "LOCATION" );
const std::string PROXY_AUTHENTICATE( "PROXY-AUTHENTICATE" );
const std::string RETRY_AFTER( "RETRY-AFTER" );
const std::string SERVER( "SERVER" );
const std::string VARY( "VARY" );
const std::string WWW_AUTHENTICATE( "WWW-AUTHENTICATE" );

// entity
const std::string ALLOW( "ALLOW" );
const std::string CONTENT_ENCODING( "CONTENT-ENCODING" );
const std::string CONTENT_LANGUAGE( "CONTENT-LANGUAGE" );
const std::string CONTENT_LENGTH( "CONTENT-LENGTH" );
const std::string CONTENT_MD5( "CONTENT-MD5" );
const std::string CONTENT_RANGE( "CONTENT-RANGE" );
const std::string CONTENT_TYPE( "CONTENT-TYPE" );
const std::string EXPIRES( "EXPIRES" );
const std::string LAST_MODIFIED( "LAST-MODIFIED" );

}


/*! \brief HTTP Media type strings. */
namespace media_type {

const std::string JSON( "application/json" );
const std::string XML( "application/xml" );
const std::string PLAIN_TEXT( "text/plain" );

}


const char SP(' ');
const char HT('\x9');
const char CR('\xd');
const char LF('\xa');

const std::string CRLF(std::string() + CR + LF);


bool isSpHt( int c );


extern const std::string PROTOCOL_VERSION;

extern const std::string CONTINUE_RESPONSE_ENTITY;

extern const std::string TRANSFER_CODING_CHUNKED;


std::string formatHeaderLine( const std::string& name, const std::string& value );

/*!
 *  \throws std::invalid_argument if header_line is not a continuation line and doesn't contain a :.
 */
void parseHeaderLine(
        const std::string& header_line,
        bool *is_continuation_out,
        std::string *name_norm_out,
        std::string *value_out
    );


} // namespace capena::http
} // namespace capena


#endif
