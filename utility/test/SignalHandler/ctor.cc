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
#define BOOST_TEST_MODULE ctor
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/SignalHandler.h"

#include <iostream>

using namespace bgq::utility;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );
    }
};

void handler(
        const boost::system::error_code& error,
        const siginfo_t& siginfo,
        bool& handled
        )
{
    if ( !error ) {
        std::cout << "received signal " << siginfo.si_signo << std::endl;
        handled = true;
    } else {
        std::cout << boost::system::system_error(error).what() << std::endl;
        handled = false;
    }
}

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );
BOOST_AUTO_TEST_CASE( ctor )
{
    // create I/O service
    boost::asio::io_service io;

    // create signal handler for SIGINT
    SignalHandler<SIGINT> sig(io);

    // wait for signal
    bool handled = false;
    sig.async_wait(
            boost::bind(
                &handler,
                _1,
                _2,
                boost::ref(handled)
                )
            );

    // raise the signal
    raise(SIGINT);

    // run the I/O service loop
    io.run();

    // ensure we handled it
    BOOST_CHECK_EQUAL( handled, true );
}


