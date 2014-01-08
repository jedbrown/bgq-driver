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

#include "utility.hpp"

#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <errno.h>
#include <string.h>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace utility {


void getRandomBytes( Bytes& bytes_out )
{
    // read bytes_out.size() characters from /dev/urandom.

    std::ifstream ifs( "/dev/urandom" );

    if ( ! ifs ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "failed to open /dev/urandom" ) );
    }

    ifs.read( reinterpret_cast<char*>(bytes_out.data()), bytes_out.size() );

    if ( ! ifs ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "failed to read random bytes" ) );
    }
}


} // namespace bgws::utility
} // namespace bgws
