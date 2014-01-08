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

#include "Request.hpp"

#include "AbstractResponder.hpp"
#include "exception.hpp"

#include "../http/MediaType.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <string>
#include <vector>


using boost::lexical_cast;

using std::invalid_argument;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "capena-http" );


namespace capena {
namespace server {


Request::Request(
        const std::string& request_line
    ) :
        _content_length(-1),
        _expects_100_continue(false),
        _is_complete(true)
{
    // split the line into Method Request-URI HTTP-Version

    vector<string> parts;
    boost::split( parts, request_line, boost::is_any_of(" ") );

    if ( parts.size() < 3 ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( string() + "error creating Request, unexpected format for request line: " + request_line ) );
        return;
    }

    const string &method_str = parts[0];
    _url_str = parts[1];
    const string &http_version(parts[2]);

    LOG_DEBUG_MSG( "Request method: " << method_str << " uri: " << _url_str << " http_version: " << http_version );

    if ( http_version != http::PROTOCOL_VERSION ) {
        BOOST_THROW_EXCEPTION( exception::Error(
                http::Status::HTTPVersionNotSupported,
                string() + "invalid HTTP version " + http_version
            ) );
    }

    _method = lexical_cast<http::Method>( method_str );

    _url_ptr.reset( new http::uri::Uri(http::uri::Uri::parse( _url_str )) );
}


const http::MediaType& Request::getContentType() const
{
    if ( _content_type_ptr )  return *_content_type_ptr;

    static const http::MediaType DEFAULT_MEDIA_TYPE;
    return DEFAULT_MEDIA_TYPE;
}

const std::string& Request::getRange() const
{
    static const string NO_RANGE;

    Headers::const_iterator i(_headers.find( http::header_norm::RANGE ));
    if ( i == _headers.end() ) {
        return NO_RANGE;
    }

    return i->second;
}


const std::string& Request::getTransferCoding() const
{
    static const string NO_TRANSFER_CODING;

    Headers::const_iterator i(_headers.find( http::header_norm::TRANSFER_ENCODING ));
    if ( i == _headers.end() ) {
        return NO_TRANSFER_CODING;
    }

    return i->second;
}


bool Request::isComplete() const
{
    return _is_complete;
}


void Request::postHeader(
        const std::string& name_norm,
        const std::string& value
    )
{
    if ( name_norm == http::header_norm::CONTENT_TYPE ) {
        _content_type_ptr.reset( new http::MediaType(http::MediaType::parse( value )) );
    } else if ( name_norm == http::header_norm::CONTENT_LENGTH ) {
        _content_length = boost::lexical_cast<int64_t>( value );
        _is_complete = false;
    } else if ( name_norm == http::header_norm::EXPECT ) {
        if ( value == "100-continue" )  _expects_100_continue = true;
    } else if ( name_norm == http::header_norm::TRANSFER_ENCODING ) {
        if ( value == http::TRANSFER_CODING_CHUNKED ) {
            _is_complete = false;
        }
    }

    _headers[name_norm] = value;
}


void Request::postData(
        const std::string& data,
        DataContinuesIndicator data_continues
    )
{
    if ( ! _body_opt ) {
        _body_opt = data;
    } else {
        *_body_opt += data;
    }
    _is_complete = (data_continues == DataContinuesIndicator::END_OF_DATA);
}


Request::~Request()
{
    LOG_DEBUG_MSG( "Destroying Request" );
}


} } // namespace capena::server
