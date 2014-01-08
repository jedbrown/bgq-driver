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

#include "server/performance/Interval.h"

#include "common/properties.h"
#include "common/defaults.h"

#include "server/Options.h"

#include "test/make_argv.h"

#include <boost/lexical_cast.hpp>

using namespace runjob::server;

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( properties )
{
    unsigned argc;
    char** argv = make_argv("--properties good.properties", argc);
    const Options options(argc, argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            boost::lexical_cast<unsigned>( options.getProperties()->getValue(PropertiesSection, "performance_counter_interval") )
            );
}

BOOST_AUTO_TEST_CASE( zero_properties )
{
    unsigned argc;
    char** argv = make_argv("--properties zero.properties", argc);
    const Options options(argc, argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            runjob::defaults::ServerPerfCounterInterval
            );
}

BOOST_AUTO_TEST_CASE( negative_properties )
{
    unsigned argc;
    char** argv = make_argv("--properties negative.properties", argc);
    const Options options(argc, argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            runjob::defaults::ServerPerfCounterInterval
            );
}

BOOST_AUTO_TEST_CASE( garbage_properties )
{
    unsigned argc;
    char ** argv = make_argv("--properties garbage.properties", argc);
    const Options options(argc, argv);
    const performance::Interval interval( options );
    BOOST_REQUIRE_EQUAL(
            interval.get(),
            runjob::defaults::ServerPerfCounterInterval
            );
}
