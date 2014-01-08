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

#ifndef RT_UTIL_H_
#define RT_UTIL_H_

#include <errno.h>
#include <string.h>

#include <sstream>
#include <stdexcept>

#include "utility/include/Log.h"

#define LOG_API_ERROR( api_name, error_no ) \
    LOG_ERROR_MSG( api_name << " failed with '" << strerror( error_no ) << "'" );

#define THROW_RUNTIME_ERROR_EXCEPTION( what_expr ) \
{ \
    std::ostringstream tae_oss_; \
    tae_oss_ << what_expr; \
    LOG_ERROR_MSG( "THROWING runtime_error '" << tae_oss_.str() << "'" ); \
    throw std::runtime_error( tae_oss_.str() ); \
}

#define DESTRUCTOR_THROW_RUNTIME_ERROR_EXCEPTION( what_expr ) \
{ \
    std::ostringstream tae_oss_; \
    tae_oss_ << what_expr; \
    if ( std::uncaught_exception() ) { \
        LOG_ERROR_MSG( "DESTRUCTOR ignoring runtime_error '" << tae_oss_.str() << "'" ); \
        return; \
    } \
    LOG_ERROR_MSG( "THROWING runtime_error '" << tae_oss_.str() << "'" ); \
    throw std::runtime_error( tae_oss_.str() ); \
}

#define THROW_API_ERROR( doing_what_expr, api_name, error_no ) \
    THROW_RUNTIME_ERROR_EXCEPTION( (api_name) << " failed with '" << strerror( error_no ) << "' while " << doing_what_expr );

#define DESTRUCTOR_THROW_API_ERROR( api_name, error_no ) \
    DESTRUCTOR_THROW_RUNTIME_ERROR_EXCEPTION( (api_name) << " failed with '" << strerror( error_no ) << "'" );

#endif
