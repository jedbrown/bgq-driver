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
#define BOOST_TEST_MODULE cwd

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( cwd, Fixture )

BOOST_AUTO_TEST_CASE( default_value )
{
    // ensure a missing --cwd argument is equal to our cwd
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getCwd(),
            boost::filesystem::current_path()
            );
}

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--cwd", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    // ensure a --cwd argument sets our cwd
    _argv = make_argv("--cwd /foo/bar/baz", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getCwd(),
            "/foo/bar/baz"
            );
}

BOOST_AUTO_TEST_CASE( largest_single_arg )
{
    std::string args("--cwd /");
    for (int32_t i = 0; i < bgcios::jobctl::MaxPathSize - 2; ++i) {
        args.append( boost::lexical_cast<std::string>(i % 10) );
    }
    _argv = make_argv( args.c_str(), _argc);
    BOOST_CHECK_NO_THROW(
            Parser(_argc, _argv)
            );
}

BOOST_AUTO_TEST_CASE( single_arg_too_large )
{
    std::string args("--cwd ");
    for (int32_t i = 0; i < bgcios::jobctl::MaxPathSize; ++i) {
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
    // ensure RUNJOB_CWD sets the cwd
    int replace = 1;
    setenv("RUNJOB_CWD", "/foo/bar/baz", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_CHECK_EQUAL(
            options.getJobInfo().getCwd(),
            "/foo/bar/baz"
            );
}

BOOST_AUTO_TEST_SUITE_END()

