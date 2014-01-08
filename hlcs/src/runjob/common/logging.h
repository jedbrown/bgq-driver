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
#ifndef RUNJOB_COMMON_LOGGING_H
#define RUNJOB_COMMON_LOGGING_H

#include <utility/include/Log.h>

#include <log4cxx/mdc.h>

#include <boost/lexical_cast.hpp>

#include <string>

#define LOGGING_DECLARE_JOB_MDC(value) \
    log4cxx::MDC::remove( "job" ); log4cxx::MDC _job_mdc( "job", boost::lexical_cast<std::string>(value) + ":" );

#define LOGGING_DECLARE_BLOCK_MDC(value) \
    log4cxx::MDC::remove( "block" ); log4cxx::MDC _block_mdc( "block", boost::lexical_cast<std::string>(value) + ":" );

#define LOGGING_DECLARE_LOCATION_MDC(value) \
    log4cxx::MDC::remove( "location" ); log4cxx::MDC _location_mdc( "location", boost::lexical_cast<std::string>(value) + ":" );

#define LOGGING_DECLARE_USER_MDC(value) \
    log4cxx::MDC::remove( "user" ); log4cxx::MDC _user_mdc( "user", boost::lexical_cast<std::string>(value) + ":" );

namespace runjob {

/*!
 * \brief top level logger.
 */
extern const char* log;

/*!
 * \brief Get the LoggerPtr for this log.
 */
inline log4cxx::LoggerPtr getLogger() { return log4cxx::Logger::getLogger( std::string("ibm.") + log ); }

namespace server {

/*!
 * \brief runjob_server logger.
 */
extern const char* log;

namespace commands {

/*!
 * \brief runjob_server commands logger.
 */
extern const char* log;

} // commands
} // server

namespace mux {

/*!
 * \brief runjob_mux logger.
 */
extern const char* log;

/*!
 * \brief Get the LoggerPtr for this log.
 */
inline log4cxx::LoggerPtr getLogger() { return log4cxx::Logger::getLogger( std::string("ibm.") + log ); }

namespace commands {

/*!
 * \brief runjob_mux commands logger.
 */
extern const char* log;

} // commands
} // mux

namespace client {

/*!
 * \brief runjob logger.
 */
extern const char* log;

} // client

} // runjob

#endif
