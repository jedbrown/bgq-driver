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
#define BOOST_TEST_MODULE block
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "client/Job.h"

#include "common/Exception.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>

#include <boost/program_options/errors.hpp>

#include <boost/asio.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( block, Fixture )

BOOST_AUTO_TEST_CASE( missing_parameter_arg )
{
    // --block requires an argument
    _argv = make_argv("--block", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    // ensure --block sets the block name
    _argv = make_argv("--block foo", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getBlock(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( arg_too_large )
{
    std::string args("--block ");
    BGQDB::DBTBlock block;
    for (uint32_t i = 0; i <= sizeof( block._blockid ); ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    // ensure RUNJOB_BLOCK sets the block name
    int replace = 1;
    setenv("RUNJOB_BLOCK", "foo", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getBlock(),
            "foo"
            );
}

BOOST_AUTO_TEST_CASE( duplicate_arg )
{
    // duplicate arguments should be flagged as an error
    _argv = make_argv("--block foo --block bar", _argc);
    BOOST_CHECK_THROW(
            Parser(_argc, _argv),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_CASE( env_and_arg )
{
    // ensure --block overrides RUNJOB_BLOCK
    int replace = 1;
    setenv("RUNJOB_BLOCK", "foo", replace);
    _argv = make_argv("--block bar", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getBlock(),
            "bar"
            );
}

BOOST_AUTO_TEST_SUITE_END()

