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
#define BOOST_TEST_MODULE corner
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "common/Exception.h"
#include "common/Shape.h"
#include "common/SubBlock.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/program_options/errors.hpp>

#include <boost/asio.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( corner, Fixture )

BOOST_AUTO_TEST_CASE( missing_parameter_arg )
{
    // --corner requires an argument
    _argv = make_argv("--corner", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( empty_parameter_arg )
{
    const char* argv[] = {"foo", "--corner", ""};
    BOOST_REQUIRE_THROW(
            Parser( 3, const_cast<char**>(argv) ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    // ensure --corner sets the corner
    _argv = make_argv("--corner R00-M0-N00-J00 --shape 1x1x1x1x1", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            boost::lexical_cast<std::string>( options.getJobInfo().getSubBlock().corner() ),
            "R00-M0-N00-J00"
            );
}

BOOST_AUTO_TEST_CASE( arg_no_shape )
{
    // ensure --corner without --shape throws an exception
    _argv = make_argv("--corner R00-M0-N00-J00", _argc);
    BOOST_REQUIRE_THROW(
        Parser options(_argc, _argv),
        boost::program_options::error
        );
}

BOOST_AUTO_TEST_CASE( core_corner_with_valid_shape )
{
    // ensure a core corner location with a valid shape is not flagged as an error
    _argv = make_argv("--corner R00-M0-N00-J00-C00 --shape 1x1x1x1x1", _argc);
    BOOST_CHECK_NO_THROW(
            Parser options(_argc, _argv)
        );
}

BOOST_AUTO_TEST_CASE( core_corner_with_invalid_ranks_per_node )
{
    // ranks per node 2, 4, 8, 16, 32, or 64 should be ignored for single core jobs
    for ( unsigned i = 2; i <= 64; i *= 2 ) {
        _argv = make_argv("--corner R00-M0-N00-J00-C00 --ranks-per-node " + boost::lexical_cast<std::string>(i), _argc);
        const Parser options(_argc, _argv);
        BOOST_CHECK_EQUAL( options.getJobInfo().getRanksPerNode(), 1u );
    }
}

BOOST_AUTO_TEST_CASE( core_corner_with_invalid_shape )
{
    // any shape other than 1x1x1x1x1 should be overridden
    _argv = make_argv("--corner R00-M0-N00-J00-C00 --shape 2x1x1x1x1", _argc);
    Parser options(_argc, _argv);
    const runjob::Shape shape(
            options.getJobInfo().getSubBlock().shape()
        );
    const runjob::Shape expected_shape( "1x1x1x1x1" );
    BOOST_CHECK_EQUAL(
            std::string(shape),
            std::string(expected_shape)
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    // ensure RUNJOB_CORNER sets the block name
    int replace = 1;
    setenv("RUNJOB_CORNER", "R00-M0-N00-J00", replace);
    _argv = make_argv("--shape 1x1x1x1x1", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            boost::lexical_cast<std::string>( options.getJobInfo().getSubBlock().corner() ),
            "R00-M0-N00-J00"
            );
}

BOOST_AUTO_TEST_CASE( duplicate_arg )
{
    // duplicate arguments should be flagged as an error
    _argv = make_argv("--corner R00-M0-N00-J00 --corner R00-M0-N00-J01", _argc);
    BOOST_CHECK_THROW(
            Parser options(_argc, _argv),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_CASE( env_and_arg )
{
    // ensure --shape overrides RUNJOB_CORNER
    int replace = 1;
    setenv("RUNJOB_CORNER", "R00-M0-N00-J00", replace);
    _argv = make_argv("--corner R00-M0-N00-J05 --shape 1x1x1x1x1", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            boost::lexical_cast<std::string>( options.getJobInfo().getSubBlock().corner() ),
            "R00-M0-N00-J05"
            );
}

BOOST_AUTO_TEST_SUITE_END()

