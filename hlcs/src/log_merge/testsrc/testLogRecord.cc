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


#include "LogRecord.hpp"
#include "Interval.hpp"


#define BOOST_TEST_MODULE Interval
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using log_merge::Interval;
using log_merge::LogRecord;


BOOST_AUTO_TEST_CASE( testInterval )
{
    Interval interval( boost::posix_time::time_from_string( "2010-05-04 16:00:10.757" ), boost::posix_time::time_from_string( "2010-05-04 17:00:10.757" ) );

    LogRecord log_record1(
            boost::posix_time::time_from_string( "2010-05-03 16:30:10.757" ),
            "2010-05-03 16:30:10.757 test"
        );

    BOOST_CHECK( log_record1.before( interval ) );
    BOOST_CHECK( ! log_record1.in( interval ) );
    BOOST_CHECK( ! log_record1.after( interval ) );


    LogRecord log_record2(
            boost::posix_time::time_from_string( "2010-05-04 16:30:10.757" ),
            "2010-05-04 16:30:10.757 test"
        );

    BOOST_CHECK( ! log_record2.before( interval ) );
    BOOST_CHECK( log_record2.in( interval ) );
    BOOST_CHECK( ! log_record2.after( interval ) );


    LogRecord log_record3(
            boost::posix_time::time_from_string( "2010-05-05 16:30:10.757" ),
            "2010-05-05 16:30:10.757 test"
        );

    BOOST_CHECK( ! log_record3.before( interval ) );
    BOOST_CHECK( ! log_record3.in( interval ) );
    BOOST_CHECK( log_record3.after( interval ) );

}
