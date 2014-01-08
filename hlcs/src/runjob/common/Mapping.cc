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
#include "common/Mapping.h"

#include "common/error.h"
#include "common/logging.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>
#include <boost/system/system_error.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

Mapping::Mapping(
        const Mapping::Type t,
        const std::string& value
        ) :
    _type( t ),
    _value( value ),
    _maxDimensions( ),
    _lineCount( 0 )
{
    if ( _value.size() > MAPPING_STRING_SIZE ) {
        BOOST_THROW_EXCEPTION(
                std::length_error(
                    "string length " +
                    boost::lexical_cast<std::string>( _value.size() ) +
                    " is greater than maximum length " +
                    boost::lexical_cast<std::string>( MAPPING_STRING_SIZE )
                    )
                );
    }

    if ( t == Mapping::Type::File ) {
        _maxDimensions.insert(
                _maxDimensions.begin(),
                6,
                std::make_pair( 0,0 )
                );
        this->validateFile();
    }
}

std::ostream&
operator<<(
        std::ostream& stream,
        const Mapping& mapping
        )
{
    switch ( mapping.type() ) {
        case Mapping::Type::None:   stream << "none"; break;
        default:                    stream << mapping.value(); break;
    }

    return stream;
}

void
Mapping::validateFile()
{
    BOOST_ASSERT( _type == Mapping::Type::File );
    Rank ranks;

    LOG_DEBUG_MSG( "opening " << _value );
    std::ifstream file( _value );
    if ( !file ) {
        boost::system::system_error error(
                boost::system::error_code(
                    errno,
                    boost::system::get_system_category()
                    )
                );

        BOOST_THROW_EXCEPTION(
                std::runtime_error(
                    "Could not open mapping file: " + _value + ". " + error.what()
                    )
            );
    }

    std::string line;
    while( std::getline(file, line) ) {
        try {
            this->analyzeLine( line, ranks );
            ++_lineCount;
        } catch( const std::exception& e ) {
            LOG_WARN_MSG( e.what() );
            throw;
        }
    }

    LOG_DEBUG_MSG( "read " << _lineCount << " lines" );

    for ( Dimensions::iterator i = _maxDimensions.begin(); i != _maxDimensions.end(); ++i ) {
        const size_t dimension = std::distance( _maxDimensions.begin(), i );
        switch (dimension) {
            case 5:
                LOG_DEBUG_MSG( "maximum T dimension " << i->first << " on line " << i->second );
                break;
            default:
                LOG_DEBUG_MSG(
                        "maximum " << static_cast<char>('A' + dimension) << " dimension " << i->first << 
                        " on line " << i->second
                        );
        }
    }
}

void
Mapping::analyzeLine(
        std::string& line,
        Rank& ranks
        )
{
    // strip comments
    const std::size_t comment = line.find_first_of('#');
    if ( comment != std::string::npos ) {
        line.erase( comment );
        if ( line.empty() ) return;
    }

    LOG_TRACE_MSG( "line: " << line );

    std::istringstream is( line );
    int coords[6];
    const char names[] = {'A','B','C','D','E','T'};

    for ( unsigned i = 0; i < 6; ++i ) {
        is >> coords[i];
        if ( !is ) {
            BOOST_THROW_EXCEPTION(
                    std::logic_error(
                        (is.eof() ? std::string("Missing") : std::string("Garbage")) + " " + names[i] + 
                        " coordinate on line " +
                        boost::lexical_cast<std::string>(_lineCount)
                        )
                    );
        }
    }

    // line was valid, compare the coordinates to see if any of these
    // dimensions are greater than what we've seen before
    for ( unsigned i = 0; i < 6; ++i ) {
        if ( coords[i] < 0 ) {
            BOOST_THROW_EXCEPTION(
                    std::logic_error(
                        names[i] + " coordinate on line " + 
                        boost::lexical_cast<std::string>(_lineCount) +
                        " must be zero or greater"
                        )
                );
        }

        if ( coords[i] > _maxDimensions[i].first ) {
            _maxDimensions[i].first = coords[i];
            _maxDimensions[i].second = _lineCount;
        }
    }

    // collapse coordinates into a single integer, need 6 bits for A, B, C, D, and T dimensions, 
    // but only one bit for E dimension
    const uint32_t nodeId = 
        ((coords[0] & 0x3F) << 26) | 
        ((coords[1] & 0x3F) << 20) | 
        ((coords[2] & 0x3F) << 14) |
        ((coords[3] & 0x3F) << 8 ) |
        ((coords[4] & 0x01) << 7 ) |
         (coords[5] & 0x3F)
         ;

    // ensure these coordinates do not exist in the mapping already
    const std::pair<Rank::iterator,bool> result = ranks.insert( nodeId );
    if ( !result.second ) {
        BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "Coordinates (" +
                    boost::lexical_cast<std::string>(coords[0]) + "," +
                    boost::lexical_cast<std::string>(coords[1]) + "," +
                    boost::lexical_cast<std::string>(coords[2]) + "," +
                    boost::lexical_cast<std::string>(coords[3]) + "," +
                    boost::lexical_cast<std::string>(coords[4]) + "," +
                    boost::lexical_cast<std::string>(coords[5]) + ") on line " +
                    boost::lexical_cast<std::string>(_lineCount) +
                    " already exists for rank " + 
                    boost::lexical_cast<std::string>(std::distance( ranks.begin(), result.first ))
                    )
                );
    }

    LOG_TRACE_MSG(
            _lineCount << " (" <<
            coords[0] << "," <<
            coords[1] << "," <<
            coords[2] << "," <<
            coords[3] << "," <<
            coords[4] << "," <<
            coords[5] << ") 0x" <<
            std::hex << nodeId
            );
}


std::istream&
operator>>(
        std::istream& stream,
        Mapping& mapping
        )
{
    // get string value from stream and convert to lowercase
    std::string value;
    stream >> value;

    Mapping::Type t = Mapping::Type::None;

    // if value is not 6 characters in length, consider it a 
    if ( value.size() == 6 ) {
        t = Mapping::Type::Permutation;
        BOOST_FOREACH( const char c, value ) {
            size_t count(0);
            switch ( c ) {
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'T':
                    // count number of occurences of this character in the string
                    count = std::count_if( 
                            value.begin(), 
                            value.end(), 
                            boost::bind(
                                std::equal_to<char>(), 
                                c,
                                _1
                                )
                            );
                    break;
                default:
                    t = Mapping::Type::File;
                    break;
            }

            if ( count > 1 ) {
                t = Mapping::Type::File;
                break;
            }

            if ( t == Mapping::Type::File ) {
                break;
            }
        }
    } else {
        // assume file
        t = Mapping::Type::File;
    }

    try {
        mapping = Mapping( t, value );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // runjob
