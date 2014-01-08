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
#define BOOST_TEST_MODULE stdinrank_test

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/test/unit_test.hpp>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <sys/un.h>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( stdinrank_test, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--stdinrank", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    _argv = make_argv("--stdinrank 123", _argc);
    Parser options(_argc, _argv);
    const size_t rank = options.getJobInfo().getStdinRank();
    BOOST_CHECK_EQUAL(
            rank,
            123u
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    int replace = 1;
    setenv("RUNJOB_STDINRANK", "456", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    const size_t rank = options.getJobInfo().getStdinRank();
    BOOST_CHECK_EQUAL(
            rank,
            456u
            );
    unsetenv("RUNJOB_STDINRANK");
}

BOOST_AUTO_TEST_CASE( arg_override_env )
{
    int replace = 1;
    setenv("RUNJOB_STDINRANK", "456", replace);
    _argv = make_argv("--stdinrank 123", _argc);
    Parser options(_argc, _argv);
    const size_t rank = options.getJobInfo().getStdinRank();
    BOOST_CHECK_EQUAL(
            rank,
            123u
            );
    unsetenv("RUNJOB_STDINRANK");
}

BOOST_AUTO_TEST_CASE( zero )
{
    _argv = make_argv("--stdinrank 0", _argc);
    Parser options(_argc, _argv);
    const size_t rank = options.getJobInfo().getStdinRank();
    BOOST_CHECK_EQUAL(
            rank,
            0u
            );
}

BOOST_AUTO_TEST_CASE( negative )
{
    _argv = make_argv("--stdinrank -1", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_SUITE_END()

