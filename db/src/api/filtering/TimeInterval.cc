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

#include "filtering/TimeInterval.h"

#include <boost/lexical_cast.hpp>

#include <iostream>


using boost::lexical_cast;

using std::string;


namespace BGQDB {
namespace filtering {


const TimeInterval TimeInterval::ALL(
        boost::posix_time::neg_infin,
        boost::posix_time::pos_infin
    );


void TimeInterval::readTimestamp( std::istream& is, Timestamp& ts_out )
{
    // timestamp is like 20100617T010101, YYYYMMDD'T'HHMMSS

    unsigned year, month, day, hour, minute, second;

    // read in 4 digits for the year.

    if ( ! std::isdigit( is.peek() ) ) {
        return;
    }

    string year_str;

    year_str += is.get();
    year_str += is.get();
    year_str += is.get();
    year_str += is.get();

    year = lexical_cast<unsigned>( year_str );

    string month_str;

    month_str += is.get();
    month_str += is.get();

    month = lexical_cast<unsigned>( month_str );

    string day_str;

    day_str += is.get();
    day_str += is.get();

    day = lexical_cast<unsigned>( day_str );

    is.ignore();

    string hour_str;
    hour_str += is.get();
    hour_str += is.get();

    hour = lexical_cast<unsigned>( hour_str );

    string minute_str;
    minute_str += is.get();
    minute_str += is.get();
    minute = lexical_cast<unsigned>( minute_str );

    string second_str;
    second_str += is.get();
    second_str += is.get();
    second = lexical_cast<unsigned>( second_str );

    ts_out = Timestamp(
            boost::gregorian::date( year, month, day ),
            boost::posix_time::time_duration( hour, minute, second )
        );
}


TimeInterval::TimeInterval(
        const Timestamp& start,
        const Timestamp& end
    ) :
        _start(start), _end(end)
{
    // Nothing to do.
}


TimeInterval::TimeInterval(
        const Duration& duration
    ) :
        _duration(duration)
{
    // Nothing to do.
}


TimeInterval::TimeInterval(
        const Timestamp& start,
        const Duration& duration
    ) :
        _start(start), _duration(duration)
{
    // Nothing to do.
}


TimeInterval::TimeInterval(
        const Duration& duration,
        const Timestamp& end
    ) :
        _end(end), _duration(duration)
{
    // Nothing to do.
}


std::istream& operator>>( std::istream& is, TimeInterval& interval )
{
    if ( is.peek() == 'P' ) {
        // starts with a duration.

        Duration duration;
        is >> duration;

        if ( is.eof() || is.peek() != '/' ) {
            interval = TimeInterval( duration );
            return is;
        }

        is.ignore(); // skip the /

        TimeInterval::Timestamp end_ts;
        TimeInterval::readTimestamp( is, end_ts );

        interval = TimeInterval( duration, end_ts );
        return is;
    }

    TimeInterval::Timestamp start_ts, end_ts;

    TimeInterval::readTimestamp( is, start_ts );
    is.ignore(); // ignore the /

    if ( is.peek() == 'P' ) {
        // it's a duration

        Duration duration;
        is >> duration;

        interval = TimeInterval( start_ts, duration );

    } else {
        // expect a timestamp.

        TimeInterval::readTimestamp( is, end_ts );

        interval = TimeInterval( start_ts, end_ts );
    }

    return is;
}


std::ostream& operator<<( std::ostream& os, const TimeInterval& interval )
{
    if ( interval == TimeInterval() ) {
        os << "not-an-interval";
        return os;
    }

    if ( interval.getStart() != TimeInterval::Timestamp() ) {
        os << boost::posix_time::to_iso_string( interval.getStart() ) << "/";
    }

    if ( interval.getDuration() != Duration() ) {
        os << interval.getDuration();

        if ( interval.getEnd() != TimeInterval::Timestamp() )  os << "/";
    }

    if ( interval.getEnd() != TimeInterval::Timestamp() ) {
        os << boost::posix_time::to_iso_string( interval.getEnd() );
    }

    return os;
}


} } // namespace BGQDB::filtering
