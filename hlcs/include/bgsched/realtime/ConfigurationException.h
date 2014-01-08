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
 * \file bgsched/realtime/ConfigurationException.h
 * \brief ConfigurationErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_REALTIME_CONFIGURATION_EXCEPTION_H_
#define BGSCHED_REALTIME_CONFIGURATION_EXCEPTION_H_

#include <bgsched/Exception.h>

namespace bgsched {
namespace realtime {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*! \brief Error values for ConfigurationException.
 *
 */
struct ConfigurationErrors
{
    enum Value
    {
        InvalidHost, //!< The host value is not in the correct format.
        MissingSecurityProperty //!< A required security configuration property is missing from the Blue Gene configuration file.
    };

    //! \brief Format the exception value to the string.
    static std::string toString( Value v, const std::string& what );
};


/*! \brief Exception class for configuration errors.
 *
 * The real-time client could not be configured.
 * The configuration error must be corrected before the real-time client can be used.
 *
 */
typedef InvalidArgument<ConfigurationErrors> ConfigurationException;

//!< @}

} // namespace bgsched::realtime
} // namespace bgsched

#endif
