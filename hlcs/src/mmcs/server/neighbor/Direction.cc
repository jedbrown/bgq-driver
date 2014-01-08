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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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


#include "Direction.h"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <cstdio>
#include <cstring>
#include <stdexcept>


LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace neighbor {

Direction::Direction(
        const char* rawdata
        ) :
    _buf(),
    _dimension( Dimension::Invalid ),
    _orientation( Orientation::Invalid )
{
    const char* match = strstr(rawdata, "DIM_DIR=");
    char buf[3] = {0};
    if ( !match || sscanf(match, "DIM_DIR=%2s;", buf) != 1 ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument("Missing DIM_DIR key in raw data")
                );
    }
    _buf = buf;

    // special case for IO link
    if ( _buf == "IO" ) {
        _dimension = Dimension::IO;
        return;
    }

    // special case for not applicable
    if ( _buf == "NA" ) {
        return;
    }

    switch ( buf[0] ) {
        case 'A': _dimension = Dimension::A; break;
        case 'B': _dimension = Dimension::B; break;
        case 'C': _dimension = Dimension::C; break;
        case 'D': _dimension = Dimension::D; break;
        case 'E': _dimension = Dimension::E; break;
        default:
                  BOOST_THROW_EXCEPTION(
                          std::invalid_argument( std::string("Invalid dimension '") + buf + "'")
                          );
                  break;
    }

    switch ( buf[1] ) {
        case '+': _orientation = Orientation::Plus; break;
        case '-': _orientation = Orientation::Minus; break;
        default:
                  BOOST_THROW_EXCEPTION(
                          std::invalid_argument( std::string("Invalid dimension direction: ") + buf )
                          );
    }
}


std::ostream&
operator<<(
        std::ostream& os,
        const Direction& direction
        )
{
    os << direction.buf();
    return os;
}

std::ostream&
operator<<(
        std::ostream& os,
        const Dimension::Value& dimension
        )
{
    switch (dimension) {
        case Dimension::A: os << "A"; break;
        case Dimension::B: os << "B"; break;
        case Dimension::C: os << "C"; break;
        case Dimension::D: os << "D"; break;
        case Dimension::E: os << "E"; break;
        case Dimension::IO: os << "IO"; break;
        default: os << "N/A"; break;
    }

    return os;
}




} } } // namespace mmcs::server::neighbor
