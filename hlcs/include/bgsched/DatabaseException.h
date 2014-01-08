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
 * \file bgsched/DatabaseException.h
 * \brief DatabaseErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_DATABASE_EXCEPTION_H_
#define BGSCHED_DATABASE_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for database errors.
 */
struct DatabaseErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value {
        DatabaseError,          //!< Error occurred while accessing database
        OperationFailed,        //!< Database operation failed
        InvalidKey,             //!< Invalid key
        DataNotFound,           //!< Expected data not found in database
        DuplicateEntry,         //!< Duplicate data found in database
        XmlError,               //!< Error parsing XML data
        ConnectionError,        //!< Communication error occurred while attempting to connect to database
        UnexpectedError         //!< Unexpected return code from internal database function
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

    /*!
     * \brief Convert database status to error value.
     *
     * \return Error value.
     */
    static Value fromDatabaseStatus (
            int status
            );
};

/*!
 *  \brief Exception class for database failures.
 */
typedef RuntimeError<DatabaseErrors> DatabaseException;

//!< @}

} // namespace bgsched

#endif
