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

#ifndef BGQ_UTILITY_SSL_UTIL_H_
#define BGQ_UTILITY_SSL_UTIL_H_


#include <string>


namespace bgq {
namespace utility {


std::string getSslErrorString();


void throwSslError(
        const std::string& ssl_function_name,
        const std::string& calling_function_name,
        unsigned calling_function_line
    );


#define THROW_SSL_ERROR( ssl_function_name ) \
    bgq::utility::throwSslError( (ssl_function_name), __FILE__, __LINE__ )


} // namespace bgq::utility
} // namespace bgq

#endif
