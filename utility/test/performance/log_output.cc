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
#define BOOST_TEST_MODULE log_output
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

BOOST_AUTO_TEST_CASE( output )
{
    // create persistent count set
    std::string container( "test" );
    PERFORMANCE_LIST_INIT( container );
    typedef PersistentStatisticList::Timer Timer;

    // time something
    {
        Timer timer( container, "test_metric", "123", Mode::Value::Basic );
    }

    // get statistics
    PersistentStatisticList::Ptr stats = GlobalStatistics::instance().get<PersistentStatisticList::Ptr>( container );

    // ensure we have one data point
    BOOST_CHECK_EQUAL(
            stats->getCount(),
            1u
            );

    // output them
    stats->output();

    // ensure we have no data points
    BOOST_CHECK_EQUAL(
            stats->getCount(),
            0u
            );
}

BOOST_AUTO_TEST_CASE( units )
{
    // create container
    typedef StatisticSet< List< DataPoint > > Container;
    Container::Ptr counters(
            new Container("unit_test")
            );

    // iterate through all possible units
    for ( unsigned int i = 0; i < static_cast<unsigned>( LogOutput::Units::NumValues ); ++i ) {
        // cast loop index to unit value
        const LogOutput::Units::Value value = static_cast<LogOutput::Units::Value>(i);

        // time something
        {
            Container::Timer::Ptr timer = counters->create()
                ->function( "some_function" )
                ->id( 123 )
                ;
        }

        // set units
        counters->setUnits( value );

        // validate we set the units
        BOOST_CHECK_EQUAL(
                counters->getUnits(),
                value
                );

        // output
        counters->output();
    }
}

