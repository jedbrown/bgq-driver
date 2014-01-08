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

#include <iostream>


namespace BGQDB {
namespace filtering {


Duration::Duration(
        const Date& date,
        const Time& time
    ) :
        _date(date),
        _time(time)
{
    // Nothing to do.
}


Duration::Duration(
        const Time& time
    ) :
        _time(time)
{
    /// Nothing to do.
}


std::istream& operator>>( std::istream& is, Duration& duration )
{
    Duration::Date date_part;
    Duration::Time time_part;

    if ( is.peek() != 'P' ) {
        is.setstate( std::ios::failbit );
        return is;
    }

    is.ignore(); // ignore the 'P'.

    if ( is.peek() != 'T' ) {
        // should be a date interval
        is >> date_part;
    }

    if ( is.eof() ) {
        duration = Duration( date_part );
        return is;
    }

    if ( is.peek() == 'T' ) {
        is.ignore(); // skip the T.

        unsigned hours(0), minutes(0), seconds(0);

        unsigned num;
        char part_ind;
        char peek_char;

        is >> num;
        part_ind = is.peek();

        if ( part_ind != 'H' && part_ind != 'M' && part_ind != 'S' ) {
            return is;
        }

        is.ignore();

        if ( part_ind == 'H' ) {
            hours = num;

            peek_char = is.peek();
            if ( is.eof() || (! std::isdigit( peek_char )) ) {
                part_ind = '\0';
            } else {
                is >> num;
                part_ind = is.get();
            }
        }
        if ( part_ind == 'M' ) {
            minutes = num;

            peek_char = is.peek();
            if ( is.eof() || (! std::isdigit( peek_char )) ) {
                part_ind = '\0';
            } else {
                is >> num;
                part_ind = is.get();
            }
        }
        if ( part_ind == 'S' ) {
            seconds = num;
        }

        time_part = boost::posix_time::time_duration( hours, minutes, seconds );
    }

    duration = Duration( date_part, time_part );

    return is;
}


std::ostream& operator<<( std::ostream& os, const Duration& duration )
{
    if ( duration == Duration() ) {
        os << "not-a-duration";
        return os;
    }

    // Duration is a valid duration.

    os << "P";
    if ( duration.getDate() != Duration::Date() ) {
        os << duration.getDate();
    }

    const Duration::Time &time(duration.getTime());

    if ( time != Duration::Time() ) {
        os << "T";

        if ( time.hours() != 0 ) os << time.hours() << "H";
        if ( time.minutes() != 0 ) os << time.minutes() << "M";
        if ( time.seconds() != 0 ) os << time.seconds() << "S";
    }

    return os;
}


std::ostream& operator<<( std::ostream& os, const Duration::Date& date )
{
    if ( date == Duration::Date() ) {
        os << "not-a-duration-date";
        return os;
    }

    if ( date.years() != 0 )  os << date.years() << "Y";
    if ( date.months() != 0 )  os << date.months() << "M";
    if ( date.days() != 0 )  os << date.days() << "D";
    return os;
}


std::istream& operator>>( std::istream& is, Duration::Date& date )
{
    unsigned years(0), months(0), days(0);

    unsigned num;
    char part_ind;
    char peek_char;

    is >> num;

    if ( ! is )  return is;

    part_ind = is.peek();

    if ( part_ind != 'Y' && part_ind != 'M' && part_ind != 'D' ) {
        is.setstate( std::ios::failbit );
        return is;
    }

    is.ignore();

    if ( part_ind == 'Y' ) {
        years = num;

        peek_char = is.peek();
        if ( is.eof() || (! std::isdigit( peek_char )) ) {
            part_ind = '\0';
        } else {
            is >> num;

            if ( ! is )  return is;

            part_ind = is.get();
        }
    }

    if ( part_ind == 'M' ) {
        months = num;

        peek_char = is.peek();
        if ( is.eof() || (! std::isdigit( peek_char )) ) {
            part_ind = '\0';
        } else {
            is >> num;

            if ( ! is )  return is;

            part_ind = is.get();
        }
    }

    if ( part_ind == 'D' ) {
        days = num;
        part_ind = '\0';
    }

    if ( part_ind != '\0' ) {
        is.setstate( std::ios::failbit );
        return is;
    }

    date = Duration::Date( years, months, days );
    return is;
}


boost::posix_time::ptime operator-( const boost::posix_time::ptime& pt, const Duration& duration )
{
    boost::posix_time::ptime ret(pt);
    if ( duration.getDate() != Duration::Date() ) {
        ret -= boost::gregorian::years( duration.getDate().years() );
        ret -= boost::gregorian::months( duration.getDate().months() );
        ret -= boost::gregorian::days( duration.getDate().days() );
    }
    if ( duration.getTime() != Duration::Time() ) {
        ret -= duration.getTime();
    }

    return ret;
}


boost::posix_time::ptime operator+( const boost::posix_time::ptime& pt, const Duration& duration )
{
    boost::posix_time::ptime ret(pt);
    if ( duration.getDate() != Duration::Date() ) {
        ret += boost::gregorian::years( duration.getDate().years() );
        ret += boost::gregorian::months( duration.getDate().months() );
        ret += boost::gregorian::days( duration.getDate().days() );
    }
    if ( duration.getTime() != Duration::Time() ) {
        ret += duration.getTime();
    }

    return ret;
}


} } // namespace BGQDB::filtering
