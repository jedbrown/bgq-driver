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
#define BOOST_TEST_MODULE label

#include "client/options/Label.h"
#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/test/unit_test.hpp>

#include <cstdlib>

#include <unistd.h>

using namespace runjob::client::options;


BOOST_FIXTURE_TEST_SUITE( label, Fixture )

BOOST_AUTO_TEST_CASE( implicit_value )
{
    _argv = make_argv("--label", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLabel().getScope(),
            Label::Long
            );
}

BOOST_AUTO_TEST_CASE( explicit_none )
{
    _argv = make_argv("--label none", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLabel().getScope(),
            Label::None
            );
}

BOOST_AUTO_TEST_CASE( explicit_short )
{
    _argv = make_argv("--label short", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLabel().getScope(),
            Label::Short
            );
}

BOOST_AUTO_TEST_CASE( explicit_long )
{
    _argv = make_argv("--label long", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLabel().getScope(),
            Label::Long
            );
}

BOOST_AUTO_TEST_CASE( env )
{
    int replace = 1;
    setenv("RUNJOB_LABEL", "long", replace);
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLabel().getScope(),
            Label::Long
            );
    unsetenv("RUNJOB_LABEL");
}

BOOST_AUTO_TEST_CASE( default_none )
{
    _argv = make_argv("", _argc);
    Parser options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getLabel().getScope(),
            Label::None
            );
}

BOOST_AUTO_TEST_SUITE_END()

