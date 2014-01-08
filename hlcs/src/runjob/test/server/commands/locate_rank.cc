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
#define BOOST_TEST_MODULE locate_rank
#include <boost/test/unit_test.hpp>

#include "server/commands/locate_rank/Options.h"

#include "test/server/commands/Fixture.h"

#include "test/make_argv.h"

#include <boost/program_options/errors.hpp>

using namespace runjob::server::commands;

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_SUITE( locate_rank_test, Fixture )

BOOST_AUTO_TEST_CASE( missing_id )
{
    _argv = make_argv("--id", _argc);
    BOOST_REQUIRE_THROW(
            locate_rank::Options(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( non_numeric_id )
{
    _argv = make_argv("--id foo", _argc);
    BOOST_REQUIRE_THROW(
            locate_rank::Options(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( negative_id )
{
    _argv = make_argv("--id -15", _argc);
    BOOST_REQUIRE_THROW(
            locate_rank::Options(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( positive_id )
{
    _argv = make_argv("--id 15", _argc);
    locate_rank::Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobId(),
            15u
            );
}

BOOST_AUTO_TEST_CASE( positional_id )
{
    _argv = make_argv("15", _argc);
    locate_rank::Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobId(),
            15u
            );
}

BOOST_AUTO_TEST_CASE( missing_rank )
{
    _argv = make_argv("--rank", _argc);
    BOOST_REQUIRE_THROW(
            locate_rank::Options(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( non_numeric_rank )
{
    _argv = make_argv("--rank foo", _argc);
    BOOST_REQUIRE_THROW(
            locate_rank::Options(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( negative_rank )
{
    _argv = make_argv("--rank -15", _argc);
    BOOST_REQUIRE_THROW(
            locate_rank::Options(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( positive_rank )
{
    _argv = make_argv("--rank 15", _argc);
    locate_rank::Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.rank(),
            15u
            );
}

BOOST_AUTO_TEST_CASE( positional_rank )
{
    _argv = make_argv("15 27", _argc);
    locate_rank::Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobId(),
            15u
            );
    BOOST_REQUIRE_EQUAL(
            options.rank(),
            27u
            );
    BOOST_REQUIRE_NO_THROW( options.validate() );
}

BOOST_AUTO_TEST_CASE( job_id_required )
{
    _argv = make_argv("", _argc);
    locate_rank::Options options(_argc, _argv);
    BOOST_REQUIRE_THROW(
            options.validate(),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_CASE( rank_required )
{
    _argv = make_argv("15", _argc);
    locate_rank::Options options(_argc, _argv);
    BOOST_REQUIRE_THROW(
            options.validate(),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_SUITE_END()
