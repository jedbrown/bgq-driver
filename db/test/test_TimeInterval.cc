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

#include "filtering/Duration.h"
#include "filtering/TimeInterval.h"

#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <string>

using BGQDB::filtering::Duration;
using BGQDB::filtering::TimeInterval;

using boost::lexical_cast;

using std::string;

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TimeInterval
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( default_date )
{
    // Can create a default date, compare default date to itself

    Duration::Date date;

    BOOST_CHECK_EQUAL( date, Duration::Date() );
}


BOOST_AUTO_TEST_CASE( duration_date_days )
{
    // Can construct date with # days, get days.

    Duration::Date date( 1 );

    BOOST_CHECK_EQUAL( date.days(), 1 );

    BOOST_CHECK_EQUAL( date, Duration::Date( 1 ) );

    BOOST_CHECK( date != Duration::Date() );
}


BOOST_AUTO_TEST_CASE( duration_date_ymd )
{
    // Can construct date with years, months, days.

    Duration::Date date( 1, 2, 3 );

    BOOST_CHECK_EQUAL( date.years(), 1 );
    BOOST_CHECK_EQUAL( date.months(), 2 );
    BOOST_CHECK_EQUAL( date.days(), 3 );

    BOOST_CHECK_EQUAL( date, Duration::Date( 1, 2, 3 ) );
}


BOOST_AUTO_TEST_CASE( duration_date_out )
{
    // Output duration date, formatted in the date part of the ISO8601 format
    // default is output as not-a-duration-date

    BOOST_CHECK_EQUAL( lexical_cast<string>( Duration::Date( 1 ) ), "1D" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( Duration::Date( 1, 1, 1 ) ), "1Y1M1D" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( Duration::Date( 1, 0, 0 ) ), "1Y" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( Duration::Date( 0, 1, 0 ) ), "1M" );
    BOOST_CHECK_EQUAL( lexical_cast<string>( Duration::Date() ), "not-a-duration-date" );
}


BOOST_AUTO_TEST_CASE( duration_date_in )
{
    // input duration date, must be formatted in the date part of the ISO8601 format

    BOOST_CHECK_EQUAL( lexical_cast<Duration::Date>( "1D" ), Duration::Date( 1 ) );
    BOOST_CHECK_EQUAL( lexical_cast<Duration::Date>( "2D" ), Duration::Date( 2 ) );
    BOOST_CHECK_EQUAL( lexical_cast<Duration::Date>( "1Y" ), Duration::Date( 1, 0, 0 ) );
    BOOST_CHECK_EQUAL( lexical_cast<Duration::Date>( "1M" ), Duration::Date( 0, 1, 0 ) );
    BOOST_CHECK_EQUAL( lexical_cast<Duration::Date>( "1Y2M3D" ), Duration::Date( 1, 2, 3 ) );
}


BOOST_AUTO_TEST_CASE( duration_date_in_ex )
{
    // should get error when try to read in a duration date and not a duration date.

    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "" ), boost::bad_lexical_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "1" ), boost::bad_lexical_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "1G" ), boost::bad_lexical_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "T" ), boost::bad_lexical_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "1YT" ), boost::bad_lexical_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "1MT" ), boost::bad_lexical_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration::Date>( "1DT" ), boost::bad_lexical_cast );
}


BOOST_AUTO_TEST_CASE( default_duration )
{
    // Can create a Duration using no args, is then some kind of invalid duration.

    Duration dun;

    BOOST_CHECK_EQUAL( dun.getDate(), Duration::Date() );
    BOOST_CHECK_EQUAL( dun.getTime(), Duration::Time() );
}


BOOST_AUTO_TEST_CASE( default_duration_compare )
{
    // Can create a Duration using no args, is equal to another dun created with no args.

    Duration dun;
    BOOST_CHECK_EQUAL( dun, Duration() );
}


BOOST_AUTO_TEST_CASE( duration_date )
{
    // Can create a TI:Duration using just a date duration.

    Duration dun( Duration::Date( 1 ) );

    BOOST_CHECK_EQUAL( dun.getDate(), Duration::Date( 1 ) );
    BOOST_CHECK_EQUAL( dun.getTime(), Duration::Time() );
}


BOOST_AUTO_TEST_CASE( duration_time )
{
    // Can create a Duration using just a time duration.

    Duration dun( boost::posix_time::seconds( 1 ) );

    BOOST_CHECK_EQUAL( dun.getDate(), Duration::Date() );
    BOOST_CHECK_EQUAL( dun.getTime(), boost::posix_time::seconds( 1 ) );
}


BOOST_AUTO_TEST_CASE( duration_date_time )
{
    // Can create a Duration using both date and time.

    Duration dun( Duration::Date( 1 ), boost::posix_time::seconds( 1 ) );

    BOOST_CHECK_EQUAL( dun.getDate(), Duration::Date( 1 ) );
    BOOST_CHECK_EQUAL( dun.getTime(), boost::posix_time::seconds( 1 ) );
}


BOOST_AUTO_TEST_CASE( duration_compare )
{
    // Can create duration, is same as another duration with the same date duration,
    // is not equal to the default duration.

    {
        Duration dun( Duration::Date( 1 ) );

        BOOST_CHECK_EQUAL( dun, Duration( Duration::Date( 1 ) ) );
        BOOST_CHECK( dun != Duration() );
    }

    {
        Duration dun( boost::posix_time::seconds( 1 ) );

        BOOST_CHECK_EQUAL( dun, Duration( boost::posix_time::seconds( 1 ) ) );
        BOOST_CHECK( dun != Duration() );
    }
}


BOOST_AUTO_TEST_CASE( duration_out )
{
    // Can output a Duration, generates a ISO8601 string.

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration( Duration::Date( 1 ) ) ),
            "P1D"
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration( boost::posix_time::seconds( 1 ) ) ),
            "PT1S"
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration( boost::posix_time::minutes( 1 ) ) ),
            "PT1M"
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration( boost::posix_time::hours( 1 ) ) ),
            "PT1H"
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration( Duration::Date( 1 ), boost::posix_time::seconds( 1 ) ) ),
            "P1DT1S"
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration( Duration::Date( 1, 2, 3 ), boost::posix_time::time_duration( 4, 5, 6 ) ) ),
            "P1Y2M3DT4H5M6S"
        );
}


BOOST_AUTO_TEST_CASE( default_duration_out )
{
    // If output default-constructed duration, prints not-a-duration
    BOOST_CHECK_EQUAL(
            lexical_cast<string>( Duration() ),
            "not-a-duration"
        );
}


BOOST_AUTO_TEST_CASE( duration_in )
{
    // Can input a Duration as a ISO8601 string.

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "P1D" ),
            Duration( Duration::Date( 1 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "P1Y2M3D" ),
            Duration( Duration::Date( 1, 2, 3 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "PT1S" ),
            Duration( boost::posix_time::seconds( 1 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "PT2M" ),
            Duration( boost::posix_time::minutes( 2 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "PT3H" ),
            Duration( boost::posix_time::hours( 3 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "PT1H2M3S" ),
            Duration( boost::posix_time::time_duration( 1, 2, 3 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "P1Y2M3DT4H5M6S" ),
            Duration(
                    Duration::Date( 1, 2, 3 ),
                    boost::posix_time::time_duration( 4, 5, 6 )
                )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "P1M" ),
            Duration( Duration::Date( 0, 1, 0 ) )
        );

    BOOST_CHECK_EQUAL(
            lexical_cast<Duration>( "P1Y" ),
            Duration( Duration::Date( 1, 0, 0 ) )
        );

}


BOOST_AUTO_TEST_CASE( duration_in_invalid )
{
    BOOST_CHECK_THROW( lexical_cast<Duration>( "T" ), std::bad_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration>( "P" ), std::bad_cast );
    BOOST_CHECK_THROW( lexical_cast<Duration>( "PT" ), std::bad_cast );
}


BOOST_AUTO_TEST_CASE( default_interval )
{
    // Can get fields for default time interval, start time and end time are not-a-time-interval, duration is not-a-duration

    TimeInterval ti;

    BOOST_CHECK_EQUAL( ti.getStart(), TimeInterval::Timestamp() );
    BOOST_CHECK_EQUAL( ti.getEnd(), TimeInterval::Timestamp() );
    BOOST_CHECK_EQUAL( ti.getDuration(), Duration() );
}


BOOST_AUTO_TEST_CASE( create_interval_start_end )
{
    // Can create TimeInterval with two ptimes for start and end.

    TimeInterval ti(
            boost::posix_time::time_from_string( "2011-01-28 9:00" ),
            boost::posix_time::time_from_string( "2011-01-28 10:00" )
        );

    BOOST_CHECK_EQUAL( ti.getStart(), boost::posix_time::time_from_string( "2011-01-28 9:00" ) );
    BOOST_CHECK_EQUAL( ti.getEnd(), boost::posix_time::time_from_string( "2011-01-28 10:00" ) );
    BOOST_CHECK_EQUAL( ti.getDuration(), Duration() );
}


BOOST_AUTO_TEST_CASE( create_interval_duration )
{
    // Can create a time interval with just a duration

    TimeInterval ti(
            Duration( Duration::Date( 1 ) )
        );

    BOOST_CHECK_EQUAL( ti.getDuration(), Duration( Duration::Date( 1 ) ) );
    BOOST_CHECK_EQUAL( ti.getStart(), TimeInterval::Timestamp() );
    BOOST_CHECK_EQUAL( ti.getEnd(), TimeInterval::Timestamp() );
}


BOOST_AUTO_TEST_CASE( create_interval_start_time_duration )
{
    // Can create a time interval with start time and duration.

    TimeInterval ti(
            boost::posix_time::time_from_string( "2011-01-28 9:00" ),
            Duration( Duration::Date( 1 ) )
        );

    BOOST_CHECK_EQUAL( ti.getStart(), boost::posix_time::time_from_string( "2011-01-28 9:00" ) );
    BOOST_CHECK_EQUAL( ti.getDuration(), Duration( Duration::Date( 1 ) ) );
    BOOST_CHECK_EQUAL( ti.getEnd(), TimeInterval::Timestamp() );
}


BOOST_AUTO_TEST_CASE( create_interval_duration_end_time )
{
    // Can create a time interval with start time and duration.

    TimeInterval ti(
            Duration( Duration::Date( 1 ) ),
            boost::posix_time::time_from_string( "2011-01-28 9:00" )
        );

    BOOST_CHECK_EQUAL( ti.getStart(), TimeInterval::Timestamp() );
    BOOST_CHECK_EQUAL( ti.getDuration(), Duration( Duration::Date( 1 ) ) );
    BOOST_CHECK_EQUAL( ti.getEnd(), boost::posix_time::time_from_string( "2011-01-28 9:00" ) );
}


BOOST_AUTO_TEST_CASE( compare_intervals )
{
    // Can compare default constructed to default constructed to see if is default constructed.
    // If compare non-default constructed to default constructed is not equal.

    {
        TimeInterval ti;

        BOOST_CHECK_EQUAL( ti, TimeInterval() );
    }

    {
        TimeInterval ti(
                boost::posix_time::time_from_string( "2011-01-28 9:00" ),
                boost::posix_time::time_from_string( "2011-01-28 10:00" )
            );

        BOOST_CHECK_EQUAL( ti,
                TimeInterval(
                        boost::posix_time::time_from_string( "2011-01-28 9:00" ),
                        boost::posix_time::time_from_string( "2011-01-28 10:00" )
            ) );

        BOOST_CHECK( ti !=
                TimeInterval(
                    boost::posix_time::time_from_string( "2011-01-28 9:00" ),
                    boost::posix_time::time_from_string( "2011-01-28 11:00" )
            ) );

        BOOST_CHECK( ti != TimeInterval() );
    }
}


BOOST_AUTO_TEST_CASE( interval_all )
{
    // Got a constant for ALL time, -inf->+inf

    BOOST_CHECK_EQUAL( TimeInterval::ALL.getStart(), boost::posix_time::neg_infin );
    BOOST_CHECK_EQUAL( TimeInterval::ALL.getEnd(), boost::posix_time::pos_infin );
    BOOST_CHECK_EQUAL( TimeInterval::ALL.getDuration(), Duration() );
}


BOOST_AUTO_TEST_CASE( interval_output )
{
    // Can output to a stringstream. Uses ISO8601 duration format.

    BOOST_CHECK_EQUAL(
            lexical_cast<string>(
                    TimeInterval(
                            boost::posix_time::time_from_string( "2011-01-28 9:00" ),
                            boost::posix_time::time_from_string( "2011-01-28 10:00" )
                        )
                ),
            "20110128T090000/20110128T100000"
        );


    // Output with just a duration.

    BOOST_CHECK_EQUAL(
            lexical_cast<string>(
                    TimeInterval(
                            Duration(
                                    Duration::Date( 1 )
                                )
                        )
                ),
            "P1D"
        );

    // Output with start and duration.

    BOOST_CHECK_EQUAL(
            lexical_cast<string>(
                    TimeInterval(
                            boost::posix_time::time_from_string( "2011-01-28 9:00" ),
                            Duration(
                                    Duration::Date( 1 )
                                )
                        )
                ),
            "20110128T090000/P1D"
        );

    // Output with duration and end time.

    BOOST_CHECK_EQUAL( lexical_cast<string>(
            TimeInterval( Duration( Duration::Date( 1 ) ), boost::posix_time::time_from_string( "2011-01-28 10:00" ) ) ),
            "P1D/20110128T100000"
        );
}


BOOST_AUTO_TEST_CASE( output_default )
{
    // Output interval that's default constructed prints "not-an-interval"

    BOOST_CHECK_EQUAL( lexical_cast<string>(TimeInterval()), "not-an-interval" );
}


BOOST_AUTO_TEST_CASE( parse_interval )
{
    // Can parse a ISO8601 string to TimeInterval

    BOOST_CHECK_EQUAL( lexical_cast<TimeInterval>( "20100617T010101/20100617T020202" ), TimeInterval( boost::posix_time::time_from_string( "2010-06-17 1:01:01" ), boost::posix_time::time_from_string( "2010-06-17 2:02:02" ) ) );
    BOOST_CHECK_EQUAL( lexical_cast<TimeInterval>( "P1D" ), TimeInterval( Duration( Duration::Date( 1 ) ) ) );
    BOOST_CHECK_EQUAL( lexical_cast<TimeInterval>( "P1D/20100617T020202" ), TimeInterval( Duration( Duration::Date( 1 ) ), boost::posix_time::time_from_string( "2010-06-17 2:02:02" )  ) );
    BOOST_CHECK_EQUAL( lexical_cast<TimeInterval>( "20100617T010101/P1D" ), TimeInterval( boost::posix_time::time_from_string( "2010-06-17 1:01:01" ), Duration( Duration::Date( 1 ) ) ) );
}


BOOST_AUTO_TEST_CASE( parse_invalid_string_no_sep )
{
    // If pass an invalid interval, fails.

    BOOST_CHECK_THROW( lexical_cast<TimeInterval>( "20100617T010101" ), std::bad_cast );
    BOOST_CHECK_THROW( lexical_cast<TimeInterval>( "20100617T010101/notatimestamp" ), std::bad_cast );
}
