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
 * \file utility/include/cxxsockets/logging.h
 */

#ifndef CXXSOCKET_LOGGING_H
#define CXXSOCKET_LOGGING_H

#include <utility/include/Log.h>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

#define CXXSOCKET_LOGGING_DECLARE_FD_MDC \
    log4cxx::MDC _descriptor_mdc( "FD", std::string("{") + boost::lexical_cast<std::string>(_fileDescriptor) + "} " );

#define CXXSOCKET_LOGGING_DECLARE_FT_MDC \
    log4cxx::MDC _four_tuple_mdc( "FT", std::string("<") + _connection_data + "> " );
#endif
