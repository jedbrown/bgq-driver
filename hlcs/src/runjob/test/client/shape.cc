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
#define BOOST_TEST_MODULE shape
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "common/Exception.h"
#include "common/Shape.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/program_options/errors.hpp>

#include <boost/asio.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( shape, Fixture )

BOOST_AUTO_TEST_CASE( missing_parameter_arg )
{
    // --shape requires an argument
    _argv = make_argv("--shape", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( empty_parameter_arg )
{
    const char* argv[] = {"foo", "--shape", ""};
    BOOST_REQUIRE_THROW(
            Parser( 3, const_cast<char**>(argv) ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    // ensure --shape sets the shape
    _argv = make_argv("--shape 1x1x1x1x1 --corner R00-M0-N00-J00", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            (std::string)options.getJobInfo().getSubBlock().shape(),
            "1x1x1x1x1"
            );
}

BOOST_AUTO_TEST_CASE( arg_no_corner )
{
    // ensure --shape without --corner throws an exception
    _argv = make_argv("--shape 1x1x1x1x1", _argc);
    BOOST_REQUIRE_THROW(
        Parser options(_argc, _argv),
        boost::program_options::error
        );
}

BOOST_AUTO_TEST_CASE( env )
{
    // ensure RUNJOB_SHAPE sets the block name
    int replace = 1;
    setenv("RUNJOB_SHAPE", "1x1x1x1x1", replace);
    _argv = make_argv("--corner R00-M0-N00-J00", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            (std::string)options.getJobInfo().getSubBlock().shape(),
            "1x1x1x1x1"
            );
}

BOOST_AUTO_TEST_CASE( duplicate_arg )
{
    // duplicate arguments should be flagged as an error
    _argv = make_argv("--shape 1x1x1x1x1 --shape 2x2x2x2x2", _argc);
    BOOST_CHECK_THROW(
            Parser(_argc, _argv),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_CASE( env_and_arg )
{
    // ensure --shape overrides RUNJOB_SHAPE
    int replace = 1;
    setenv("RUNJOB_SHAPE", "1x1x1x1x1", replace);
    _argv = make_argv("--shape 2x2x2x2x2 --corner R00-M0-N00-J00", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            (std::string)options.getJobInfo().getSubBlock().shape(),
            "2x2x2x2x2"
            );
}

BOOST_AUTO_TEST_SUITE_END()

