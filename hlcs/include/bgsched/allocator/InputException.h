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
 * \file bgsched/allocator/InputException.h
 * \brief InputErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_ALLOCATOR_INPUT_EXCEPTION_H_
#define BGSCHED_ALLOCATOR_INPUT_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for user generated input.
 */
struct InputErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value
    {
        PluginNotFound,             //!< Plug-in name does not match any loaded plug-in
        PluginLibraryNotFound,      //!< Path given does not specify a valid plug-in library
        InvalidMidplaneCoordinates, //!< Invalid coordinates for a midplane given
        InvalidLocationString,      //!< Invalid location string given
        InvalidDimension,           //!< Invalid dimension given
        InvalidConnectivity,        //!< Invalid connectivity given
        InvalidComputeNodeSize      //!< Invalid compute node size given
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
 * \brief Exception thrown when error occurs due to faulty input such as an
 * invalid hardware location.
 */
typedef InvalidArgument<InputErrors> InputException;

//!< @}

} // namespace bgsched::allocator
} // namespace bgsched

#endif
