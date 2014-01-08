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

#include "job/Shape.h"

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::lexical_cast;

using std::string;

namespace BGQDB {
namespace job {

const Shape Shape::Block;
const Shape Shape::Midplane( 4, 4, 4, 4, 2 );
const Shape Shape::NodeBoard( 2, 2, 2, 2, 2 );
const Shape Shape::SingleNode( 1, 1, 1, 1, 1 );
const Shape Shape::SingleCore( 1, 1, 1, 1, 1, 1 );

Shape::Shape(
        uint16_t a,
        uint16_t b,
        uint16_t c,
        uint16_t d,
        uint16_t e,
        uint16_t cores
    ) :
    _a(a), _b(b), _c(c), _d(d), _e(e), _cores(cores)
{
    // Nothing to do.
}

Shape::Shape(
        const std::string& shape_str
    )
{
    if ( shape_str.empty() ) return;

    if ( boost::iequals( shape_str, "block" ) ) {
        *this = Shape::Block;
        return;
    }
    if ( boost::iequals( shape_str, "MP" ) ) {
        *this = Shape::Midplane;
        return;
    }
    if ( boost::iequals( shape_str, "NB" ) ) {
        *this = Shape::NodeBoard;
        return;
    }
    if ( boost::iequals( shape_str, "node" ) ) {
        *this = Shape::SingleNode;
        return;
    }
    if ( boost::iequals( shape_str, "core" ) ) {
        *this = Shape::SingleCore;
        return;
    }

    // split on x character
    typedef std::vector<string> Tokens;
    Tokens tokens;
    boost::split( tokens, shape_str, boost::is_any_of("x") );

    // ensure we found exactly 5 tokens
    if ( tokens.size() != 5 ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument(
                "expected 5 tokens, found " +
                lexical_cast<string>( tokens.size() )
                ) );
    }

    // iterate through tokens
    try {
        for ( Tokens::iterator i = tokens.begin(); i != tokens.end(); ++i ) {
            ssize_t index = std::distance( tokens.begin(), i );
            switch( index ) {
                case 0: _a = lexical_cast<uint16_t>( *i ); break;
                case 1: _b = lexical_cast<uint16_t>( *i ); break;
                case 2: _c = lexical_cast<uint16_t>( *i ); break;
                case 3: _d = lexical_cast<uint16_t>( *i ); break;
                case 4: _e = lexical_cast<uint16_t>( *i ); break;
                default: BOOST_ASSERT( !"unhandled token" ); break;
            }
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument( string() +
                "invalid shape: " + e.what()
            ) );
    }

    _validate();
}

uint64_t Shape::getSize() const
{
    if ( *this == SingleCore )  return 0;

    return _a * _b * _c * _d * _e;
}

void Shape::_validate()
{
    // validate each token size
    if ( _a == 0 || _b == 0 || _c == 0 || _d == 0 || _e == 0 ) {
        // no dimension can be zero
        BOOST_THROW_EXCEPTION( std::invalid_argument(
                "0 is not a valid size for any dimension"
                ) );
    } else if ( _a > 4 || _b > 4 || _c > 4 || _d > 4 ) {
        // A, B, C, and D must be 4 or less
        BOOST_THROW_EXCEPTION( std::invalid_argument(
                "A, B, C, and D dimensions must be of size 4 or less"
                ) );
    } else if ( _e > 2 ) {
        // E must be 2 or less
        BOOST_THROW_EXCEPTION( std::invalid_argument(
                "E dimension must be of size 2 or less"
                ) );
    }
}

std::istream& operator>>( std::istream& is, Shape& shape_out )
{
    string shape_str;
    is >> shape_str;

    try {
        shape_out = Shape( shape_str );
    } catch( std::exception& e ) {
        is.setstate( std::ios::failbit );
    }

    return is;
}

std::ostream& operator<<( std::ostream& os, const Shape& shape )
{
    return (os << shape.getA() << "x" << shape.getB() << "x" << shape.getC() << "x" << shape.getD() << "x" << shape.getE());
}


} } // namespace BGQDB::job
