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
 * \file bgsched/allocator/ResourceAllocationException.h
 * \brief ResourceAllocationErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_ALLOCATOR_RESOURCE_ALLOCATION_EXCEPTION_H_
#define BGSCHED_ALLOCATOR_RESOURCE_ALLOCATION_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for resource allocation errors.
 */
struct ResourceAllocationErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value
    {
        HardwareUnavailableError,   //!< Hardware is marked as unavailable
        ResourcesInUseError,        //!< Hardware resources are already "in use"
        BlockDeallocateFailed,      //!< Block could not be deallocated (e.g. the block was not found)
        BlockResourcesUnavailable,  //!< Block resources are not available for allocation
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
 * \brief Exception thrown when resources are unable to be allocated or deallocated.
 */
typedef RuntimeError<ResourceAllocationErrors> ResourceAllocationException;

//!< @}

} // namespace bgsched::allocator
} // namespace bgsched

#endif
