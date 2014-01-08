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
#define BOOST_TEST_MODULE thread_pool_size
#include <boost/test/unit_test.hpp>

#include "common/defaults.h"

#include "mux/Options.h"

#include "test/mux/Fixture.h"
#include "test/make_argv.h"

using namespace runjob::mux;

BOOST_FIXTURE_TEST_SUITE( thread_pool_size, Fixture )

BOOST_AUTO_TEST_CASE( properties )
{
    _argv =  make_argv("--properties good.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getThreadPoolSize(),
            8u
            );
}

BOOST_AUTO_TEST_CASE( garbage_value )
{
    _argv =  make_argv("--properties garbage.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getThreadPoolSize(),
            static_cast<unsigned>(NUM_PROCS)
            );
}

BOOST_AUTO_TEST_CASE( default_value )
{
    _argv = make_argv("", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getThreadPoolSize(),
            static_cast<unsigned>(NUM_PROCS)
            );
}

BOOST_AUTO_TEST_CASE( negative_value )
{
    _argv =  make_argv("--properties negative.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getThreadPoolSize(),
            static_cast<unsigned>(NUM_PROCS)
            );
}

BOOST_AUTO_TEST_CASE( zero_value )
{
    _argv =  make_argv("--properties zero.properties", _argc);
    Options options(_argc, _argv);
    BOOST_REQUIRE_EQUAL(
            options.getThreadPoolSize(),
            static_cast<unsigned>(NUM_PROCS)
            );
}

BOOST_AUTO_TEST_SUITE_END()
