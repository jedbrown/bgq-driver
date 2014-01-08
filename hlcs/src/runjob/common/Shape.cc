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
#include "common/Shape.h"

#include "common/logging.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

Shape::Shape(
        const std::string& shape
        ) :
    _a(0),
    _b(0),
    _c(0),
    _d(0),
    _e(0),
    _nodes()
{
    if ( shape.empty() ) return;

    // split on x character
    typedef std::vector<std::string> Tokens;
    Tokens tokens;
    boost::split( tokens, shape, boost::is_any_of("x") );

    // ensure we found exactly 5 tokens
    if ( tokens.size() != 5 ) {
        throw std::invalid_argument( 
                "expected 5 tokens, found " + 
                boost::lexical_cast<std::string>( tokens.size() )
                );
    }

    // iterate through tokens
    try {
        for ( Tokens::iterator i = tokens.begin(); i != tokens.end(); ++i ) {
            ssize_t index = std::distance( tokens.begin(), i );
            switch( index ) {
                case 0: _a = boost::lexical_cast<unsigned>( *i ); break;
                case 1: _b = boost::lexical_cast<unsigned>( *i ); break;
                case 2: _c = boost::lexical_cast<unsigned>( *i ); break;
                case 3: _d = boost::lexical_cast<unsigned>( *i ); break;
                case 4: _e = boost::lexical_cast<unsigned>( *i ); break;
                default: BOOST_ASSERT( !"unhandled token" ); break;
            }
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        throw std::invalid_argument(
                std::string("invalid shape: ") + e.what()
                );
    }

    // validate each token size
    if ( _a == 0 || _b == 0 || _c == 0 || _d == 0 || _e == 0 ) {
        // no dimension can be zero
        throw std::invalid_argument(
                "0 is not a valid size for any dimension" 
                );
    } else if ( _a > 4 || _b > 4 || _c > 4 || _d > 4 ) {
        // A, B, C, and D must be 4 or less
        throw std::invalid_argument(
                "A, B, C, and D dimensions must be a positive value of size 4 or less" 
                );
    } else if ( _e > 2 ) {
        // E must be 2 or less
        throw std::invalid_argument(
                "E dimension must be a positive value of size 2 or less" 
                );
    } else if ( _a == 3 || _b == 3 || _c == 3 || _d == 3 ) {
        // no dimension can be three
        throw std::invalid_argument(
                "3 is not a valid size for any dimension" 
                );
    }
}

bool
Shape::operator==(
        const Shape& other
        ) const
{
    return (
            _a == other._a &&
            _b == other._b && 
            _c == other._c &&
            _d == other._d &&
            _e == other._e
       );
}

Shape::operator std::string() const
{
    // if all dimensions are 0, return an empty string
    if ( !_a && !_b && !_c && !_d && !_e ) {
        return std::string();
    }

    std::ostringstream os;
    os << _a << "x" << _b << "x" << _c << "x" << _d << "x" << _e;
    return os.str();
}

void
Shape::addNode(
        const Uci& location
        )
{
    const uint8_t nodeBoard = BG_UCI_GET_NODE_BOARD(location.get());
    Nodes::iterator nodeboard = _nodes.find( nodeBoard );
    if ( nodeboard == _nodes.end() ) {
        nodeboard = _nodes.insert(
                Nodes::value_type(
                    nodeBoard,
                    Nodes::mapped_type()
                    )
                ).first;
    }

    nodeboard->second.push_back( BG_UCI_GET_COMPUTE_CARD(location.get()) );
}

std::istream&
operator>>(
        std::istream& stream,
        Shape& shape
        )
{
    std::string value;
    stream >> value;
    try {
        shape = Shape( value );
    } catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << std::endl;
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

std::ostream&
operator<<(
        std::ostream& os,
        const Shape& shape
        )
{
    os << std::string(shape);
    
    return os;
}

} // runjob
