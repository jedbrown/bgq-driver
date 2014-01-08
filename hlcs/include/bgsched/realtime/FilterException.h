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
 * \file bgsched/realtime/FilterException.h
 * \brief FilterStateErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_REALTIME_FILTER_EXCEPTION_H_
#define BGSCHED_REALTIME_FILTER_EXCEPTION_H_

#include <bgsched/Exception.h>

namespace bgsched {
namespace realtime {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*! \brief %Filter errors
 */
struct FilterErrors
{
    enum Value
    {
        PatternNotValid //!< The pattern supplied to the filter option is not valid.
    };

    /*!
     * \copydoc InputErrors::toString
     */
    static std::string toString( Value v, const std::string& what );
};


/*!
 *  \brief Exception class for filter errors.
 *
 *  %Filter errors are when the real-time API determines that the application has set a filter
 *  option incorrectly, such as using an regular expression pattern that is not valid. If this
 *  occurs, the filter has not been changed and the application can continue.
 */
typedef InvalidArgument<FilterErrors> FilterException;

//!< @}

} // namespace bgsched::realtime
} // namespace bgsched


#endif
