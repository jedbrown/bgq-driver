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
#define BOOST_TEST_MODULE socket_test

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/test/unit_test.hpp>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_array.hpp>

#include <sys/un.h>

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( socket_test, Fixture )

BOOST_AUTO_TEST_CASE( missing_arg )
{
    _argv = make_argv("--socket", _argc);
    BOOST_REQUIRE_THROW(
            Parser(_argc, _argv),
            boost::program_options::invalid_command_line_syntax
            );
}

BOOST_AUTO_TEST_CASE( arg )
{
    _argv = make_argv("--socket /foo/bar/baz", _argc);
    Parser options(_argc, _argv);
    const std::string socket = options.getSocket();
    BOOST_CHECK_EQUAL(
            socket,
            "/foo/bar/baz"
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    int replace = 1;
    setenv("RUNJOB_SOCKET", "/foo/bar/baz", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    const std::string socket = options.getSocket();
    BOOST_CHECK_EQUAL(
            socket,
            "/foo/bar/baz"
            );
    unsetenv("RUNJOB_SOCKET");
}

BOOST_AUTO_TEST_CASE( properties )
{
    _argv = make_argv("--properties socket.properties", _argc);
    Parser options(_argc, _argv);
    const std::string socket = options.getSocket();
    BOOST_CHECK_EQUAL(
            socket,
            "/foobar"
            );
}

BOOST_AUTO_TEST_CASE( arg_override_properties )
{
    _argv = make_argv("--socket /foo/bar/baz --properties socket.properties", _argc);
    Parser options(_argc, _argv);
    const std::string socket = options.getSocket();
    BOOST_CHECK_EQUAL(
            socket,
            "/foo/bar/baz"
            );
}

BOOST_AUTO_TEST_CASE( arg_override_env )
{
    int replace = 1;
    setenv("RUNJOB_SOCKET", "/foobar", replace);
    _argv = make_argv("--socket /foo/bar/baz", _argc);
    Parser options(_argc, _argv);
    const std::string socket = options.getSocket();
    BOOST_CHECK_EQUAL(
            socket,
            "/foo/bar/baz"
            );
    unsetenv("RUNJOB_SOCKET");
}

BOOST_AUTO_TEST_CASE( too_long )
{
    sockaddr_un size;
    boost::scoped_array<char> path(new char[ sizeof(size.sun_path) ]);
    for ( unsigned i=0; i < sizeof(size.sun_path); ++i ) {
        path[i] = 'a';
    }

    const char* argv[] = {"foo", "--block", "foo", "--exe", "bar", "--socket", path.get() };
    BOOST_CHECK_THROW(
            Parser( 7, const_cast<char**>(argv) ),
            boost::program_options::invalid_option_value
            );
}

BOOST_AUTO_TEST_SUITE_END()

