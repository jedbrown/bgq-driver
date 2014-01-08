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
#define BOOST_TEST_MODULE socket_test
#include <boost/test/unit_test.hpp>

#include "mux/Options.h"

#include "test/mux/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::mux;

BOOST_FIXTURE_TEST_SUITE( socket_test, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--socket", _argc);
    BOOST_REQUIRE_THROW(
            Options(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    _argv = make_argv("--socket foo", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLocalSocket(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( properties )
{
    _argv = make_argv("--properties good.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLocalSocket(),
            "/foo/bar"
            );
}

BOOST_AUTO_TEST_CASE( arg_override_properties )
{
    _argv = make_argv("--socket foo --properties good.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLocalSocket(),
            "foo"
            );
}

BOOST_AUTO_TEST_SUITE_END()
