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


#include "Interval.hpp"


#define BOOST_TEST_MODULE Interval
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using log_merge::Interval;


BOOST_AUTO_TEST_CASE( interval1 )
{
    // Can create an interval with a start and end time.
    // Can test whether a time is before, included in, or after the interval.

    Interval interval( boost::posix_time::time_from_string( "2010-05-04 16:00:10.757" ), boost::posix_time::time_from_string( "2010-05-04 17:00:10.757" ) );

    BOOST_CHECK( interval.includes( boost::posix_time::time_from_string( "2010-05-04 16:30:10.757" ) ) ); // in
    BOOST_CHECK( ! interval.includes( boost::posix_time::time_from_string( "2010-05-03 16:30:10.757" ) ) ); // before
    BOOST_CHECK( ! interval.includes( boost::posix_time::time_from_string( "2010-05-05 16:30:10.757" ) ) ); // after

    BOOST_CHECK( ! interval.before( boost::posix_time::time_from_string( "2010-05-04 16:30:10.757" ) ) ); // in
    BOOST_CHECK( interval.before( boost::posix_time::time_from_string( "2010-05-03 16:30:10.757" ) ) ); // before
    BOOST_CHECK( ! interval.before( boost::posix_time::time_from_string( "2010-05-05 16:30:10.757" ) ) ); // after

    BOOST_CHECK( ! interval.after( boost::posix_time::time_from_string( "2010-05-04 16:30:10.757" ) ) ); // in
    BOOST_CHECK( ! interval.after( boost::posix_time::time_from_string( "2010-05-03 16:30:10.757" ) ) ); // before
    BOOST_CHECK( interval.after( boost::posix_time::time_from_string( "2010-05-05 16:30:10.757" ) ) ); // after
}
