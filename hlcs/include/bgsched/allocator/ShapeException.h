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
 * \file bgsched/allocator/ShapeException.h
 * \brief ShapeErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_ALLOCATOR_SHAPE_EXCEPTION_H_
#define BGSCHED_ALLOCATOR_SHAPE_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for Shape errors.
 */
struct ShapeErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value
    {
        InvalidMidplaneDimension,   //!< A midplane dimension was given which exceeds the system dimensions
        InvalidNodeDimension,       //!< A node dimension was given which exceeds the system dimensions
        InvalidSize,                //!< An invalid size was specified (larger than the machine, larger than 256 compute nodes for small block or smaller than a midplane for a large block)
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
 * \brief Exception thrown when creating Shape objects.
 */
typedef InvalidArgument<ShapeErrors> ShapeException;

//!< @}

} // namespace bgsched::allocator
} // namespace bgsched

#endif
