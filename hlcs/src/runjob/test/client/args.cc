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
#define BOOST_TEST_MODULE args
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <ramdisk/include/services/JobctlMessages.h>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( args, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--args", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( single_arg )
{
    _argv = make_argv("--args foo", _argc);
    Parser options(_argc, _argv);
    const std::vector<std::string>& args = options.getJobInfo().getArgs();
    BOOST_REQUIRE_EQUAL(
            args.at(0),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( double_arg )
{
    _argv = make_argv("--args foo --args bar", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            2u
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(0),
            "foo"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(1),
            "bar"
            );
}

BOOST_AUTO_TEST_CASE( multitoken )
{
    _argv = make_argv("--args foo bar --args baz", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            3u
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(0),
            "foo"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(1),
            "bar"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(2),
            "baz"
            );
}

BOOST_AUTO_TEST_CASE( multitoken_2 )
{
    // ensure job arguments that look like runjob arguments (--block) are
    // accepted
    _argv = make_argv("--args foo --args --block bar --block baz", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            3u
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(0),
            "foo"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(1),
            "--block"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(2),
            "bar"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getBlock(),
            "baz"
            );
}

BOOST_AUTO_TEST_CASE( exe_with_single_arg_max_size )
{
    std::string args("--exe foo --args ");
    for (int32_t i = 4; i < bgcios::jobctl::MaxArgumentSize - 2; ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_NO_THROW(
            Parser(_argc, _argv)
            );
}

BOOST_AUTO_TEST_CASE( exe_with_single_arg_too_large )
{
    std::string args("--exe foo --args ");
    for (int32_t i = 4; i < bgcios::jobctl::MaxArgumentSize - 1; ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( exe_with_multiple_args_too_large )
{
    std::string args("--exe foo ");
    const size_t num_args = 4;
    for ( size_t arg = 0; arg < num_args; ++arg ) {
        args.append( "--args " );
        
        // -1 for NULL terminator implicitly included in max argument size
        const size_t arg_size = bgcios::jobctl::MaxArgumentSize / num_args - 1;

        for ( size_t i = 0; i < arg_size; ++i ) {
            args.append( boost::lexical_cast<std::string>(i % 10) );
        }
    }
    _argv = make_argv( args.c_str(), _argc );
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_ARGS", "--foo", replace),
            0
            );
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            1u
            );
}

BOOST_AUTO_TEST_CASE( composing )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_ARGS", "--foo", replace),
            0
            );
    _argv = make_argv("--args --bar", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            2u
            );
}

BOOST_AUTO_TEST_CASE( space )
{
    const char* argv[] = {"foo", "--args", "hello world", "goodbye world"};

    Parser options( 4, const_cast<char**>(argv));
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().size(),
            2u
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(0),
            "hello world"
            );
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getArgs().at(1),
            "goodbye world"
            );
}

BOOST_AUTO_TEST_SUITE_END()

