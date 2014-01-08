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

#include "bgws.hpp"

#include "Response.hpp"

#include "capena-http/http/http.hpp"

#include "chiron-json/json.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>


using boost::lexical_cast;
using boost::regex;
using boost::regex_match;
using boost::smatch;

using std::cerr;
using std::ostringstream;
using std::runtime_error;
using std::string;


namespace bgws_clients {
namespace bgws {


const capena::http::uri::Path BLOCKS_URL_PATH(capena::http::uri::Path() / "blocks");
const capena::http::uri::Path JOBS_URL_PATH(capena::http::uri::Path() / "jobs");
const capena::http::uri::Path LOGGING_URL_PATH(capena::http::uri::Path() / "bgws" / "logging");
const capena::http::uri::Path SESSIONS_URL_PATH(capena::http::uri::Path() / "bgws" / "sessions");


void checkResponse(
        const Response& response,
        capena::http::Status expected_status
    )
{
    // If the operation was successful then expect a 200 OK response.
    capena::http::StatusClass status_class(capena::http::codeToClass( response.getStatusCode() ));

    if ( status_class == capena::http::StatusClass::Successful ) {
        if ( response.getStatusCode() != expected_status ) {
            cerr << "Note: successful response with status '" << response.getStatusText() << "'.\n";
        }

        if ( ((expected_status != capena::http::Status::NoContent) &&
              (expected_status != capena::http::Status::Created)) &&
             (response.getContentType() != capena::http::media_type::JSON) ) {
            // successful but content-type wasn't JSON ?

            BOOST_THROW_EXCEPTION( runtime_error( string() +
                    "unexpected content type in response. The content type is '" + response.getContentType() + "'"
                ) );
        }

        return;
    }

    // This is some type of error.

    if ( status_class == capena::http::StatusClass::Informational ) {
        // Informational, but don't know how to handle.
        BOOST_THROW_EXCEPTION( runtime_error( string() +
                "don't know how to handle HTTP response '" + response.getStatusText() + "'"
            ) );
    }

    // Check for typical BGWS error response.

    if ( response.getContentType() == capena::http::media_type::JSON ) {
        json::Object err_obj(json::Parser()( response.getData() )->getObject());

        const string &err_text(err_obj.getString( "text" ));

        BOOST_THROW_EXCEPTION( runtime_error( string() +
                "Blue Gene web services response:\n" +
                err_text
            ) );
    }

    // Otherwise, process HTTP error.

    ostringstream oss;

    oss << "failed with HTTP response '" + response.getStatusText() + "'.";

    if ( ! response.getData().empty() ) {
        oss << " Response data:\n" << response.getData();
    }

    BOOST_THROW_EXCEPTION( runtime_error( oss.str() ) );

}


void parseContentRange(
        const std::string& content_range_str,
        uint64_t *start_out,
        uint64_t *end_out,
        uint64_t *count_out
    )
{
    static const regex RE( "\\s*items\\s+(\\d+)-(\\d+)/(\\d+).*" );

    smatch match;

    if ( ! regex_match( content_range_str, match, RE ) ) {
        *start_out = 0;
        *end_out = 0;
        *count_out = 0;

        return;
    }


    *start_out = lexical_cast<uint64_t>( match[1] );
    *end_out = lexical_cast<uint64_t>( match[2] );
    *count_out = lexical_cast<uint64_t>( match[3] );
}


}} // namespace bgws_clients::bgws
