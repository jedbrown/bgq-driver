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

/*!
 * \file bgsched/TimeInterval.h
 * \brief TimeInterval class definition.
 */

#ifndef BGSCHED_TIMEINTERVAL_H_
#define BGSCHED_TIMEINTERVAL_H_

#include <boost/date_time.hpp>

#include <iosfwd>

namespace bgsched {

/*!
 * \brief Represents a start and end time interval.
 */
class TimeInterval
{
public:

    static const TimeInterval ALL;

    /*!
     * \brief
     */
    TimeInterval();

    /*!
     * \brief
     */
    explicit TimeInterval(
            const boost::posix_time::ptime start,  //!< [in] Start time value
            const boost::posix_time::ptime end     //!< [in] End time value
        );

    /*!
     * \brief Construct time interval from ISO8601 timestamps (http://en.wikipedia.org/wiki/ISO_8601#Time_intervals).
     * Strings are ISO8601 timestamps like YYYYMMDDThhmmss
     *
     * \throws std::bad_cast exception if unable to convert string to boost::posix_time format
     */
    explicit TimeInterval(
            const std::string& startTime_ISO_Format, //!< [in] Start time as ISO8601 timestamp like YYYYMMDDThhmmss
            const std::string& endTime_ISO_Format    //!< [in] End time as ISO8601 timestamp like YYYYMMDDThhmmss
        );

    /*!
     * \brief Equality operator.
     */
    bool operator==(
            const TimeInterval& other        //!< [in] Time interval to compare with
            ) const;

    /*!
     * \brief  Non-equality operator.
     */
    bool operator!=(
            const TimeInterval& other        //!< [in] Time interval to compare with
            ) const;

    /*!
     * \brief Get the start time value.
     *
     * \return Start time value.
     */
    const boost::posix_time::ptime& getStart() const;

    /*!
     * \brief Get the end time value.
     *
     * \return End time value.
     */
    const boost::posix_time::ptime& getEnd() const;

protected:
    boost::posix_time::ptime _start;  //!< Start time
    boost::posix_time::ptime _end;    //!< End time
};

} // namespace bgsched

#endif
