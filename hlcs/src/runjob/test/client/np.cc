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
#define BOOST_TEST_MODULE np
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "common/defaults.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/assign/list_of.hpp>

#include <boost/asio.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( np, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    // no --np should be 0
    _argv = make_argv("", _argc);
    const Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getNp(),
            0
            );
}

BOOST_AUTO_TEST_CASE( empty_arg )
{
    // --np-per-node requires a value
    _argv = make_argv("--np", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( valid_value_arg )
{
    _argv = make_argv( "--np 50", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getNp(),
            50
            );
}

BOOST_AUTO_TEST_CASE( valid_value_arg_short )
{
    _argv = make_argv( "-n 50", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getNp(),
            50
            );
}

BOOST_AUTO_TEST_CASE( valid_value_env )
{
    const int replace = 1;
    const int value = 75;
    const std::string env( boost::lexical_cast<std::string>(value) );
    setenv( "RUNJOB_NP", env.c_str(), replace );
    _argv = make_argv( "", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getNp(),
            value
            );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_negative )
{
    // asdf is not a number
    _argv = make_argv( "--np -1", _argc );
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_0 )
{
    // asdf is not a number
    _argv = make_argv( "--np 0", _argc );
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_asdf )
{
    // asdf is not a number
    _argv = make_argv( "--np asdf", _argc );
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_SUITE_END()
