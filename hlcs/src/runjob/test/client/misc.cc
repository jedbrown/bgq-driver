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
#define BOOST_TEST_MODULE misc
#include <boost/test/unit_test.hpp>

#include "client/options/Parser.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::client::options;

BOOST_FIXTURE_TEST_SUITE( misc, Fixture )

BOOST_AUTO_TEST_CASE( invalid_env )
{
    // invalid RUNJOB_ arg should not cause a parsing failure
    int replace = 1;
    BOOST_REQUIRE_EQUAL(
            setenv("RUNJOB_FOOBAR", "foo=bar", replace),
            0
            );

    _argv = make_argv("", _argc);
    BOOST_REQUIRE_NO_THROW(
            Parser options(_argc, _argv)
            );
}

BOOST_AUTO_TEST_SUITE_END()

