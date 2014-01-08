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
 * \file bgsched/InputException.h
 * \brief InputErrors class definition.
 */

#ifndef BGSCHED_INPUT_EXCEPTION_H_
#define BGSCHED_INPUT_EXCEPTION_H_

#include <bgsched/Exception.h>

#include <string>

namespace bgsched {

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
        InvalidMidplaneCoordinates,     //!< Invalid coordinates for a midplane given
        InvalidLocationString,          //!< Location string does not specify a valid location
        InvalidBlockSize,               //!< Block size is not valid for the request
        InvalidBlockName,               //!< Block name is not valid
        InvalidBlockDescription,        //!< Block description is not valid
        InvalidBlockOptions,            //!< Block options are not valid
        InvalidBlockBootOptions,        //!< Block boot options are not valid
        InvalidBlockMicroLoaderImage,   //!< Block micro-loader image is not valid
        InvalidBlockNodeConfiguration,  //!< Block Node configuration is not valid
        InvalidBlockInfo,               //!< Block options, boot options, micro-loader image or Node configuration is not valid
        InvalidNodeBoards,              //!< Node boards argument is not valid
        InvalidDimension,               //!< Dimension argument is not valid
        InvalidNodeBoardCount,          //!< Node board count argument is not valid
        InvalidNodeBoardPosition,       //!< Node board position argument is not valid
        InvalidMidplanes,               //!< Midplanes argument is not valid
        InvalidPassthroughMidplanes,    //!< Pass-through midplanes argument is not valid
        InvalidConnectivity,            //!< Connectivity argument is not valid
        BlockNotFound,                  //!< Block name does not match a valid block
        BlockNotAdded,                  //!< Error prevented block from being added
        BlockNotCreated,                //!< Error prevented block from being created
        InvalidUser,                    //!< User name is not valid
        JobNotFound,                    //!< Job ID was not found
        InvalidIONodeCount              //!< I/O node count is not valid
    };

    /*!
     * \brief Error message string.
     *
     * \return Error message string.
     */
    static std::string toString(
            Value v,                    //!< [in] Error value
            const std::string& what     //!< [in] Descriptive string
            );
};

/*!
 * \brief Exception class when arguments are invalid.
 */
typedef InvalidArgument<InputErrors> InputException;

//!< @}

} // namespace bgsched

#endif
