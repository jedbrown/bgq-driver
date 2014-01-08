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
 * \file bgsched/utility.h
 * \brief utility functions.
 */

#ifndef BGSCHED_UTILITY_H_
#define BGSCHED_UTILITY_H_

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <sstream>

#define THROW_EXCEPTION( exception_class, error_type, msg_expr ) \
{ \
    std::ostringstream te_oss_; \
    te_oss_ << msg_expr; \
    std::string te_msg_(te_oss_.str()); \
    LOG_ERROR_MSG( te_msg_ ); \
    throw exception_class( error_type, te_msg_ ); \
}

#define THROW_EXCEPTION_NO_LOGGER( exception_class, error_type, msg_expr ) \
{ \
    std::ostringstream te_oss_; \
    te_oss_ << msg_expr; \
    std::string te_msg_(te_oss_.str()); \
    throw exception_class( error_type, te_msg_ ); \
}

namespace bgsched {

/*!
 * \brief Set the Properties object.
 */
void setProperties(
        const bgq::utility::Properties::Ptr prop  //!< Pointer to Properties object
        );

/*!
 * \brief Get the Properties object.
 *
 * \return Properties object.
 */
bgq::utility::Properties::Ptr getProperties();

/*!
 * \brief Log XML stream.
 */
void logXML(
        const std::stringstream& os,  //!< [in/out] Stream to write to
        const bool force              //!< [in] true if forcing writing to stream
        );


} // namespace bgsched

#endif
