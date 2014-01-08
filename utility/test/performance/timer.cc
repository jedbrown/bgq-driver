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
#define BOOST_TEST_MODULE timer
#include <boost/test/unit_test.hpp>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/performance/DataPoint.h"
#include "utility/include/performance/List.h"
#include "utility/include/performance/StatisticSet.h"

#include "utility/include/performance.h"

using namespace bgq::utility::performance;

// Container type
typedef StatisticSet< List< DataPoint > > Container;

// storage for container
namespace {
Container::Ptr counters_;
}

// some type that is not ostreamable
struct Foo {
};

std::ostream&
operator<<(
        std::ostream& os,
        const Foo&
        )
{
    os.setstate( std::ios::badbit );
    return os;
}

class Fixture
{
public:
    Fixture()
    {
        using namespace bgq::utility;
        initializeLogging( *Properties::create() );

        // initialize performance API
        performance::init( Properties::create() );

        // create container
        counters_ = Container::Ptr( new Container("timer_test") );
    }
};

BOOST_GLOBAL_FIXTURE( Fixture );

BOOST_AUTO_TEST_CASE( create_from_container )
{
    const char* function = "some_function";
    const unsigned id = 123;
    const std::string other( "other data" );
    const std::string sub_function( "some_sub_function" );
    const std::string qualifier( "my qualifier" );
    {
        // test default mode for Timer
        Container::Timer::Ptr timer = counters_->create()
            ->function( function )
            ->id( id )
            ->otherData( other )
            ->subFunction( sub_function )
            ->qualifier( qualifier )
            ;
    }

    // get statistics
    Container::StoragePolicy::Container data;
    counters_->get( data );

    // ensure we have one data point
    BOOST_CHECK_EQUAL( data.size(), 1u );

    // get data point
    const DataPoint& dp = data.front();

    // validate function
    BOOST_CHECK_EQUAL( dp.getFunction(), function );

    // validate id
    BOOST_CHECK_EQUAL( dp.getId(), boost::lexical_cast<std::string>(123) );
    
    // validate other data
    BOOST_CHECK_EQUAL( dp.getOtherData(), other );

    // validate sub function
    BOOST_CHECK_EQUAL( dp.getSubFunction(), sub_function );
    
    // validate qualifier
    BOOST_CHECK_EQUAL( dp.getQualifier(), qualifier );
}

BOOST_AUTO_TEST_CASE( bad_setter_types ) 
{
    // bad ID
    BOOST_CHECK_THROW(
        counters_->create()->id( Foo() ),
        boost::bad_lexical_cast
        );

    // bad function
    BOOST_CHECK_THROW(
        counters_->create()->function( Foo() ),
        boost::bad_lexical_cast
        );
    
    // bad other data
    BOOST_CHECK_THROW(
        counters_->create()->otherData( Foo() ),
        boost::bad_lexical_cast
        );
   
    // bad sub function
    BOOST_CHECK_THROW(
        counters_->create()->subFunction( Foo() ),
        boost::bad_lexical_cast
        );
}

BOOST_AUTO_TEST_CASE( container_not_found )
{
    // creating a Timer on the stack with an invalid container
    // name should throw
    const std::string container( "does_not_exist" );
    const std::string function( "function" );
    const std::string id( "123" );
    {
        BOOST_CHECK_THROW(
                Container::Timer( container, function, id ),
                std::invalid_argument
                );
    }
}
