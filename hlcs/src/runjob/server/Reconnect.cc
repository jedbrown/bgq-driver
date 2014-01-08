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
#include "server/Reconnect.h"

#include <boost/algorithm/string.hpp>

#include <iostream>

namespace runjob {
namespace server {

Reconnect::Reconnect(
        Scope scope 
        ) :
    _scope( scope )
{

}

std::ostream&
operator<<(
        std::ostream& os,
        const Reconnect& reconnect
        )
{
    if ( reconnect.scope() == Reconnect::Scope::Jobs ) {
        os << "jobs";
    } else if ( reconnect.scope() == Reconnect::Scope::Blocks) {
        os << "blocks";
    } else {
        os << "none";
    }

    return os;
}

std::istream&
operator>>(
        std::istream& stream,
        Reconnect& reconnect
        )
{
    // get string value from stream and convert to lowercase
    std::string value;
    stream >> value;
    boost::to_lower( value );

    if ( value == "none" ) {
        reconnect = Reconnect( Reconnect::Scope::None );
    } else if ( value == "jobs" ) {
        reconnect = Reconnect( Reconnect::Scope::Jobs );
    } else if ( value == "blocks" ) {
        reconnect = Reconnect( Reconnect::Scope::Blocks );
    } else {
        std::cerr << value << " is not none, blocks, or jobs" << std::endl;
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // server
} // runjob

