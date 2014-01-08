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
#define BOOST_TEST_MODULE tool
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/errors.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( tool, Fixture )

BOOST_AUTO_TEST_CASE( empty_arg )
{
    _argv = make_argv("--tool-args", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( missing_start_tool_with_args )
{
    // --tool-args requires --start-tool
    _argv = make_argv("--tool-args foo bar far", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( missing_start_tool_with_subset )
{
    // --tool-subset requires --start-tool
    _argv = make_argv("--tool-subset 0-$max", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( single_arg )
{
    _argv = make_argv("--start-tool foo --tool-args hello", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments().size(),
            1u
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments()[0],
            "hello"
            );
}

BOOST_AUTO_TEST_CASE( two_args )
{
    const char* argv[] = {"foo", "--start-tool", "foo", "--tool-args", "hello world"};
    Parser options(5, const_cast<char**>(argv));
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments().size(),
            2u
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments()[0],
            "hello"
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments()[1],
            "world"
            );
}

BOOST_AUTO_TEST_CASE( three_args )
{
    const char* argv[] = {"foo", "--start-tool", "foo", "--tool-args", "--goodbye hello --world"};
    Parser options(5, const_cast<char**>(argv));
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments().size(),
            3u
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments()[0],
            "--goodbye"
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments()[1],
            "hello"
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getArguments()[2],
            "--world"
            );
}

BOOST_AUTO_TEST_CASE( tool_daemon )
{

    _argv = make_argv("--start-tool hello", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getTool().getExecutable().empty(),
            false
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getExecutable(),
            "hello"
            );
}

BOOST_AUTO_TEST_CASE( tool_subset )
{

    _argv = make_argv("--start-tool hello --tool-subset 1-10", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getTool().getExecutable().empty(),
            false
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getExecutable(),
            "hello"
            );
    BOOST_CHECK_EQUAL(
            options.getTool().getSubset().value(),
            "1-10"
            );
}

BOOST_AUTO_TEST_CASE( path_too_long )
{
    const std::string executable( "hello" );
    size_t size = executable.size();
    std::string args;
    for ( ; size < static_cast<unsigned>(bgcios::jobctl::MaxArgumentSize) - 2; ++size ) {
        args.append( boost::lexical_cast<std::string>(size % 10 ) );
    }
    _argv = make_argv( "--start-tool " + executable + " --tool-args " + args, _argc );

    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_SUITE_END()

