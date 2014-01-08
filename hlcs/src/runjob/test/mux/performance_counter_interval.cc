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
#define BOOST_TEST_MODULE performance_counter_interval
#include <boost/test/unit_test.hpp>

#include "mux/performance/Interval.h"

#include "common/properties.h"
#include "common/defaults.h"

#include "mux/Options.h"

#include "test/mux/Fixture.h"
#include "test/make_argv.h"

#include <boost/lexical_cast.hpp>

using namespace runjob::mux;

BOOST_FIXTURE_TEST_SUITE( socket_test, Fixture )

BOOST_AUTO_TEST_CASE( properties )
{
    _argv = make_argv("--properties good.properties", _argc);
    const Options options(_argc, _argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            boost::lexical_cast<unsigned>( options.getProperties()->getValue(PropertiesSection, "performance_counter_interval") )
            );
}

BOOST_AUTO_TEST_CASE( zero_properties )
{
    _argv = make_argv("--properties zero.properties", _argc);
    const Options options(_argc, _argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            runjob::defaults::MuxPerfCounterInterval
            );
}

BOOST_AUTO_TEST_CASE( negative_properties )
{
    _argv = make_argv("--properties negative.properties", _argc);
    const Options options(_argc, _argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            runjob::defaults::MuxPerfCounterInterval
            );
}

BOOST_AUTO_TEST_CASE( garbage_properties )
{
    _argv = make_argv("--properties garbage.properties", _argc);
    const Options options(_argc, _argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            runjob::defaults::MuxPerfCounterInterval
            );
}

BOOST_AUTO_TEST_SUITE_END()
