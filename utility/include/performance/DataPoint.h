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
#ifndef BGQ_UTILITY_PERFORMANCE_DATAPOINT_H_
#define BGQ_UTILITY_PERFORMANCE_DATAPOINT_H_
/*!
 * \file utility/include/performance/DataPoint.h
 * \brief \link bgq::utility::performance::DataPoint DataPoint\endlink definition and implementation.
 */

#include <utility/include/performance/Mode.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <string>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Measures a duration of some event.
 */
class DataPoint
{
public:
    /*!
     * \brief Construct a new data point.
     */
    DataPoint( 
            const std::string& function,                            //!< [in] indicates that this data point can be compared to others
                                                                    //!<      of the same type
            const std::string& id,                                  //!< [in] a "unique" identifier.  For duration statistics, this is
                                                                    //!<      used to match two data points to measure the interval between
                                                                    //!<      events (i.e. start and end of boot of a block)
            const boost::posix_time::time_duration& duration        //!< [in] duration of the event
            ) :
        _function( function ),
        _sub_function( ),
        _id( id ),
        _other(),
        _qualifier(),
        _mode( Mode::Value::None ),
        _start( boost::posix_time::not_a_date_time ),
        _duration( duration )
    {

    }

    /*!
     * \brief Get function.
     */
    const std::string& getFunction() const { return _function; }

    /*!
     * \brief get ID.
     */
    const std::string& getId() const { return _id; }

    /*!
     * \brief Get other data.
     */
    const std::string& getOtherData() const { return _other; }

    /*!
     * \brief Set other data.
     */
    void setOtherData(
            const std::string& other    //!< [in]
            )
    {
        _other = other;
    }

    /*!
     * \brief Get qualifier.
     */
    const std::string& getQualifier() const { return _qualifier; }

    /*!
     * \brief Set qualifier.
     */
    void setQualifier(
            const std::string& qualifier    //!< [in]
            )
    {
        _qualifier = qualifier;
    }

    /*!
     * \brief Get mode.
     */
    Mode::Value::Type getMode() const { return _mode; }

    /*!
     * \brief Set mode.
     */
    void setMode(
            Mode::Value::Type mode    //!< [in]
            )
    {
        _mode = mode;
    }

    /*!
     * \brief Get subfunction.
     */
    const std::string& getSubFunction() const { return _sub_function; }

    /*!
     * \brief Set subfunction.
     */
    void setSubFunction(
            const std::string& sub_function    //!< [in]
            )
    {
        _sub_function = sub_function;
    }

    /*!
     * \brief Get start time.
     */
    const boost::posix_time::ptime& getStart() const { return _start; }

    /*!
     * \brief Set start time.
     */
    void setStart(
        const boost::posix_time::ptime& start    //!< [in]
        )
    {
        _start = start;
    }

    /*!
     * \brief Get duration.
     */
    const boost::posix_time::time_duration& getDuration() const { return _duration; }

private:
    std::string _function;                      //!< function generating this event.
    std::string _sub_function;                  //!< sub-function generating this event.
    std::string _id;                            //!< Identification for this event.
    std::string _other;                         //!< other data associated with this event.
    std::string _qualifier;                     //!< additional qualification to distinguish id
    Mode::Value::Type _mode;                    //!< extended or basic mode for this datapoint
    boost::posix_time::ptime _start;            //!< time this event started.
    boost::posix_time::time_duration _duration; //!< duration of the event.
};

} // performance
} // utility
} // bgq

#endif
