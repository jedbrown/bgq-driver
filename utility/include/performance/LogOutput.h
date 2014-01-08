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
#ifndef BGQ_UTILITY_PERFORMANCE_LOG_OUTPUT_H
#define BGQ_UTILITY_PERFORMANCE_LOG_OUTPUT_H
/*!
 * \file utility/include/performance/LogOutput.h
 * \brief \link bgq::utility::performance::LogOutput LogOutput\endlink definition and implementation.
 */

#include <utility/include/Log.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <algorithm>
#include <string>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Implements the \link bgq::utility::performance::StatisticSet StatisticSet\endlink log output policy.
 *
 * Sends all output to the log. Time durations are configurable, the default value is microseconds.
 */
class LogOutput
{
public:
    /*!
     * \brief Output units.
     */
    struct Units
    {
        /*!
         * \brief All possible unit values.
         */
        enum Value
        {
            Hours,
            MicroSeconds,
            MilliSeconds,
            Minutes,
            Seconds,

            NumValues
        };

        /*!
         * \brief Convert a Value into a string.
         */
        static const char* toString(
                Value v //!< [in]
                )
        {
            static const char* strings[] = {
                "h",
                "us",
                "ms",
                "m",
                "s"
            };

            if ( v >= 0 && v < NumValues ) {
                return strings[v];
            } else {
                return "INVALID";
            }
        }
    };

protected:
    /*!
     * \brief
     */
    template <typename StoragePolicy>
    void output(
            const std::string& type,    //!< [in]
            StoragePolicy& storage      //!< [in]
            )
    {
        // can't use LOG_DECLARE_FILE here since we're in a header
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.performance" );

        // output statistic boiler plate stuff
        LOG_DEBUG_MSG( "statistics for " << type );

        // get storage container
        typename StoragePolicy::Container container;
        storage.get( container );
        LOG_DEBUG_MSG( container.size() << " statistics" );
        container.sort(
                boost::bind(
                    std::less<boost::posix_time::time_duration>(),
                    boost::bind( &StoragePolicy::Container::value_type::getDuration, _1 ),
                    boost::bind( &StoragePolicy::Container::value_type::getDuration, _2 )
                    )
                );

        // calculate maximum size of each value
        size_t function = strlen("Function");
        size_t id = strlen("Id");
        size_t subFunction = strlen("Sub Function");
        size_t data = strlen("Other Data");
        BOOST_FOREACH( const typename StoragePolicy::Container::value_type& i, container ) {
            function = i.getFunction().size() > function ? i.getFunction().size() : function;
            id = i.getId().size() > id ? i.getId().size() : id;
            subFunction = i.getSubFunction().size() > subFunction ? i.getSubFunction().size() : subFunction;
            data = i.getOtherData().size() > data ? i.getOtherData().size() : data;
        }

        // create format string
        std::ostringstream formatting;
        formatting << "%-" << function << "s ";
        formatting << "%-" << id << "s ";
        formatting << "%-" << subFunction << "s ";
        formatting << "%-" << data << "s";
        LOG_DEBUG_MSG( boost::format( formatting.str() ) % "Function" % "Id" % "Sub Function" % "Other Data" );

        // iterate through results
        formatting << " %4.2e " << Units::toString(_units);
        const std::string format = formatting.str();
        BOOST_FOREACH( const typename StoragePolicy::Container::value_type& i, container ) {
            // convert duration into requested units
            double duration(0);
            static const unsigned SecondsPerMinute = 60;
            static const unsigned MinutesPerHour = 60;
            switch ( _units ) {
                case Units::MicroSeconds: duration = i.getDuration().total_microseconds(); break;
                case Units::MilliSeconds: duration = i.getDuration().total_milliseconds(); break;
                case Units::Seconds:      duration = i.getDuration().total_seconds(); break;
                case Units::Minutes:      duration = i.getDuration().total_seconds() / SecondsPerMinute; break;
                case Units::Hours:        duration = i.getDuration().total_seconds() / (SecondsPerMinute * MinutesPerHour); break;
                default:                  BOOST_ASSERT( !"unhandled unit" ); break;
            }

            LOG_DEBUG_MSG(
                    boost::format( format )
                    % i.getFunction() % i.getId() % i.getSubFunction() % i.getOtherData() % duration
                    );
        }
    }

    /*!
     * \brief ctor.
     */
    LogOutput() :
        _units( Units::MicroSeconds )
    {

    }

    /*!
     * \brief dtor.
     */
    ~LogOutput() { }

public:
    /*!
     * \brief Set the Units.
     */
    void setUnits(
            Units::Value v  //!< [in]
            )
    {
        _units = v;
    }

    /*!
     * \brief Get the units.
     */
    Units::Value getUnits() const { return _units; }

private:
    Units::Value _units;
};

} // performance
} // utility
} // bgq

#endif
