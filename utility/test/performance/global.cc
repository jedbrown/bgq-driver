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
#define BOOST_TEST_MODULE global
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

        // initialize performance API
        performance::init( Properties::create( "good.properties" ) );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( get_all )
{
    // create two persistent statistic sets.
    std::string container_one( "test_one" );
    std::string container_two( "test_two" );
    PERFORMANCE_LIST_INIT( container_one );
    PERFORMANCE_LIST_INIT( container_two );
    typedef PersistentStatisticList::Timer Timer;

    // time something
    {
        Timer timer( container_one, "test_metric", "123", Mode::Value::Basic );
    }

    // time something else
    {
        Timer timer( container_two, "test_metric", "456", Mode::Value::Basic );
    }

    // get statistics
    GlobalStatistics& global = GlobalStatistics::instance();
    {
        std::list<PersistentStatisticList::Ptr> stats;
        global.get( stats );

        // ensure we have two
        BOOST_CHECK_EQUAL(
                stats.size(),
                2u
                );
    }

    // ensure container_one has one statistic
    {
        PersistentStatisticList::Ptr stats;
        global.get( container_one, stats );
        BOOST_CHECK_EQUAL(
                stats->getCount(),
                1u
                );
    }

    // ensure container_two has one statistic
    {
        PersistentStatisticList::Ptr stats;
        global.get( container_two, stats );
        BOOST_CHECK_EQUAL(
                stats->getCount(),
                1u
                );
    }
}
