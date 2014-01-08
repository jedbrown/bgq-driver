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
 * \file bgsched/InitializationException.h
 * \brief InitializationErrors class definition.
 */

#ifndef BGSCHED_INITIALIZATION_EXCEPTION_H_
#define BGSCHED_INITIALIZATION_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for initialization errors.
 */
struct InitializationErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value {
        DatabaseInitializationFailed, //!< Error initializing database
        MalformedPropertiesFile,      //!< Properties file format is not correct
        PropertiesNotFound,           //!< Properties file not found
    };

    /*!
     * \brief Error message string.
     *
     * \return Error message string.
     */
    static std::string toString(
            Value v,                   //!< [in] Error value
            const std::string& what    //!< [in] Descriptive string
            );
};

/*!
 *  \brief Exception class for initialization failures.
 *
 *  This exception is thrown if the bgsched library fails to initialize. An
 *  example of this is if the configuration file cannot be found or is not
 *  valid. The application should not use the bgsched library if it fails to
 *  initialize.
 */
typedef RuntimeError<InitializationErrors> InitializationException;

//!< @}

} // namespace bgsched

#endif
