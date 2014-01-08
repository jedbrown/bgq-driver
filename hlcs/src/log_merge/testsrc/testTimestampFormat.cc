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


#include "TimestampFormat.hpp"

#define BOOST_TEST_MODULE TimestampFormat
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using log_merge::TimestampFormat;
using log_merge::TimestampFormat1;
using log_merge::TimestampFormat3;
using log_merge::TimestampFormatQ;
using log_merge::TimestampFormats;

using std::string;


BOOST_AUTO_TEST_CASE( test_fmt1 )
{
    string line( "04/08/09-01:22:29  /bgsys/local/etc/startmmcs STARTED" );

    string time_str;
    string remaining;

    BOOST_REQUIRE( TimestampFormat1::INSTANCE.split( line, &time_str, &remaining ) );
    BOOST_CHECK_EQUAL( time_str, "04/08/09-01:22:29" );
    BOOST_CHECK_EQUAL( remaining, "  /bgsys/local/etc/startmmcs STARTED");
    BOOST_CHECK_EQUAL( TimestampFormat1::INSTANCE.parse( time_str ), boost::posix_time::time_from_string( "2009-04-08 01:22:29" ) );
}


BOOST_AUTO_TEST_CASE( test_fmt3 )
{
    string line( "2009-04-22-23:20:00.025 mc::bringup" );

    string time_str;
    string remaining;

    BOOST_REQUIRE( TimestampFormat3::INSTANCE.split( line, &time_str, &remaining ) );
    BOOST_CHECK_EQUAL( time_str, "2009-04-22-23:20:00.025" );
    BOOST_CHECK_EQUAL( remaining, " mc::bringup");
    BOOST_CHECK_EQUAL( TimestampFormat3::INSTANCE.parse( time_str ), boost::posix_time::time_from_string( "2009-04-22 23:20:00.025" ) );
}


BOOST_AUTO_TEST_CASE( test_fmtq )
{
    string line( "2009-08-25 10:31:54.660 (INFO ) [queenBee] mcservermain - File limits: 1024" );

    string time_str;
    string remaining;

    BOOST_REQUIRE( TimestampFormatQ::INSTANCE.split( line, &time_str, &remaining ) );
    BOOST_CHECK_EQUAL( time_str, "2009-08-25 10:31:54.660" );
    BOOST_CHECK_EQUAL( remaining, " (INFO ) [queenBee] mcservermain - File limits: 1024");
    BOOST_CHECK_EQUAL( TimestampFormatQ::INSTANCE.parse( time_str ), boost::posix_time::time_from_string( "2009-08-25 10:31:54.660" ) );
}


BOOST_AUTO_TEST_CASE( test_formats_split_fmt1 )
{
    string line( "04/08/09-01:22:29  /bgsys/local/etc/startmmcs STARTED" );

    const TimestampFormat *timestamp_format_p;
    string time_str, remaining;

    TimestampFormats::getInstance().split( line, &timestamp_format_p, &time_str, &remaining );

    BOOST_CHECK_EQUAL( timestamp_format_p, &TimestampFormat1::INSTANCE );
}


BOOST_AUTO_TEST_CASE( test_formats_split_fmt3 )
{
    string line( "2009-04-22-23:20:02.845 R00-M0-S: palomino is initialized (FF:EB:A1:83:09:3B:00:14:5E:7C:F6:C4)" );

    const TimestampFormat *timestamp_format_p;
    string time_str, remaining;

    TimestampFormats::getInstance().split( line, &timestamp_format_p, &time_str, &remaining );

    BOOST_CHECK_EQUAL( timestamp_format_p, &TimestampFormat3::INSTANCE );
}


BOOST_AUTO_TEST_CASE( test_formats_split_fmtQ )
{
    string line( "2009-08-25 10:31:54.660 (INFO ) [queenBee] mcservermain - File limits: 1024" );

    const TimestampFormat *timestamp_format_p;
    string time_str, remaining;

    TimestampFormats::getInstance().split( line, &timestamp_format_p, &time_str, &remaining );

    BOOST_CHECK_EQUAL( timestamp_format_p, &TimestampFormatQ::INSTANCE );
}


BOOST_AUTO_TEST_CASE( test_formats_split_not_format )
{
    string line( "This line won't match any format." );

    TimestampFormats timestamp_formats;

    const TimestampFormat *timestamp_format_p;
    string time_str, remaining;

    timestamp_formats.split( line, &timestamp_format_p, &time_str, &remaining );

    BOOST_CHECK_EQUAL( timestamp_format_p, (TimestampFormat*)(0) );
}
