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
#define BOOST_TEST_MODULE raise
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( raise, Fixture )

BOOST_AUTO_TEST_CASE( default_value )
{
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getRaise(),
            false
            );
}

BOOST_AUTO_TEST_CASE( implicit_value )
{
    _argv = make_argv("--raise", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getRaise(),
            true
            );
}

BOOST_AUTO_TEST_CASE( explicit_env_true )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_RAISE", "true", replace),
            0
            );

    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getRaise(),
            true
            );
    unsetenv( "RUNJOB_RAISE" );
}

BOOST_AUTO_TEST_CASE( explicit_env_false )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_RAISE", "false", replace),
            0
            );

    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getRaise(),
            false
            );
    unsetenv( "RUNJOB_RAISE" );
}

BOOST_AUTO_TEST_CASE( explicit_env_1 )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_RAISE", "1", replace),
            0
            );

    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getRaise(),
            true
            );
    unsetenv( "RUNJOB_RAISE" );
}

BOOST_AUTO_TEST_CASE( explicit_env_0 )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_RAISE", "0", replace),
            0
            );

    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getRaise(),
            false
            );
    unsetenv( "RUNJOB_RAISE" );
}

BOOST_AUTO_TEST_SUITE_END()

