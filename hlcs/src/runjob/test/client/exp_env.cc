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
#define BOOST_TEST_MODULE exp_env
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( exp_env, Fixture )

BOOST_AUTO_TEST_CASE( empty_parameter_arg )
{
    const char* argv[] = {"foo", "--exp-env", ""};
    BOOST_REQUIRE_THROW(
            Parser( 3, const_cast<char**>(argv) ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--exp-env", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( missing_environment )
{
    _argv = make_argv("--exp-env TEST_ENV", _argc);
    BOOST_CHECK_THROW(
            Parser options(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    const int replace = 1;
    setenv("TEST_ENV", "foo", replace);
    _argv = make_argv("--exp-env TEST_ENV", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            "TEST_ENV"
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( multitoken )
{
    const int replace = 1;
    setenv("foo", "hello world", replace);
    setenv("bar", "hello world", replace);
    setenv("baz", "hello world", replace);
    _argv = make_argv("--exp-env foo bar --exp-env baz", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            3u
            );
}

BOOST_AUTO_TEST_CASE( composing )
{
    const int replace = 1;
    setenv("RUNJOB_EXP_ENV", "foo", replace);
    setenv("foo", "hello world", replace);
    setenv("bar", "hello world", replace);
    _argv = make_argv("--exp-env bar", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getEnvs().size(),
            2u
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    const int replace = 1;
    setenv("TEST_ENV", "foo", replace);
    setenv("RUNJOB_EXP_ENV", "TEST_ENV", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            "TEST_ENV"
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( duplicate )
{
    const int replace = 1;
    setenv("TEST_ENV", "foo", replace);
    _argv = make_argv("--exp-env TEST_ENV --envs TEST_ENV=bar", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( space )
{
    const int replace = 1;
    setenv("TEST_ENV", "foo bar", replace);
    _argv = make_argv("--exp-env TEST_ENV", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            "TEST_ENV"
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            "foo bar"
            );
}

BOOST_AUTO_TEST_CASE( equals )
{
    const int replace = 1;
    setenv("TEST_ENV", "foo=bar", replace);
    _argv = make_argv("--exp-env TEST_ENV", _argc);
    Parser options(_argc, _argv);
    const runjob::JobInfo::EnvironmentVector& envs = options.getJobInfo().getEnvs();
    const runjob::Environment& env = envs.at(0);
    BOOST_REQUIRE_EQUAL(
            env.getKey(),
            "TEST_ENV"
            );
    BOOST_REQUIRE_EQUAL(
            env.getValue(),
            "foo=bar"
            );
}

BOOST_AUTO_TEST_CASE( line_feed )
{
    const int replace = 1;
    const std::string key( "TEST_ENV" );
    const std::string value( "hello\nworld" );
    setenv( key.c_str(), value.c_str(), replace );
    _argv = make_argv("--exp-env " + key, _argc);
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

BOOST_AUTO_TEST_SUITE_END()

