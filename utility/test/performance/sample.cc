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
#include <utility/include/performance/PersistentStatistics.h>

#include <utility/include/Log.h>
#include <utility/include/performance.h>

namespace perf = bgq::utility::performance;

int
main()
{
    // initialize API
    bgq::utility::Properties::ConstPtr properties(
            bgq::utility::Properties::create()
            );
    bgq::utility::initializeLogging( *properties );
    perf::init( properties );

    // create statistics set
    std::string name( "my_name" );
    PERFORMANCE_LIST_INIT( name );
    typedef perf::PersistentStatisticList Container;
    typedef Container::Timer Timer;

    // time some long running operation
    {
        Timer timer( name, "some_metric", "123", perf::Mode::Value::Basic );

        // perform some long running operation
        sleep(1);
        
        // timer goes out of scope
    }

    // get statistics
    Container::Ptr stats;
    perf::GlobalStatistics::instance().get( name, stats );
    stats->output();

    return 0;
}

