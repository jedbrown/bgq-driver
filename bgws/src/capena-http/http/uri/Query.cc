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

#include "Query.hpp"

#include "utility.hpp"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <stdint.h>


using boost::bind;

using std::invalid_argument;
using std::ostringstream;
using std::string;


namespace capena {
namespace http {
namespace uri {


Query Query::parse( const std::string& query_string_escaped )
{

    try {
        Query ret;

        if ( query_string_escaped.empty() )  return ret;

        // split query_string_escaped on &.

        std::vector<std::string> parts;

        boost::algorithm::split( parts, query_string_escaped, boost::is_any_of( "&" ) );

        BOOST_FOREACH( const string& part, parts ) {

            if ( part.empty() )  continue; // skip empty parts.

            // split part on =.
            string::size_type split_pos(part.find( '=' ));

            if ( split_pos == string::npos ) {

                ret._parameters.push_back( Parameter(
                        form::decode( part ),
                        ""
                    ) );

            } else {

                ret._parameters.push_back( Parameter(
                        form::decode( part.substr(0, split_pos) ),
                        form::decode( part.substr(split_pos+1) )
                    ) );

            }
        }

        return ret;

    } catch ( std::invalid_argument& e )
    {
        BOOST_THROW_EXCEPTION( InvalidQueryStringError( query_string_escaped ) );
    }
}


Query::Query(
        const Parameters& parameters
    ) :
        _parameters(parameters)
{
    // Nothing to do.
}


std::string Query::calcString() const
{
    ostringstream oss;

    bool first(true);

    BOOST_FOREACH( const Parameter& param, _parameters ) {
        if ( param.first.empty() && param.second.empty() )  continue;

        if ( first ) {
            oss << "?";
            first = false;
        } else {
            oss << "&";
        }

        form::encodeTo( param.first, oss );
        oss << "=";
        form::encodeTo( param.second, oss );
    }

    return oss.str();
}


Query::Arguments Query::calcArguments() const
{
    if ( _parameters.empty() )  return Arguments();

    Arguments ret;

    BOOST_FOREACH( const Parameter& parameter, _parameters ) {
        ret.push_back( string() + "--" + parameter.first );
        ret.push_back( parameter.second );
    }

    return ret;
}


InvalidQueryStringError::InvalidQueryStringError( const std::string& query_string_escaped )
    : std::invalid_argument( string() + "invalid query string '" + query_string_escaped + "'" )
{
    // Nothing to do.
}

} } } // namespace capena::http::uri
