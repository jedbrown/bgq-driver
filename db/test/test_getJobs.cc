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

#include "api/filtering/getJobs.h"

#include "api/BGQDBlib.h"

#include "api/tableapi/DBConnectionPool.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <iostream>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE getJobs
#include <boost/test/unit_test.hpp>

using namespace BGQDB::filtering;

using std::cout;

class MyFixture
{
public:

    MyFixture() {
        bgq::utility::Properties::Ptr properties_ptr(bgq::utility::Properties::create());

        bgq::utility::initializeLogging( *properties_ptr );

        BGQDB::init( properties_ptr, 1 );
    }

};

BOOST_GLOBAL_FIXTURE( MyFixture );

BOOST_AUTO_TEST_CASE( easy )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    uint64_t row_count;
    cxxdb::ResultSetPtr rs_ptr;

    getJobs(
            JobFilter(),
            JobSort(),
            *conn_ptr,
            "", // user
            "",
            0,
            0,
            &row_count,
            &rs_ptr
        );

    BOOST_CHECK( true ); // if got here then no exception.
}


BOOST_AUTO_TEST_CASE( start_time_interval )
{
    JobFilter job_filter;
    job_filter.setJobType(JobFilter::JobType::All);
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime::date_duration_type days(1);
    boost::posix_time::ptime then = now - days;
    TimeInterval interval(then, now);
    std::cout << "Between " << boost::posix_time::to_simple_string(interval.getStart())  << " and " <<
             boost::posix_time::to_simple_string(interval.getEnd()) << "\n";
    job_filter.setStartTimeInterval(interval);

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    uint64_t row_count;
    cxxdb::ResultSetPtr rs_ptr;

    getJobs(
            job_filter,
            JobSort(),
            *conn_ptr,
            "", // user
            "",
            0,
            0,
            &row_count,
            &rs_ptr
        );

    BOOST_CHECK( true ); // if got here then no exception.
}
