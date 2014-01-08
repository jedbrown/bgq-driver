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
#define BOOST_TEST_MODULE bool_alpha
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/BoolAlpha.h"

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

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( true_string )
{
    BoolAlpha foo;
    const std::string bar( "true" );
    std::istringstream is( bar );
    is >> foo;
    BOOST_CHECK_EQUAL( foo._value, true );
}

BOOST_AUTO_TEST_CASE( false_string )
{
    BoolAlpha foo;
    const std::string bar( "false" );
    std::istringstream is( bar );
    is >> foo;
    BOOST_CHECK_EQUAL( foo._value, false );
}

BOOST_AUTO_TEST_CASE( gibberish_string )
{
    BoolAlpha foo;
    const std::string bar( "adsfsdf" );
    std::istringstream is( bar );
    is >> foo;
    BOOST_CHECK_EQUAL( is.good(), false );
}

BOOST_AUTO_TEST_CASE( empty_string )
{
    BoolAlpha foo;
    const std::string bar;
    std::istringstream is( bar );
    is >> foo;
    BOOST_CHECK_EQUAL( is.good(), false );
}
