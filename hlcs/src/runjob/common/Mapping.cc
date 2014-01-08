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

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace runjob {

Mapping::Mapping(
        Mapping::Type t,
        const std::string& value
        ) :
    _type( t ),
    _value( value )
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
