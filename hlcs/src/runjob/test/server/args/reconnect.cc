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
#define BOOST_TEST_MODULE reconnect
#include <boost/test/unit_test.hpp>

#include "common/defaults.h"

#include "server/Options.h"

#include "test/server/args/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::server;

BOOST_FIXTURE_TEST_SUITE( reconnect, Fixture )

BOOST_AUTO_TEST_CASE( none )
{
    _argv = make_argv("--reconnect none", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.reconnect().scope(),
            Reconnect::Scope::None
            );
}

BOOST_AUTO_TEST_CASE( blocks )
{
    _argv = make_argv("--reconnect blocks", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.reconnect().scope(),
            Reconnect::Scope::Blocks
            );
}

BOOST_AUTO_TEST_CASE( jobs )
{
    _argv = make_argv("--reconnect jobs", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.reconnect().scope(),
            Reconnect::Scope::Jobs
            );
}

BOOST_AUTO_TEST_CASE( garbage )
{
    _argv = make_argv("--reconnect asdfasdf", _argc);
    BOOST_REQUIRE_THROW(
            Options(_argc, _argv),
            boost::program_options::validation_error
            );
}

BOOST_AUTO_TEST_CASE( implicit_value )
{
    _argv = make_argv("--reconnect", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.reconnect().scope(),
            Reconnect::Scope::Jobs
            );
}

BOOST_AUTO_TEST_CASE( default_value )
{
    _argv = make_argv("", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.reconnect().scope(),
            Reconnect::Scope::Jobs
            );
}

BOOST_AUTO_TEST_SUITE_END()
