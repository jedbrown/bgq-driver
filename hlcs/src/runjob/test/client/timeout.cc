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
#define BOOST_TEST_MODULE timeout
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/lexical_cast.hpp>
#include <boost/program_options/errors.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( timeout, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    // no --timeout should be 0
    _argv = make_argv("", _argc);
    const Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getTimeout(),
            0
            );
}

BOOST_AUTO_TEST_CASE( empty_arg )
{
    _argv = make_argv("--timeout", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( zero )
{
    _argv = make_argv("--timeout 0", _argc);
    BOOST_CHECK_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value 
            );
}

BOOST_AUTO_TEST_CASE( negative )
{
    _argv = make_argv("--timeout -1", _argc);
    BOOST_CHECK_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value 
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    _argv = make_argv("--timeout 15", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getTimeout(),
            15
            );
}

BOOST_AUTO_TEST_CASE( duplicate_arg )
{
    _argv = make_argv("--timeout 15 --timeout 30", _argc);
    BOOST_CHECK_THROW(
            Parser(_argc, _argv),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    int32_t value = 300;
    int replace = 1;
    setenv("RUNJOB_TIMEOUT", boost::lexical_cast<std::string>(value).c_str(), replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getTimeout(),
            value
            );
    unsetenv("RUNJOB_TIMEOUT");
}

BOOST_AUTO_TEST_CASE( arg_env )
{
    int replace = 1;
    BOOST_CHECK_EQUAL(
            setenv("RUNJOB_TIMEOUT", "30", replace),
            0
            );

    _argv = make_argv("--timeout 15", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getTimeout(),
            15
            );
    unsetenv("RUNJOB_TIMEOUT");
}

BOOST_AUTO_TEST_SUITE_END()

