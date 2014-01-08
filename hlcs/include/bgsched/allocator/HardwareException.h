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
 * \file bgsched/allocator/HardwareException.h
 * \brief HardwareErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_ALLOCATOR_HARDWARE_EXCEPTION_H_
#define BGSCHED_ALLOCATOR_HARDWARE_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for hardware errors.
 */
struct HardwareErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value
    {
        InError,    //!< Hardware has been marked in error due to detected error
        InService,  //!< Hardware marked for service action
        Missing     //!< Hardware is missing
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
 * \brief Exception thrown when allocation fails due to hardware in error state.
 */
typedef RuntimeError<HardwareErrors> HardwareException;

//!< @}

} // namespace bgsched::allocator
} // namespace bgsched

#endif
