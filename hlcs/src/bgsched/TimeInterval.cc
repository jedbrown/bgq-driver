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

#include <bgsched/TimeInterval.h>

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace bgsched {

const TimeInterval TimeInterval::ALL;

TimeInterval::TimeInterval() :
    _start(boost::posix_time::neg_infin),
    _end(boost::posix_time::pos_infin)
{
    // Nothing to do.
}

TimeInterval::TimeInterval(
        const boost::posix_time::ptime start,
        const boost::posix_time::ptime end
    ) :
    _start(start),
    _end(end)
{
    // Nothing to do.
}

TimeInterval::TimeInterval(
        const string& startTime_ISO_Format,
        const string& endTime_ISO_Format
        ) :
    _start(boost::posix_time::neg_infin),
    _end(boost::posix_time::pos_infin)
{
    try {
        _start = boost::posix_time::from_iso_string(startTime_ISO_Format);
        _end = boost::posix_time::from_iso_string(endTime_ISO_Format);
    } catch (const std::bad_cast& e) {
        throw; // just re-throw it
    }
}

bool
TimeInterval::operator==(
        const TimeInterval& other
        ) const
{
    return ((_start == other._start) && (_end == other._end));
}

bool
TimeInterval::operator!=(
        const TimeInterval& other
        ) const
{
    return (! (*this == other));
}

const boost::posix_time::ptime&
TimeInterval::getStart() const
{
    return _start;
}

const boost::posix_time::ptime&
TimeInterval::getEnd() const
{
    return _end;
}

} // namespace bgsched
