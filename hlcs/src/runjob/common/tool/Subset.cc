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
#include "common/tool/Subset.h"

#include "common/logging.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <algorithm>
#include <iostream>
#include <limits>

namespace runjob {
namespace tool {

const size_t Subset::MaximumLength = 4096;

Subset::Subset(
        const std::string& value
        ) :
    _value( value ),
    _min( std::numeric_limits<unsigned>::max() ),
    _max( 0 ),
    _ranks(),
    _ranges()
{
    if ( value.empty() ) {
        return;
    }

    if ( value.size() > 4096 ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "subset length of " +
                    boost::lexical_cast<std::string>( value.size() ) +
                    " is larger than maximum length of " +
                    boost::lexical_cast<std::string>( MaximumLength ) + 
                    " characters."
                    )
                );
    }

    // split on spaces
    typedef std::vector<std::string> Tokens;
    Tokens tokens;
    boost::split( tokens, _value, boost::is_any_of(" ") );

    BOOST_FOREACH( const std::string& i, tokens ) {
        this->parse( i );
    }
}

void
Subset::parse(
        const std::string& token
        )
{
    // A rank specification can be either a number or "$max" which represents the last rank in the MPI job. For
    // example: 1 2 4-6 7-$max:2 will attach to ranks 1, 2, 4, 5, 6, 7, 9, 11, 13. … etc.
    if ( token.find_first_of( '-' ) != std::string::npos ) {
        SubsetRange range( token );

        if ( _max && static_cast<unsigned>(range.begin()) <= _max ) {
            BOOST_THROW_EXCEPTION(
                    std::logic_error(
                        "invalid subset range '" + token +
                        "'. Starting range must be a greater than " +
                        boost::lexical_cast<std::string>( _max )
                        )
                    );
        }

        _ranges.push_back( range );

        if ( range.end() == -1 ) {
            _max =  std::numeric_limits<unsigned>::max();
        } else {
            _max = range.end();
        }

        if ( static_cast<unsigned>(range.begin()) < _min ) {
            _min = range.begin();
        }
    } else {
        // assume rank
        const unsigned rank = this->rank( token );
        if ( _max && rank <= _max ) {
            BOOST_THROW_EXCEPTION(
                    std::logic_error(
                        "invalid subset rank '" + token +
                        "'. Must be a greater than " +
                        boost::lexical_cast<std::string>( _max )
                        )
                    );
        }

        _ranks.push_back( rank );
        _max = rank;

        if ( rank < _min ) {
            _min = rank;
        }
    }
}

unsigned
Subset::rank(
        const std::string& token
        )
{
    try {
        const int rank = boost::lexical_cast<int>( token );
        if ( rank < 0 ) {
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument(
                        "invalid subset range rank '" +
                        boost::lexical_cast<std::string>( rank ) +
                        "'. Must be a greater than or equal to zero."
                        )
                    );
        }

        return rank;
    } catch ( const boost::bad_lexical_cast& e ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "invalid subset range rank '" + token +
                    "'. Must be a number greater than or equal to zero."
                    )
                );
    }
}

bool
Subset::match(
        unsigned rank
        ) const
{
    if ( _value.empty() ) return true;
    if ( rank < _min ) return false;
    if ( rank > _max ) return false;

    const Ranks::const_iterator i = std::find_if(
                _ranks.begin(),
                _ranks.end(),
                boost::bind(
                    std::equal_to<unsigned>(),
                    rank,
                    _1
                    )
                );
    if ( i != _ranks.end() ) {
        return true;
    }

    const Ranges::const_iterator j = std::find_if(
            _ranges.begin(),
            _ranges.end(),
            boost::bind(
                &SubsetRange::match,
                _1,
                rank
                )
            );

    return j != _ranges.end();
}

std::istream&
operator>>(
        std::istream& is,
        Subset& subset
        )
{
    std::string line;
    (void)std::getline( is, line );
    try {
        subset = Subset( line );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        is.setstate( std::ios::failbit );
    }

    return is;
}

std::ostream&
operator<<(
        std::ostream& os,
        const Subset& subset
        )
{
    if ( subset._value.empty() ) {
        os << "0-$max";
        return os;
    }

    for ( Subset::Ranks::const_iterator i = subset._ranks.begin(); i != subset._ranks.end(); ++i ) {
        os << *i;
        if ( i != subset._ranks.begin() ) {
            os << " ";
        }
    }

    for ( Subset::Ranges::const_iterator i = subset._ranges.begin(); i != subset._ranges.end(); ++i ) {
        os << *i;
        if ( i != subset._ranges.begin() ) {
            os << " ";
        }
    }

    return os;
}

} // tool
} // runjob

