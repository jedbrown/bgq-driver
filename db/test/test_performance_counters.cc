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

#define BOOST_TEST_MODULE database_performance_counters_output
#include <boost/test/unit_test.hpp>

#include <db/include/api/BGQDBlib.h>

#include "utility/include/performance/DatabaseOutput.h"
#include <utility/include/performance/List.h>
#include <utility/include/performance/StatisticSet.h>

#include <utility/include/Log.h>
#include "utility/include/performance.h"
#include <utility/include/Properties.h>

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

namespace perf = bgq::utility::performance;

class Fixture
{
public:
        typedef perf::StatisticSet< perf::List< perf::DataPoint >, perf::DatabaseOutput > Container;

public:
    Fixture() :
        _counters()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );

        // initialize performance API
        performance::init( Properties::create() );

        // initialize DB API
        BGQDB::init( Properties::create(), 1 );

        // create container
        Container::Ptr container( new Container("database_test") );

        // add to global container
        perf::GlobalStatistics::instance().add( container );
    }

public:
    Container::Ptr _counters;
};


BOOST_GLOBAL_FIXTURE( Fixture );

BOOST_AUTO_TEST_CASE( output )
{
    // create persistent count set
    const std::string container( "database_test" );
    typedef Fixture::Container::Timer Timer;

    // time something
    {
        Timer timer( container, "some_function", "some_id" );
    }

    // get statistics
    Fixture::Container::Ptr stats = perf::GlobalStatistics::instance().get<Fixture::Container::Ptr>( container );

    // ensure container is non-zero
    BOOST_CHECK( stats->getCount() > 0u );

    // output them
    stats->output();

    // ensure container is empty
    BOOST_CHECK_EQUAL( stats->getCount(), 0u );

    // remove any rows we inserted
    cxxdb::ConnectionPtr connection = BGQDB::DBConnectionPool::instance().getConnection();
    connection->execute( "DELETE FROM TBGQComponentperf where component='" + container + "'" );
}
