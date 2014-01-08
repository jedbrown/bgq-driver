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
#define BOOST_TEST_MODULE ranks_per_node
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "common/defaults.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( ranks_per_node, Fixture )

BOOST_AUTO_TEST_CASE( default_value )
{
    // ensure a missing --ranks-per-node argument uses the default value
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            runjob::defaults::ClientRanksPerNode
            );
}

BOOST_AUTO_TEST_CASE( missing_arg )
{
    // --ranks-per-node requires a value
    _argv = make_argv("--ranks-per-node", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( missing_arg_short )
{
    // --ranks-per-node requires a value
    _argv = make_argv("-p", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( valid_values_env )
{
    // ensure underscore characters in environment variable name are correctly
    // matched to the dashes used in ranks-per-node
    const int replace = 1;
    for( unsigned i = 1; i <= 64; i *= 2 ) {
        const std::string env( boost::lexical_cast<std::string>(i) );
        setenv( "RUNJOB_RANKS_PER_NODE", env.c_str(), replace );
        _argv = make_argv( "", _argc );
        Parser options(_argc, _argv);
        BOOST_REQUIRE_EQUAL(
                options.getJobInfo().getRanksPerNode(),
                i
                );
    }
}

BOOST_AUTO_TEST_CASE( valid_values_arg_1 )
{
    _argv = make_argv( "--ranks-per-node 1", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            1u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_1 )
{
    _argv = make_argv( "-p 1", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            1u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_2 )
{
    _argv = make_argv( "--ranks-per-node 2", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            2u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_2 )
{
    _argv = make_argv( "-p 2", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            2u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_4 )
{
    _argv = make_argv( "--ranks-per-node 4", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            4u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_4 )
{
    _argv = make_argv( "-p 4", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            4u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_8 )
{
    _argv = make_argv( "--ranks-per-node 8", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            8u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_8 )
{
    _argv = make_argv( "-p 8", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            8u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_16 )
{
    _argv = make_argv( "--ranks-per-node 16", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            16u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_16 )
{
    _argv = make_argv( "-p 16", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            16u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_32 )
{
    _argv = make_argv( "--ranks-per-node 32", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            32u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_32 )
{
    _argv = make_argv( "-p 32", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            32u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_64 )
{
    _argv = make_argv( "--ranks-per-node 64", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            64u
            );
}

BOOST_AUTO_TEST_CASE( valid_values_arg_short_64 )
{
    _argv = make_argv( "-p 64", _argc );
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getJobInfo().getRanksPerNode(),
            64u
            );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_0 )
{
    _argv = make_argv( "--ranks-per-node 0", _argc );
    BOOST_REQUIRE_THROW(
        Parser(_argc, _argv),
        boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_128 )
{
    // 128 is a power of 2, but larger than 64
    _argv = make_argv( "--ranks-per-node 128", _argc );
    BOOST_REQUIRE_THROW(
        Parser(_argc, _argv),
        boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_100 )
{
    // 100 is not a power of 2
    _argv = make_argv( "--ranks-per-node 100", _argc );
    BOOST_REQUIRE_THROW(
        Parser(_argc, _argv),
        boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_CASE( invalid_values_arg_asdf )
{
    //asdf is not a number
    _argv = make_argv( "--ranks-per-node asdf", _argc );
    BOOST_REQUIRE_THROW(
        Parser(_argc, _argv),
        boost::program_options::invalid_option_value
        );
}

BOOST_AUTO_TEST_SUITE_END()
