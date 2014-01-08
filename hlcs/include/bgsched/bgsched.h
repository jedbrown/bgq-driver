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
 * \file bgsched/bgsched.h
 * \brief initialization method.
 *
 * \defgroup V1R1M1 New content in V1R1M1
 *
 * \defgroup V1R2 New content in V1R2
 
 * \defgroup V1R2M1 New content in V1R2M1
 */

#ifndef BGSCHED_H_
#define BGSCHED_H_

#include <string>

namespace bgsched {

/*!
 * \brief Library initialization function.
 *
 * This function must be called before calling any other functions in the bgsched library.
 *
 * The default configuration file name is '/bgsys/local/etc/bg.properties'.
 *
 * \param[in] properties_filename Name of the BGQDB configuration file. If
 * this is an empty string then the BG_PROPERTIES_FILE environment variable will
 * be used for the configuration file name. If the BG_PROPERTIES_FILE
 * environment variable isn't set then the default file name will be used.
 *
 * \throws bgsched::InitializationException with values:
 * - bgsched::InitializationErrors::PropertiesNotFound - if properties file is not found
 * - bgsched::InitializationErrors::MalformedPropertiesFile - if properties file format is invalid
 * - bgsched::InitializationErrors::DatabaseInitializationFailed - if database initialization fails
 */
void init(
        const std::string& properties_filename
    );

/*!
 * \brief Refresh the configuration settings.
 *
 * This function can be called any time after init() to re-read the configuration settings.
 *
 * \return true if configuration settings were refreshed and false if refresh fails. The original
 * configuration settings are maintained if the refresh fails.
 */
bool refreshConfiguration();

/*!
 * \brief Get the I/O usage limit setting from the configuration file.
 *
 * This function can be called any time after init().
 *
 * \return The I/O usage limit.
 */
unsigned getIOUsageLimit();

namespace version {

/*!
 * \brief Library major version number.
 */
extern const unsigned major;

/*!
 * \brief Library minor version number.
 */
extern const unsigned minor;

/*!
 * \brief Library minor modification number.
 */
extern const unsigned mod;

/*!
 * \brief driver name.
 *
 * For example, V1R1M0
 */
extern const char* driver;

} // version

/*!
 * \brief Methods for database connection pooling.
 *
 * \ingroup V1R2M1
 */
namespace connection_pool {

/*!
 * \brief Get the number of connections available in the connection pool.
 * \ingroup V1R2M1
 */
unsigned getAvailable();

/*!
 * \brief Get the number of connections used in the connection pool.
 * \ingroup V1R2M1
 */
unsigned getUsed();

/*!
 * \brief Get the maximum number of connections used in the connection pool since startup.
 * \ingroup V1R2M1
 */
unsigned getMax();

/*!
 * \brief Get the configured number of connections in the connection pool.
 * \ingroup V1R2M1
 */
unsigned getConfigured();

} // connection_pool

} // namespace bgsched

#endif
