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
#include "client/options/Label.h"

#include <boost/algorithm/string.hpp>

#include <boost/assert.hpp>

#include <iostream>

namespace runjob {
namespace client {
namespace options {

Label::Label(
        Scope scope
        ) :
    _scope( scope )
{

}

std::ostream&
operator<<(
        std::ostream& stream,
        const Label& label
        )
{
    switch ( label.getScope() ) {
        case Label::None:  stream << "none"; break;
        case Label::Short: stream << "short"; break;
        case Label::Long:  stream << "long"; break;
        default:            BOOST_ASSERT( !"unhandled value" );
    }

    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        Label& label
        )
{
    // get string value from stream and convert to lowercase
    std::string value;
    stream >> value;
    boost::to_lower( value );

    if ( value == "none" ) {
        label = Label( Label::None );
        return stream;
    } else if ( value == "short" ) {
        label = Label( Label::Short );
        return stream;
    } else if ( value == "long" ) {
        label = Label( Label::Long );
        return stream;
    }

    stream.setstate( std::ios::failbit );

    return stream;
}

} // options
} // clent
} // runjob
