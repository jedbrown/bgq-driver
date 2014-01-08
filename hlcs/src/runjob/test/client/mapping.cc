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
#define BOOST_TEST_MODULE mapping

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include "mapping_permutations.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <boost/test/unit_test.hpp>
#include <boost/scoped_array.hpp>

#include <iostream>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( mapping, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--mapping", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg_permutation )
{
    runjob::Mapping m;
    for ( unsigned i = 0; i < 720; ++i ) {
        std::istringstream is( permutations[i] );
        const bool rc = is >> m;
        BOOST_CHECK_EQUAL(
                rc,
                true
                );
    }
}

BOOST_AUTO_TEST_CASE( arg_relative_file )
{
    _argv = make_argv( "--mapping mapping_file", _argc);
    const Parser options(_argc, _argv);
    char buf[ 512 ];
    (void)getcwd( buf, sizeof(buf) );
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getMapping().value(),
            std::string(buf) + "/mapping_file"
            );
}

BOOST_AUTO_TEST_CASE( arg_too_large )
{
    std::string args("--mapping ");
    BGQDB::DBTJob job;
    for (uint32_t i = 0; i <= sizeof( job._mapping ); ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( arg_relative_too_large )
{
    std::string args("--cwd /foooooo --mapping ");
    BGQDB::DBTJob job;
    for (uint32_t i = 0; i <= sizeof( job._mapping ) - 10; ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::error
            );
}

BOOST_AUTO_TEST_SUITE_END()

