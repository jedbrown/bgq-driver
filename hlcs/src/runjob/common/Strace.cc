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
#include "common/Strace.h"

#include "common/logging.h"

#include <boost/algorithm/string.hpp>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

std::ostream&
operator<<(
        std::ostream& stream,
        const Strace& strace
        )
{
    switch ( strace.getScope() ) {
        case Strace::None:  stream << "none"; break;
        case Strace::Node:  stream << strace.getRank(); break;
        default:            BOOST_ASSERT( !"unhandled value" );
    }

    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        Strace& strace
        )
{
    // get string value from stream and convert to lowercase
    std::string value;
    stream >> value;
    boost::to_lower( value );

    if ( value == "none" ) {
        strace = Strace( Strace::None );
        return stream;
    }

    try {
        uint32_t rank = boost::lexical_cast<uint32_t>( value );
        strace = Strace( Strace::Node , rank );
    } catch ( const boost::bad_lexical_cast& e ) {
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // runjob
