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
#include "common/tool/SubsetRange.h"

#include "common/logging.h"

#include <iostream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

namespace runjob {
namespace tool {

SubsetRange::SubsetRange(
        const std::string& token
        ) :
    _begin( 0 ),
    _end( 0 ),
    _stride( 1 )
{
    if ( token.empty() ) return;

    // range is in the format rank1-rank2:stride
    // note that stride is optional and assumed to be 1
    // note that rank2 can optionally be $max or max

    // copy token into range so we can strip off stride if needed
    const std::string range = this->parseStride( token );

    // split on - character
    typedef std::vector<std::string> Tokens;
    Tokens tokens;
    boost::split( tokens, range, boost::is_any_of("-") );
    if ( tokens.size() != 2 ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range token '" + token + "'."
                    )
                );
    }

    _begin = this->parseRank( tokens[0] );
    _end = this->parseRank( tokens[1] );

    // beginning range cannot be max
    if ( _begin == -1 ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range rank '" + token + "'. rank1 cannot be $max."
                    )
                );
    }

    // must have a range
    if ( _begin == _end ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range '" + token + "'. Range must be larger than 0."
                    )
                );
    }

    // beginning must be before end
    if ( _end != -1 && _begin > _end ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range '" + token + "'. rank1 must be less than rank2."
                    )
                );
    }
    
    // stride must encompass more than a single element
    if ( _end != -1 && _begin + _stride > _end ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range '" + token + "'. Stride must fit within range."
                    )
                );
    }
}

int
SubsetRange::parseRank(
        const std::string& token
        ) const
{
    // special case for $max
    if ( token == "$max" || token == "max" ) {
        return -1;
    }

    try {
        const int rank = boost::lexical_cast<int>( token );
        if ( rank < 0 ) {
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument(
                        "invalid subset range rank '" +
                        boost::lexical_cast<std::string>( rank ) +
                        "'"
                        )
                    );
        }

        return rank;
    } catch ( const boost::bad_lexical_cast& e ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range token '" + token + "'."
                    )
                );
    }
}

std::string
SubsetRange::parseStride(
        const std::string& token
        )
{
    // stride is optionally specified after the range
    // rank1-rank2:stride
    const std::string::size_type stride_position = token.find_first_of(':');
    if ( stride_position == std::string::npos ) return token;

    try {
        _stride = boost::lexical_cast<int>( 
                token.substr(stride_position + 1)
                );
    } catch ( const boost::bad_lexical_cast& e ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid rank subset range token: '" + token + "'"
                    )
                );
    }

    // stride cannot be zero or negative
    if ( _stride <= 0 ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid rank subset range token: '" + token + "'"
                    )
                );
    }

    return token.substr( 0, stride_position );
}

bool
SubsetRange::match(
        unsigned rank
        ) const
{
    if ( rank < static_cast<unsigned>(_begin) ) return false;

    // special case for $max
    if ( _end == -1 ) return true;

    if ( rank > static_cast<unsigned>(_end) ) return false;

    for ( unsigned i = _begin; i <= static_cast<unsigned>(_end); i += _stride ) {
        if ( rank == i ) return true;
    }

    return false;
}

std::ostream&
operator<<(
        std::ostream& os,
        const SubsetRange& range
        )
{
    os << range._begin << "-";
    if ( range._end == -1 ) {
        os << "$max";
    } else {
        os << range._end;
    }
    os << ":" << range._stride;

    return os;
}

} // tool
} // runjob

