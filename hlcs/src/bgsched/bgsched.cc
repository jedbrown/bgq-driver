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

#include <bgsched/bgsched.h>

#include <bgsched/InitializationException.h>
#include <bgsched/DatabaseException.h>

#include "bgsched/utility.h"

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/BGQDBlib.h>
#include <db/include/api/ioUsage.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/version.h>

#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/basicconfigurator.h>

#include <stdexcept>
#include <sstream>

using namespace log4cxx;
using std::string;

LOG_DECLARE_FILE( "bgsched" );

namespace bgsched {

static void initialize_db_api(
        const bgq::utility::Properties::Ptr properties
    )
{
    try {
        BGQDB::init(properties, "bgsched");
    } catch (const std::runtime_error& rte) {
        THROW_EXCEPTION(
                bgsched::InitializationException,
                bgsched::InitializationErrors::DatabaseInitializationFailed,
                "Error initializing the bgsched API. Initializing database failed with '" << rte.what() << "'"
            );
    }
}

void init(
        const string& properties_filename
    )
{
    bgq::utility::Properties::Ptr properties;
    try {
        properties = bgq::utility::Properties::Ptr( new bgq::utility::Properties(properties_filename) );

        // set Properties so we can get them later
        bgsched::setProperties(properties);
    } catch (const bgq::utility::Properties::FileError& e) {
        THROW_EXCEPTION_NO_LOGGER(
                bgsched::InitializationException,
                bgsched::InitializationErrors::PropertiesNotFound,
                "Properties file not found. Error is: " << e.what()
                );
    } catch (const std::runtime_error& e) {
        THROW_EXCEPTION_NO_LOGGER(
                bgsched::InitializationException,
                bgsched::InitializationErrors::MalformedPropertiesFile,
                "Malformed properties file. Error is: " << e.what()
                );
    }

    // initialize logging
    bgq::utility::initializeLogging( *properties );

    // initialize database
    initialize_db_api( properties );

    // initialize I/O usage limit property value
    BGQDB::ioUsage::init();
}

bool refreshConfiguration()
{
    // Reload properties set on bgsched::init()
    LOG_INFO_MSG("Refreshing Scheduler API configuration");
    try {
        const bool isRefresh = bgsched::getProperties()->reload(bgsched::getProperties()->getFilename());
        if (!isRefresh) {
            LOG_ERROR_MSG("Refreshing Scheduler API configuration failed.");
        }
        return isRefresh;
    } catch (const std::exception& e ) {
        LOG_ERROR_MSG("Refreshing Scheduler API configuration: " << e.what());
        return false;
    }
}

unsigned getIOUsageLimit()
{
    return BGQDB::ioUsage::getUsageLimit();
}

namespace version {

const unsigned major = MAJOR_VERSION;

const unsigned minor = MINOR_VERSION;

const unsigned mod = MOD_VERSION;

const char* driver = bgq::utility::DriverName;

} // namespace version

namespace connection_pool {

unsigned
getAvailable()
{
    return BGQDB::DBConnectionPool::instance().availableCount();
}

unsigned
getUsed()
{
    return BGQDB::DBConnectionPool::instance().usedCount();
}

unsigned
getMax()
{
    return BGQDB::DBConnectionPool::instance().maxCount();
}

unsigned
getConfigured()
{
    return BGQDB::DBConnectionPool::instance().size();
}

} // connection_pool
} // namespace bgsched
