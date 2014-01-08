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
#define BOOST_TEST_MODULE host
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <utility/include/cxxsockets/Host.h>

#include <iostream>

using namespace bgq::utility;
using namespace CxxSockets;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( default_ctor )
{
    Host h;
    BOOST_CHECK( h.ip().empty() );
    BOOST_CHECK( h.fqhn().empty() );
    BOOST_CHECK( h.uhn().empty() );
}

BOOST_AUTO_TEST_CASE( localhost_explicit_ctor )
{
    Host h( "127.0.0.1" );
    BOOST_CHECK( !h.ip().empty() );
    BOOST_CHECK( !h.fqhn().empty() );
    BOOST_CHECK( !h.uhn().empty() );
}

BOOST_AUTO_TEST_CASE( equality_operator )
{
    Host h1( "127.0.0.1" );
    Host h2( "192.168.0.1" );
    BOOST_CHECK( h1 == h1 );
    BOOST_CHECK( h2 == h2 );
}

BOOST_AUTO_TEST_CASE( less_than_operator )
{
    Host h1( "1.9.1.9" );
    Host h2( "1.9.1.10" );
    if ( !(h1 == h2) ) {
        BOOST_CHECK( h2 < h1 );
    }
}


