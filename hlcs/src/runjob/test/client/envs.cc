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
#define BOOST_TEST_MODULE envs
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <ramdisk/include/services/JobctlMessages.h>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( envs, Fixture )

BOOST_AUTO_TEST_CASE( empty_parameter_arg )
{
    const char* argv[] = {"foo", "--envs", ""};
    BOOST_REQUIRE_THROW(
            Parser( 3, const_cast<char**>(argv) ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( missing_equals )
{
    _argv = make_argv("--envs foo", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( single_arg )
{
    _argv = make_argv("--envs foo=bar", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const std::string& env = envs.at(0);
    std::string::size_type equals = env.find_first_of('=');
    BOOST_REQUIRE_EQUAL(
            env.substr(0, equals),
            "foo"
            );
    BOOST_REQUIRE_EQUAL(
            env.substr(equals + 1),
            "bar"
            );
}

BOOST_AUTO_TEST_CASE( multitoken )
{
    _argv = make_argv("--envs foo=bar bar=foo --envs baz=foo", _argc);
    Parser options(_argc, _argv);

    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            3u
            );
}

BOOST_AUTO_TEST_CASE( double_arg )
{
    _argv = make_argv("--envs foo=bar --envs bar=foo", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            2u
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_ENVS", "foo=bar", replace),
            0
            );
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            1u
            );
}

BOOST_AUTO_TEST_CASE( duplicate_key )
{
    _argv = make_argv("--envs foo=bar --envs foo=baz", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( equals )
{
    // multiple equals tokens are supported
    _argv = make_argv("--envs foo=bar=baz", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            "foo"
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            "bar=baz"
            );
}

BOOST_AUTO_TEST_CASE( too_large )
{
    std::string envs("--envs foo=");
    for (int32_t i = 4; i < bgcios::jobctl::MaxVariableSize; ++i) {
        envs.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( envs.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( composing )
{
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_ENVS", "foo=bar", replace),
            0
            );
    _argv = make_argv("--envs bar=foo", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            2u
            );
}

BOOST_AUTO_TEST_CASE( space )
{
    const char* argv[] = {"foo", "--envs", "ONE=hello world", "TWO=goodbye world"};

    Parser options( 4, const_cast<char**>(argv));
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            2u
            );
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& first = envs.at(0);
    const runjob::Environment& second = envs.at(1);

    BOOST_REQUIRE_EQUAL(
            first.getKey(),
            "ONE"
            );
    BOOST_REQUIRE_EQUAL(
            first.getValue(),
            "hello world"
            );
    BOOST_REQUIRE_EQUAL(
            second.getKey(),
            "TWO"
            );
    BOOST_REQUIRE_EQUAL(
            second.getValue(),
            "goodbye world"
            );
}

BOOST_AUTO_TEST_CASE( equals_first_character )
{
    const char* argv[] = {"foo", "--envs", "=foo"};

    BOOST_REQUIRE_THROW(
            Parser options( 3, const_cast<char**>(argv)),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( space_first_character )
{
    const char* argv[] = {"foo", "--envs", " foo=foo"};

    BOOST_REQUIRE_THROW(
            Parser options( 3, const_cast<char**>(argv)),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( number_first_character )
{
    const char* argv[] = {"foo", "--envs", "1foo=foo"};

    BOOST_REQUIRE_THROW(
            Parser options( 3, const_cast<char**>(argv)),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( underscore_first_character )
{
    _argv = make_argv("--envs _foo=bar", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            "_foo"
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            "bar"
            );
}

BOOST_AUTO_TEST_CASE( line_feed )
{
    const std::string key( "TEST_ENV" );
    const std::string value( "hello\nworld" );
    _argv = make_argv("--envs " + key + "=" + value, _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            key
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            value
            );
}

BOOST_AUTO_TEST_CASE( single_env_max_size )
{
    std::string args("--envs foo=");
    for (int32_t i = 0; i < bgcios::jobctl::MaxVariableSize - 6; ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_NO_THROW(
            Parser(_argc, _argv)
            );
}

BOOST_AUTO_TEST_CASE( exe_with_single_arg_too_large )
{
    std::string args("--envs foo=");
    for (int32_t i = 0; i < bgcios::jobctl::MaxVariableSize - 5; ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_SUITE_END()

