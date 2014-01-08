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
#include "common/Uci.h"

#include <boost/throw_exception.hpp>

#include <iostream>
#include <stdexcept>

namespace runjob {

Uci::Uci(
        const std::string& location
        ) :
    _location( 0 )
{
    if ( location.empty() ) return;

    const int rc = bg_string_to_uci( const_cast<char*>(location.c_str()), &_location );
    if ( rc != 0 ) {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    location + " is not a valid universal component identifier"
                    )
                );
    }
}

Uci::Uci(
        BG_UniversalComponentIdentifier location
        ) :
    _location( location )
{

}

std::ostream&
operator<<(
        std::ostream& os,
        const Uci& uci
        )
{
    char buf[128];
    if ( bg_uci_toString( uci.get(), buf )  == 0) {
        os << buf;
    } else {
        os.setstate( std::ios::failbit );
    }

    return os;
}

} // runjob
