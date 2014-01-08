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
#define BOOST_TEST_MODULE version
#include <boost/test/unit_test.hpp>

#include "server/Options.h"

#include "test/server/args/Fixture.h"
#include "test/make_argv.h"

#include <boost/program_options/errors.hpp>

using namespace runjob::server;

BOOST_FIXTURE_TEST_SUITE( version, Fixture )

BOOST_AUTO_TEST_CASE( arg )
{
    _argv = make_argv("--version", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getVersion(),
            true
            );
}

BOOST_AUTO_TEST_CASE( arg_short )
{
    _argv = make_argv("-v", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getVersion(),
            true
            );
}

BOOST_AUTO_TEST_CASE( both_args )
{
    _argv = make_argv("--version -v", _argc);
    BOOST_REQUIRE_THROW(
            Options(_argc, _argv),
            boost::program_options::multiple_occurrences
            );
}

BOOST_AUTO_TEST_SUITE_END()
