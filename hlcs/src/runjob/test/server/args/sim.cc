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
#define BOOST_TEST_MODULE sim
#include <boost/test/unit_test.hpp>

#include "common/defaults.h"

#include "server/Options.h"

#include "test/server/args/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::server;

BOOST_FIXTURE_TEST_SUITE( sim, Fixture )

BOOST_AUTO_TEST_CASE( properties )
{
    const std::string file( "server.properties" );
    bgq::utility::Properties properties( file );
    _argv = make_argv("--properties " + file, _argc);
    Options options(_argc, _argv);

    const std::string value( properties.getValue("runjob.server", "job_sim") );
    bool flag;
    std::istringstream is( value );
    is >> std::boolalpha >> flag;

    BOOST_REQUIRE_EQUAL(
            options.getSim(),
            flag
            );
}

BOOST_AUTO_TEST_CASE( garbage_properties )
{
    _argv = make_argv("--properties garbage.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getSim(),
            runjob::defaults::ServerJobSimulation
            );
}

BOOST_AUTO_TEST_CASE( arg_override_properties )
{
    _argv = make_argv("--properties server.properties --sim true --reconnect blocks", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getSim(),
            true
            );
}

BOOST_AUTO_TEST_CASE( garbage_value )
{
    _argv = make_argv("--sim foo", _argc);
    BOOST_REQUIRE_THROW(
            Options(_argc, _argv),
            boost::program_options::validation_error
            );
}

BOOST_AUTO_TEST_CASE( enable )
{
    _argv = make_argv("--properties server.properties --sim true --reconnect blocks", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getSim(),
            true
            );
}

BOOST_AUTO_TEST_CASE( disable )
{
    _argv = make_argv("--properties server.properties --sim false", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getSim(),
            false
            );
}

BOOST_AUTO_TEST_CASE( reconnect )
{
    _argv = make_argv("--properties server.properties --sim true --reconnect jobs", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.reconnect().scope(),
            Reconnect::Scope::Blocks
            );
}

BOOST_AUTO_TEST_SUITE_END()
