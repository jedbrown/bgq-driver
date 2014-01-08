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
#define BOOST_TEST_MODULE persistent
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/performance/PersistentStatistics.h"

#include "utility/include/performance.h"

#include <boost/foreach.hpp>

using namespace bgq::utility::performance;
using namespace bgq::utility;

struct InitializeLoggingFixture
{
    InitializeLoggingFixture()
    {
        // initialize logging
        bgq::utility::initializeLogging( *Properties::create() );
    
        // initialize performance API
        _properties = Properties::create( "good.properties");
        performance::init( _properties );
    }

    Properties::ConstPtr _properties;
};

BOOST_FIXTURE_TEST_SUITE( persistent, InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( circular_buffer_size )
{
    // create persistent count set
    PersistentStatisticBuffer stats( "test" );

    // ensure buffer size is sane
    BOOST_CHECK_EQUAL(
            stats.getCapacity(),
            boost::lexical_cast<size_t>(
                _properties->getValue( "performance", "buffer_capacity" )
                )
            );
}

BOOST_AUTO_TEST_CASE( garbage_circular_buffer_size )
{
    // initialize API with bad properties file
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "garbage.properties" );
    performance::init( properties );

    // create persistent count set
    PersistentStatisticBuffer stats( "test" );

    // ensure buffer size is sane
    BOOST_CHECK_EQUAL(
            stats.getCapacity(),
            PersistentStatisticBuffer::DefaultBufferCapacity
            );
}

BOOST_AUTO_TEST_CASE( negative_circular_buffer_size )
{
    // initialize API with bad properties file
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "negative.properties" );
    performance::init( properties );

    // create persistent count set
    PersistentStatisticBuffer stats( "test" );

    // ensure buffer size is sane
    BOOST_CHECK_EQUAL(
            stats.getCapacity(),
            PersistentStatisticBuffer::DefaultBufferCapacity
            );
}

BOOST_AUTO_TEST_CASE( missing_circular_buffer_size )
{
    // initialize API with bad properties file
    bgq::utility::Properties::ConstPtr properties;
    properties = bgq::utility::Properties::create( "missing.properties" );
    performance::init( properties );

    // create persistent count set
    PersistentStatisticBuffer stats( "test" );

    // ensure buffer size is sane
    BOOST_CHECK_EQUAL(
            stats.getCapacity(),
            PersistentStatisticBuffer::DefaultBufferCapacity
            );
}

BOOST_AUTO_TEST_CASE( buffer_count )
{
    // create persistent count set
    std::string container( "test_buffer" );
    PERFORMANCE_BUFFER_INIT( container );
    typedef PersistentStatisticBuffer::Timer Timer;

    // time something
    {
        Timer timer( container, "test_metric", "123", Mode::Value::Basic );
    }

    // get statistics
    PersistentStatisticBuffer::Ptr stats = GlobalStatistics::instance().get<PersistentStatisticBuffer::Ptr>( container );

    // ensure we have 1 data point
    BOOST_CHECK_EQUAL(
            stats->getCount(),
            1u
            );

    // ensure getting datapoints empties container
    CircularBuffer<DataPoint>::Container dataPoints;
    stats->get( dataPoints );
    BOOST_CHECK_EQUAL(
            dataPoints.size(),
            1u
            );
    BOOST_CHECK_EQUAL(
            stats->getCount(),
            0u
            );

    // time something else
    {
        Timer timer( container, "test_metric", "123", Mode::Value::Basic );
    }
    BOOST_CHECK_EQUAL(
            stats->getCount(),
            1u
            );
}

BOOST_AUTO_TEST_CASE( buffer_values )
{
    // create persistent count set
    std::string name( "test_buffer_values" );
    PERFORMANCE_BUFFER_INIT( name );
    typedef PersistentStatisticBuffer::Timer Timer;

    // time something
    {
        Timer timer( name, "test_metric", "27", Mode::Value::Basic, "123");
        sleep(3); // sleep for 3 seconds
    }

    // get statistics
    PersistentStatisticBuffer::Ptr stats = GlobalStatistics::instance().get<PersistentStatisticBuffer::Ptr>( name );    
  
    // verify the DataPoint that was recorded
    CircularBuffer<DataPoint>::Container container;
    stats->get(container);
    BOOST_FOREACH( const DataPoint& dp, container ) {
        // ensure the type matches 
        BOOST_CHECK_EQUAL( 
                dp.getFunction(),
                "test_metric"
                );

        // ensure the id matches 
        BOOST_CHECK_EQUAL( 
                dp.getId(),
                "27"
                );

        // ensure the other data matches 
        BOOST_CHECK_EQUAL( 
                dp.getOtherData(),
                "123"
                );

        // ensure the timer recorded at least three seconds
        BOOST_CHECK( 
                dp.getDuration().total_seconds() >= 3.0
                );

        // ensure the timer recorded was less than 3.5 seconds
        BOOST_CHECK( 
                dp.getDuration().total_seconds() < 3.5
                );
    }
}

BOOST_AUTO_TEST_CASE( list_count )
{
    // create persistent count set
    std::string container( "test_list" );
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
}


BOOST_AUTO_TEST_CASE( list_values )
{
    // create persistent count set
    std::string name( "test_list_values" );
    PERFORMANCE_LIST_INIT( name );
    typedef PersistentStatisticList::Timer Timer;

    // time something
    {
        Timer timer( name, "test_metric", "456", Mode::Value::Basic, "99" );
        sleep(2); // sleep for 2 seconds
    }

    // get statistics
    PersistentStatisticList::Ptr stats = GlobalStatistics::instance().get<PersistentStatisticList::Ptr>( name );

    // verify the DataPoint that was recorded
    List<DataPoint>::Container container;
    stats->get(container);
    BOOST_FOREACH( const DataPoint& dp, container ) {
        // ensure the type matches 
        BOOST_CHECK_EQUAL( 
                dp.getFunction(),
                "test_metric"
                );

        // ensure the id matches 
        BOOST_CHECK_EQUAL( 
                dp.getId(),
                "456"
                );

        // ensure the other data matches 
        BOOST_CHECK_EQUAL( 
                dp.getOtherData(),
                "99"
                );

        // ensure the timer recorded at least two seconds
        BOOST_CHECK( 
                dp.getDuration().total_seconds() >= 2.0
                );

        // ensure the timer recorded was less than 2.5 seconds
        BOOST_CHECK( 
                dp.getDuration().total_seconds() < 2.5
                );
    }
}

BOOST_AUTO_TEST_SUITE_END()
