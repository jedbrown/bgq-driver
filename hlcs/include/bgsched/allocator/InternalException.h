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
 * \file bgsched/allocator/InternalException.h
 * \brief InternalErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_ALLOCATOR_INTERNAL_EXCEPTION_H_
#define BGSCHED_ALLOCATOR_INTERNAL_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \brief Enumerated error codes for exceptions internal to the allocator.
 */
struct InternalErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value
    {
        UnexpectedError,                 //!< Unexpected internal error
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
 * \brief Reports when an internal logic assumption has been violated.
 */
typedef LogicError<InternalErrors> InternalException;

} // namespace bgsched::allocator
} // namespace bgsched

//!< @}

#endif
