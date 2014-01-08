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

#ifndef BGQDB_FILTERING_DURATION_H_
#define BGQDB_FILTERING_DURATION_H_

#include <boost/date_time.hpp>

#include <iosfwd>

namespace BGQDB {
namespace filtering {

class Duration {
public:

    class Date {
        public:
            explicit Date( unsigned days = 0 ) : _years(0), _months(0), _days(days)  { /* nothing to do. */ }
            Date( unsigned years, unsigned months, unsigned days ) : _years(years), _months(months), _days(days)  { /* nothing to do. */ }

            bool operator==( const Date& other ) const  { return (_days == other._days && _months == other._months && _years == other._years); }
            bool operator!=( const Date& other ) const  { return (! (*this == other)); }

            unsigned years() const  { return _years; }
            unsigned months() const  { return _months; }
            unsigned days() const  { return _days; }

        private:
            unsigned _years, _months, _days;
    };

    typedef boost::posix_time::time_duration Time;


    Duration()  { /* Nothing do to */ }

    explicit Duration(
            const Date& date,
            const Time& time = Time()
        );

    explicit Duration(
            const Time& time
        );


    const Date& getDate() const  { return _date; }
    const Time& getTime() const  { return _time; }

    bool operator==( const Duration& other ) const  { return (_date == other._date && _time == other._time); }
    bool operator!=( const Duration& other ) const  { return (! (*this == other)); }


private:
    Date _date;
    Time _time;
};


std::ostream& operator<<( std::ostream& os, const Duration& duration );
std::istream& operator>>( std::istream& is, Duration& duration );

std::ostream& operator<<( std::ostream& os, const Duration::Date& date );
std::istream& operator>>( std::istream& is, Duration::Date& date );

boost::posix_time::ptime operator-( const boost::posix_time::ptime& pt, const Duration& duration );
boost::posix_time::ptime operator+( const boost::posix_time::ptime& pt, const Duration& duration );

} } // namespace BGQDB::filtering


#endif
