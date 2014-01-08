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

#ifndef BGQDB_FILTERING_TIMEINTERVAL_H_
#define BGQDB_FILTERING_TIMEINTERVAL_H_

#include "Duration.h"

#include <boost/date_time.hpp>

#include <iosfwd>

namespace BGQDB {
namespace filtering {

class TimeInterval
{
public:

    typedef boost::posix_time::ptime Timestamp;


    static const TimeInterval ALL;


    static void readTimestamp( std::istream& is, Timestamp& ts_out );


    TimeInterval()
    { /* Nothing to do */ }

    TimeInterval( const Timestamp& start, const Timestamp& end );

    TimeInterval( const Duration& duration );

    TimeInterval( const Timestamp& start, const Duration& duration );

    TimeInterval( const Duration& duration, const Timestamp& end );


    bool operator==( const TimeInterval& other ) const  { return (_start == other._start && _end == other._end && _duration == other._duration); }
    bool operator!=( const TimeInterval& other ) const  { return (! (*this == other)); }

    const Timestamp& getStart() const  { return _start; }
    const Timestamp& getEnd() const  { return _end; }
    const Duration& getDuration() const  { return _duration; }


private:

    Timestamp _start;
    Timestamp _end;
    Duration _duration;
};


std::istream& operator>>( std::istream& is, TimeInterval& interval );

std::ostream& operator<<( std::ostream& os, const TimeInterval& interval );


} } // namespace BGQDB::filtering

#endif
