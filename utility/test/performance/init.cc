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
#define BOOST_TEST_MODULE init
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/performance/PersistentStatistics.h"

#include "utility/include/performance.h"

using namespace bgq::utility::performance;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );
BOOST_AUTO_TEST_CASE( init_no )
{
    // init library with a bogus properties file
    init( bgq::utility::Properties::Ptr() );

    // create a persistent statistic set
    std::string container( "container_one" );
    PERFORMANCE_LIST_INIT( container );
    typedef PersistentStatisticList::Timer Timer;

    // time something
    {
        Timer timer( container, "test_metric", "123", Mode::Value::Basic );
    }

    // get statistics
    GlobalStatistics& global = GlobalStatistics::instance();
    {
        std::list<PersistentStatisticList::Ptr> stats;
        global.get( stats );

        // ensure we have two
        BOOST_CHECK_EQUAL(
                stats.size(),
                1u
                );
    }

    // ensure container has one statistic
    {
        PersistentStatisticList::Ptr stats;
        global.get( container, stats );
        BOOST_CHECK_EQUAL(
                stats->getCount(),
                1u
                );
    }
    
    // reset global statistics and mode
    GlobalStatistics::reset();
    Mode::reset();
}

BOOST_AUTO_TEST_CASE( init_yes )
{
    // initialize performance API
    init( bgq::utility::Properties::create( "good.properties" ) );

    // create a persistent statistic set
    std::string container( "container_one" );
    PERFORMANCE_LIST_INIT( container );
    typedef PersistentStatisticList::Timer Timer;

    // time something
    {
        Timer timer( container, "test_metric", "123", Mode::Value::Basic );
    }

    // get statistics
    GlobalStatistics& global = GlobalStatistics::instance();
    {
        std::list<PersistentStatisticList::Ptr> stats;
        global.get( stats );

        // ensure we have two
        BOOST_CHECK_EQUAL(
                stats.size(),
                1u
                );
    }

    // ensure container has one statistic
    {
        PersistentStatisticList::Ptr stats;
        global.get( container, stats );
        BOOST_CHECK_EQUAL(
                stats->getCount(),
                1u
                );
    }

    // reset global statistics and mode
    GlobalStatistics::reset();
    Mode::reset();
}


