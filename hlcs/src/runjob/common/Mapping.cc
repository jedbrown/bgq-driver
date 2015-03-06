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

#include <db/include/api/tableapi/gensrc/DBTJob.h>

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
        const std::string& value,
        const bool performValidation
        ) :
    _type( t ),
    _value( value )
{
    if ( _value.size() > BGQDB::DBTJob::MAPPING_SIZE ) {
        BOOST_THROW_EXCEPTION(
                std::length_error(
                    "string length " +
                    boost::lexical_cast<std::string>( _value.size() ) +
                    " is greater than maximum length " +
                    boost::lexical_cast<std::string>( BGQDB::DBTJob::MAPPING_SIZE )
                    )
                );
    }

    if ( t == Mapping::Type::File && performValidation ) {
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

    LOG_DEBUG_MSG( "Opening " << _value );
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
    size_t lineCount( 0 );
    while ( std::getline(file, line) ) {
        _fullMapFileContents.push_back( line );
        try {
            this->analyzeLine( line, ranks, lineCount );
            ++lineCount;
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( e.what() );
            throw;
        }
    }

    LOG_DEBUG_MSG( "Read " << lineCount << " lines" );

    // remember encoded contents of mapping file
    std::vector<uint32_t> contents( ranks.begin(), ranks.end() );
    _fileContents.swap( contents );
}

void
Mapping::analyzeLine(
        std::string& line,
        Rank& ranks,
        const size_t lineNumber
        )
{
    // strip comments
    const std::size_t comment = line.find_first_of('#');
    if ( comment != std::string::npos ) {
        line.erase( comment );
    }

    // skip empty lines
    if ( line.empty() ) return;

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
                        boost::lexical_cast<std::string>(lineNumber)
                        )
                    );
        }
    }

    // line was valid, ensure all the coordinates are positive
    for ( unsigned i = 0; i < 6; ++i ) {
        if ( coords[i] < 0 ) {
            BOOST_THROW_EXCEPTION(
                    std::logic_error(
                        names[i] + " coordinate on line " + 
                        boost::lexical_cast<std::string>(lineNumber) +
                        " must be zero or greater"
                        )
                );
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
    const Rank::iterator result = std::find( ranks.begin(), ranks.end(), nodeId );
    if ( result != ranks.end() ) {
        BOOST_THROW_EXCEPTION(
                std::logic_error(
                    "Coordinates (" +
                    boost::lexical_cast<std::string>(coords[0]) + "," +
                    boost::lexical_cast<std::string>(coords[1]) + "," +
                    boost::lexical_cast<std::string>(coords[2]) + "," +
                    boost::lexical_cast<std::string>(coords[3]) + "," +
                    boost::lexical_cast<std::string>(coords[4]) + "," +
                    boost::lexical_cast<std::string>(coords[5]) + ") on line " +
                    boost::lexical_cast<std::string>(lineNumber) +
                    " already exists for rank " + 
                    boost::lexical_cast<std::string>(std::distance( ranks.begin(), result ))
                    )
                );
    }
    ranks.push_back( nodeId );

    LOG_TRACE_MSG(
            lineNumber << " (" <<
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

    const Mapping::Type t = Mapping::getType( value );

    try {
        mapping = Mapping( t, value );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

Mapping::Type
Mapping::getType(
        const std::string& value
        )
{
    Mapping::Type result;

    // if value is 6 characters in length, assume permutation
    if ( value.size() == 6 ) {
        result = Mapping::Type::Permutation;
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
                    result = Mapping::Type::File;
                    break;
            }

            if ( count > 1 ) {
                result = Mapping::Type::File;
                break;
            }

            if ( result == Mapping::Type::File ) {
                break;
            }
        }
    } else {
        // assume file
        result = Mapping::Type::File;
    }

    return result;
}

} // runjob
