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

#include "client/bgsched/ClientImpl.h"

#include <bgsched/runjob/Client.h>

#include <utility/include/ExitStatus.h>
#include <utility/include/version.h>

#include <boost/program_options/errors.hpp>
#include <boost/throw_exception.hpp>
#include <boost/utility.hpp>

namespace bgsched {
namespace runjob {

const unsigned Client::Version::major = MAJOR_VERSION;
const unsigned Client::Version::minor = MINOR_VERSION;
const unsigned Client::Version::mod = MOD_VERSION;
const char* Client::Version::driver = bgq::utility::DriverName;


Client::Client(
        const int argc,
        char** const argv
        ) :
    _impl( )
{
    BOOST_ASSERT( argc > 0 );
    BOOST_ASSERT( argv );

    try {
        _impl.reset(
                new Impl(argc, argv)
            );
    } catch ( const boost::program_options::error& e ) {
        // re-throw as a more sensible type
        BOOST_THROW_EXCEPTION(
                std::invalid_argument( e.what())
                );
    }
}

int
Client::start(
        const int input,
        const int output,
        const int error
        )
{
    const bgq::utility::ExitStatus result(
            _impl->start( input, output, error )
            );

    return result.get();
}

void
Client::kill(
        const int signal
        )
{
    _impl->kill( signal );
}

Client::Client(
        const Pimpl& impl
        ) :
    _impl( impl )
{

}

} // runjob
} // bgsched
