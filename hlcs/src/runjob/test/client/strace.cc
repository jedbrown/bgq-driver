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
#define BOOST_TEST_MODULE strace
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "common/defaults.h"
#include "common/Strace.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( strace, Fixture )

BOOST_AUTO_TEST_CASE( default_value )
{
    // ensure a missing --strace argument uses the default value
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getStrace().getScope(),
            runjob::Strace::None
            );
}

BOOST_AUTO_TEST_CASE( empty_parameter_arg )
{
    const char* argv[] = {"foo", "--strace", ""};
    BOOST_REQUIRE_THROW(
            Parser( 3, const_cast<char**>(argv) ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_CASE( explicit_none )
{
    _argv = make_argv("--strace none", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getStrace().getScope(),
            runjob::Strace::None
            );
}

BOOST_AUTO_TEST_CASE( specific_node )
{
    _argv = make_argv("--strace 1", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getStrace().getScope(),
            runjob::Strace::Node
            );
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getStrace().getRank(),
            1u
            );
}

BOOST_AUTO_TEST_CASE( garbage )
{
    _argv = make_argv("--strace asdf", _argc);
    BOOST_CHECK_THROW(
            Parser options(_argc, _argv),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_SUITE_END()
