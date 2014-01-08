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
#ifndef BGQ_UTILITY_PERFORMANCE_MODE_H
#define BGQ_UTILITY_PERFORMANCE_MODE_H
/*!
 * \file utility/include/performance/Mode.h
 * \brief \link bgq::utility::performance::Mode Mode\endlink definition.
 */

#include <utility/include/Properties.h>
#include <utility/include/Singleton.h>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Global configuration mode for collecting performance monitoring.
 *
 * Set the mode using setMode, get it using getMode.  The default mode is obtained
 * from the Blue Gene properties file.  If no value is found, or if the value
 * is not one of None, Basic, or Extended, the static membe MMode::DefaultMode
 * is used.
 */
class Mode : public bgq::utility::Singleton<Mode>
{
public:
    /*!
     * \brief Wrapper for mode values.
     */
    struct Value
    {
        /*!
         * \brief Possible performance monitor modes.
         */
        enum Type {
            None,
            Basic,
            Extended,
            NumModes
        };
    };

    /*!
     * \brief Convert a Value::Type into a string.
     *
     * \returns "Invalid" if the mode is out of range.
     */
    static const char* toString(
            Value::Type mode    //!< [in]
            )
    {
        static const char* strings[] = {
            "None",
            "Basic",
            "Extended"
        };

        if ( mode >= Value::None && mode < Value::NumModes ) {
            return strings[ mode ];
        } else {
            return "Invalid";
        }
    }

    /*!
     * \brief Default mode if nothing is set.
     */
    static const Value::Type DefaultMode;

public:
    /*!
     * \brief Get the mode.
     *
     * The mode is read from the properties file in the [performance] section using the mode key.
     *
     * \pre library has been initialized with a call to \link bgq::utility::performance::init init\endlink.
     */
    Value::Type getMode() const;
};

} // performance
} // utility
} // bgq

#endif
