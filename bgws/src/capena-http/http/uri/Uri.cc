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


#include "Uri.hpp"

#include <boost/optional.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdexcept>


using std::string;


namespace capena {
namespace http {
namespace uri {


/*! \brief (Internal static stuff used in URI parsing, generation) */
namespace statics {

static const char SCHEME_SEPARATOR(':');
static const char QUERY_SEPARATOR('?');

static const string HTTP_SCHEME("http");
static const string HTTPS_SCHEME("https");

static const string HTTP_SCHEME_NORM("HTTP");
static const string HTTPS_SCHEME_NORM("HTTPS");

static const string HTTP_PREFIX(HTTP_SCHEME + SCHEME_SEPARATOR + "//");
static const string HTTPS_PREFIX(HTTPS_SCHEME + SCHEME_SEPARATOR + "//");


typedef boost::optional< std::pair<std::string,std::string> > StringPairOpt;

StringPairOpt splitAt( const std::string& str, char c )
{
    string::size_type split_pos(str.find( c ));
    if ( split_pos == string::npos ) {
        return StringPairOpt();
    }

    return StringPairOpt( std::make_pair( str.substr( 0, split_pos ), str.substr( split_pos + 1 ) ) );
}


std::pair<Path,Query> parsePathQuery( const std::string& str )
{
    StringPairOpt spo(splitAt( str, statics::QUERY_SEPARATOR ));

    if ( ! spo ) {
        return std::make_pair( Path( str ), Query() );
    }

    return std::make_pair( Path( spo->first ), Query::parse( spo->second ) );
}


Uri parseAbsolute( const std::string& abs_str, std::string::size_type c_pos )
{
    string scheme_norm(boost::algorithm::to_upper_copy( abs_str.substr( 0, c_pos ) ));

    Uri::Security security;

    if ( scheme_norm == statics::HTTP_SCHEME_NORM ) {
        security = Uri::Security::NotSecure;
    } else if ( scheme_norm == statics::HTTPS_SCHEME_NORM ) {
        security = Uri::Security::Secure;
    } else {
        BOOST_THROW_EXCEPTION( std::invalid_argument( "failed to parse URI '" + abs_str + "', invalid scheme" ) );
    }


    string after_colon(abs_str.substr( c_pos + 1 ) );

    if ( after_colon.size() < 2 ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( "failed to parse URI '" + abs_str + "'" ) );
    }

    string two_slashes(after_colon.substr( 0, 2 ) );

    if ( two_slashes != "//" ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( "failed to parse URI '" + abs_str + "'" ) );
    }


    string net_path_query(after_colon.substr( 2 ));

    string::size_type hostname_end_pos(net_path_query.find( '/' ));

    if ( hostname_end_pos == string::npos ) { // no path
        string hostname_str(net_path_query);

        Uri ret(
                security,
                hostname_str
            );

        return ret;
    }

    // has path & query part.

    string hostname_str(net_path_query.substr( 0, hostname_end_pos ));

    string path_query_str(net_path_query.substr( hostname_end_pos ));

    auto path_query(parsePathQuery( path_query_str ));

    Uri ret(
            security,
            hostname_str,
            path_query.first,
            path_query.second
        );

    return ret;
}


Uri parseRelative( const std::string& relative_str )
{
    std::pair<Path,Query> path_query(parsePathQuery( relative_str ));

    Uri ret(
            path_query.first,
            path_query.second
        );

    return ret;
}


} // namespace statics


Uri Uri::parse( const std::string& uri_str )
{
    if ( uri_str.empty() ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( "failed to parse URI '" + uri_str + "'" ) );
    }

    string::size_type c_pos(uri_str.find( statics::SCHEME_SEPARATOR ));

    if ( c_pos == string::npos ) {
        return statics::parseRelative( uri_str );
    }

    return statics::parseAbsolute( uri_str, c_pos );
}


Uri::Uri(
        const Path& path,
        const Query& query
    ) :
        _is_absolute(false),
        _path(path),
        _query(query)
{
    // Nothing to do
}


Uri::Uri(
        const std::string& host_port,
        const Path& path,
        const Query& query
    ) :
        _is_absolute(true),
        _security(Security::NotSecure),
        _host_port(host_port),
        _path(path),
        _query(query)
{
    // Nothing to do.
}


Uri::Uri(
        Security security,
        const std::string& host_port,
        const Path& path,
        const Query& query
    ) :
        _is_absolute(true),
        _security(security),
        _host_port(host_port),
        _path(path),
        _query(query)
{
    // Nothing to do.
}


std::string Uri::calcString() const
{
    if ( _is_absolute ) {
        return ((_security == Security::Secure ? statics::HTTPS_PREFIX : statics::HTTP_PREFIX) + _host_port + _path.toString() + _query.calcString());
    } else {
        return (_path.toString() + _query.calcString());
    }
}


std::ostream& operator<<( std::ostream& os, Uri::Security security )
{
    os << (security == Uri::Security::Secure ? "secure" : "notSecure");
    return os;
}

} } } // namespace capena::http::uri
