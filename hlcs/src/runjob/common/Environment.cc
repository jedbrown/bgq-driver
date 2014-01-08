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
#include "common/Environment.h"

#include <cctype>
#include <vector>

namespace runjob {

Environment::Environment(
        const std::string& key,
        const std::string& value
        ) :
    _key( key ),
    _value( value )
{

}

std::ostream&
operator<<(
        std::ostream& stream,
        const Environment& env
        )
{
    stream << env.getKey() << "=" << env.getValue();
    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        Environment& env
        )
{
    // instead of using 
    // std::getline OR
    // std::istream::getline OR
    // stream >> value
    //
    // we loop through the stream character by character. Environment variables can have
    // embedded line feeds and we need to handle them appropriately.
    std::string value;
    while ( true ) {
        char c;
        if ( !stream.get(c) ) break;
        value += c;
    }

    std::string::size_type equals = value.find_first_of('=');

    // validate supported environment variable syntax
    if ( equals == std::string::npos ) {
        std::cerr << "environment '" << value << "' missing equals character." << std::endl;
        stream.setstate( std::ios::failbit );
    } else if ( value[0] == '=' ) {
        std::cerr << "environment '" << value << "' has equals character in first position." << std::endl;
        stream.setstate( std::ios::failbit );
    } else if ( isblank(value[0]) ) {
        std::cerr << "environment '" << value << "' has a leading space." << std::endl;
        stream.setstate( std::ios::failbit );
    } else if ( std::isdigit(value[0]) ) {
        std::cerr << "environment '" << value << "' has a digit as the leading character." << std::endl;
        stream.setstate( std::ios::failbit );
    } else {
        stream.clear();
        env = Environment(
                value.substr(0, equals),
                value.substr(equals + 1)
                );
    }

    return stream;
}

} // runjob
