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
#include "common/RanksPerNode.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stdexcept>

namespace runjob {

RanksPerNode::RanksPerNode(
        unsigned ranks
        ) :
    _value( ranks )
{
    switch ( _value ) {
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
            break;
        default:
            throw std::logic_error(
                    boost::lexical_cast<std::string>(_value) +
                    " is not a valid ranks per node value"
                    );
    }
}

std::ostream&
operator<<(
        std::ostream& stream,
        const RanksPerNode& ranks
        )
{
    stream << ranks.getValue();
    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        RanksPerNode& ranks
        )
{
    // get string value from stream
    unsigned value;
    stream >> value;
    if ( !stream ) return stream;

    // assign value
    try {
        ranks = RanksPerNode(value);
    } catch ( const std::logic_error& e ) {
        stream.setstate( std::ios::failbit );
        std::cerr << e.what() << std::endl;
    }

    return stream;
}

} // runjob
