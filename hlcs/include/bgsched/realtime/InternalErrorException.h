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
 * \file bgsched/realtime/InternalErrorException.h
 * \brief InternalErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_REALTIME_INTERNAL_ERROR_EXCEPTION_H_
#define BGSCHED_REALTIME_INTERNAL_ERROR_EXCEPTION_H_

#include <bgsched/Exception.h>

namespace bgsched {
namespace realtime {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*! \brief Internal errors */
struct InternalErrors
{
    enum Value
    {
        ApiUnexpectedFailure //!< An API called by the real-time client failed in an unexpected way.
    };

    /*!
     * \copydoc InputErrors::toString
     */
    static std::string toString( Value v, const std::string& what );
};


/*!
 *  \brief Exception class for internal errors.
 *
 *  Internal errors are when the real-time API encounters an error calling an API that fails in
 *  some unexpected way. One example is if the API called close() and that call failed. This
 *  might be an indication of memory corruption or a programming error in the API and the calling
 *  application probably cannot recover.
 */
typedef RuntimeError<InternalErrors> InternalErrorException;

//!< @}

} // namespace bgsched::realtime
} // namespace bgsched

#endif
