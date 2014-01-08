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
 * \file bgsched/RuntimeException.h
 * \brief RuntimeErrors class definition.
 * \ingroup Exceptions
 */

#ifndef BGSCHED_RUNTIME_EXCEPTION_H_
#define BGSCHED_RUNTIME_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Enumerated error codes for runtime exceptions.
 */
struct RuntimeErrors
{
    /*!
     * \brief Error codes.
     */
    enum Value
    {
        BlockBootError,          //!< Error trying to boot a block
        BlockFreeError,          //!< Error trying to free a block
        BlockCreateError,        //!< Error trying to create a block
        BlockAddError,           //!< Error trying to add a block
        InvalidBlockState,       //!< Block state is not correct for the operation
        DimensionOutOfRange,     //!< Dimension out of range error
        AuthorityError,          //!< Error granting or revoking user authority
        HardwareInUseError,      //!< Hardware "in use" status prevents hardware state operation
        MmcsConnectionError,     //!< Connection to MMCS server failed
        BlockRetrievalError      //!< Error trying to retrieve block from database
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
 * \brief Exception thrown when runtime error occurs.
 */
typedef RuntimeError<RuntimeErrors> RuntimeException;

//!< @}

} // namespace bgsched

#endif

