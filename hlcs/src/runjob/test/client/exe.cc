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
#define BOOST_TEST_MODULE exe
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"
#include "client/Job.h"

#include "common/Exception.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <boost/asio.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( exe, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--exe", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    _argv = make_argv("--exe foo", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getExe(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( arg_too_large )
{
    std::string args("--exe ");
    BGQDB::DBTJob job;
    for (uint32_t i = 0; i <= sizeof( job._executable ); ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    // ensure RUNJOB_EXE sets the executable name
    const int replace = 1;
    setenv("RUNJOB_EXE", "foo", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getExe(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( mpmd_positional_args )
{
    _argv = make_argv(": foo bar baz", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getExe(),
            "foo"
            );

    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            2u
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(0),
            "bar"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(1),
            "baz"
            );
}

BOOST_AUTO_TEST_CASE( both_exe_and_mpmd_positional_exe )
{
    _argv = make_argv("--exe foo : bar", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( both_args_and_mpmd_positional_exe )
{
    _argv = make_argv("--args foo bar : baz", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( mpmd_positional_missing_args )
{
    _argv = make_argv(":", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_SUITE_END()
