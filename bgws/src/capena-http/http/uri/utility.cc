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


#include "utility.hpp"

#include <boost/bind.hpp>

#include <boost/spirit/include/classic.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <ctype.h>
#include <stdint.h>


using boost::bind;

using boost::spirit::classic::anychar_p;
using boost::spirit::classic::ch_p;
using boost::spirit::classic::parse;
using boost::spirit::classic::parse_info;
using boost::spirit::classic::rule;
using boost::spirit::classic::xdigit_p;

using std::string;


namespace capena {
namespace http {
namespace uri {


static void handleNonEscapedChars(
        std::string& str_in_out,
        const char* start,
        const char* end
    )
{
    str_in_out += std::string( start, end );
}


static void handleEscapedSpace(
        std::string& str_in_out
    )
{
    str_in_out += ' ';
}


static void handleEscapedHexSequence(
        std::string& str_in_out,
        const char* start,
        const char* end
    )
{
    std::string hex_str( start, end );

    uint16_t i;

    std::istringstream iss( hex_str );
    iss >> std::hex >> i;

    str_in_out += char(i);
}


static bool isSafeUrlChar( char c )
{
    // This is all based on RFC1738, section 2.2, see the last sentence where it says these are the safe ones
    // Except it says '+' is safe, but that's the character for ' '.

    if ( isalnum( c ) ||
         c == '$' ||
         c == '-' ||
         c == '_' ||
         c == '.' ||
         c == '!' ||
         c == '*' ||
         c == '\'' ||
         c == '(' ||
         c == ')' ||
         c == ','
       )
    {
        return true;
    }

    return false;
}


void escTo(
        const std::string& str,
        std::ostream& os
    )
{
    for ( std::string::const_iterator i(str.begin()) ; i != str.end() ; ++i ) {
        if ( ! isSafeUrlChar( *i ) ) {
            os << '%' << std::hex << uint16_t(*i);
            continue;
        }

        os << *i;
    }
}


std::string escape( const std::string& part )
{
    std::ostringstream oss;

    escTo( part, oss );

    return oss.str();
}


std::string unescape( const std::string& part )
{
    string cur_str;

    rule<> r = *((anychar_p - ch_p('%'))[bind( &handleNonEscapedChars, boost::ref(cur_str), _1, _2 )] |
                 (ch_p('%') >> (xdigit_p >> xdigit_p)[bind( &handleEscapedHexSequence, boost::ref(cur_str), _1, _2 )])
                );

    parse_info<const char*> p_info(parse( part.c_str(), r ));

    if ( ! p_info.full ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( string() + "invalid escape sequence in '" + part + "'" ) );
    }

    return cur_str;
}


namespace form {


void encodeTo(
        const std::string& str,
        std::ostream& os
    )
{
    for ( std::string::const_iterator i(str.begin()) ; i != str.end() ; ++i ) {
        if ( *i == ' ' ) {
            os << '+';
            continue;
        }

        if ( ! isSafeUrlChar( *i ) ) {
            os << '%' << std::hex << uint16_t(*i);
            continue;
        }

        os << *i;
    }
}


std::string encode( const std::string& part )
{
    std::ostringstream oss;

    encodeTo( part, oss );

    return oss.str();
}


std::string decode( const std::string& part )
{
    string cur_str;

    rule<> r = *((anychar_p - ch_p('+') - ch_p('%'))[bind( &handleNonEscapedChars, boost::ref(cur_str), _1, _2 )] |
                 ch_p('+')[bind( &handleEscapedSpace, boost::ref(cur_str) )] |
                 (ch_p('%') >> (xdigit_p >> xdigit_p)[bind( &handleEscapedHexSequence, boost::ref(cur_str), _1, _2 )])
                );

    parse_info<const char*> p_info(parse( part.c_str(), r ));

    if ( ! p_info.full ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( string() + "invalid escape sequence in '" + part + "'" ) );
    }

    return cur_str;
}


} // namespace form


} } } // namespace capena::http:uri
