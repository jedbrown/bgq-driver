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

#include "Path.hpp"

#include "utility.hpp"

#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <stdexcept>


using std::ostringstream;
using std::string;


namespace capena {
namespace http {
namespace uri {


Path::Path( const std::string& escaped_url_string )
{
    if ( escaped_url_string.empty() )  return; // Nothing to do.


    // Strip off initial / if it's there, should have some indication that it was there.
    string to_use;

    if ( (! escaped_url_string.empty()) && escaped_url_string[0] == '/' ) {

        to_use = escaped_url_string.substr( 1 );

    } else {

        to_use = escaped_url_string;

    }


    // Split the path into parts on /s. Unescape each of the parts.
    std::vector<std::string> parts;

    boost::algorithm::split( parts, to_use, boost::is_any_of( "/" ) );

    try {
        BOOST_FOREACH( const string& part, parts ) {
            push_back( unescape( part ) );
        }
    } catch ( std::invalid_argument& e ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( string() + "invalid url path, " + e.what() ) );
    }
}


Path::Path(
        std::vector<std::string>::const_iterator start,
        std::vector<std::string>::const_iterator end
    ) :
        std::vector<std::string>( start, end )
{
    // Nothing to do.
}


std::string Path::toString() const
{
    ostringstream ret;
    ret << *this;
    return ret.str();
}


Path Path::operator/( const std::string& new_part ) const
{
    Path ret(*this);
    ret /= new_part;
    return ret;
}


Path Path::operator/( const Path& other ) const
{
    Path ret(*this);
    for ( const_iterator i(other.begin()) ; i != other.end() ; ++i ) {
        ret /= *i;
    }
    return ret;
}


Path& Path::operator/=( const std::string& new_part )
{
    push_back( new_part );
    return *this;
}


Path Path::calcParent() const
{
    if ( empty() ) { return Path(); }
    return Path( begin(), end() - 1 );
}


bool Path::isDescendantOf( const Path& other ) const
{
    // The other must be shorter than me.
    if ( other.size() >= size() )  return false;

    // All of the parts in the other must be in mine.
    return std::equal( other.begin(), other.end(), begin() );
}


bool Path::isAncestorOf( const Path& other ) const
{
    return other.isDescendantOf( *this );
}


bool Path::isChildOf( const Path& other ) const
{
    // The other must be 1 shorter than me.
    if ( (other.size() + 1) != size() )  return false;

    // All of the parts in the other must be in mine.
    return std::equal( other.begin(), other.end(), begin() );
}


bool Path::isParentOf( const Path& other ) const
{
    return other.isChildOf( *this );
}


void Path::_handlePathPartComplete( std::string& new_part_in_out )
{
    push_back( new_part_in_out );
    new_part_in_out.clear();
}


std::ostream& operator<<( std::ostream& os, const Path& url_path )
{
    BOOST_FOREACH( const Path::value_type& part, url_path ) {
        os << "/";
        escTo( part, os );
    }

    return os;
}


} } } // namespace capena::http::uri
