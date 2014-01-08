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

#include "Database.h"

#include "common/Properties.h"

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/assert.hpp>
#include <unistd.h>
#include <pwd.h>

LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


Database::Database(
        const std::string& name
        ) :
    _handle(NULL)
{
    std::string dbname = name;
    if (dbname.empty()) {
        // use properties file
        dbname = common::Properties::getProperty("sqlite");
    }

    size_t loc;
    if ((loc=dbname.find("$USERID")) != std::string::npos) {
        // Get the current username
        bgq::utility::UserId uid;
        dbname.replace(loc,7,uid.getUser());      // replace $USERID in the dbname with the current userid
    }

    LOG_TRACE_MSG("using database " << dbname);

    // open db
    int rc = sqlite3_open_v2(
            dbname.c_str(),
            &_handle,
            SQLITE_OPEN_READWRITE | // flags
            SQLITE_OPEN_CREATE |    // flags
            SQLITE_OPEN_FULLMUTEX,  // flags
            NULL                                            // name of VFS module
            );
    if (rc != SQLITE_OK) {
        LOG_ERROR_MSG("could not open database: " << sqlite3_errmsg(_handle));
        throw std::runtime_error("could not open database");
    }

    // ensure we created a database handle
    BOOST_ASSERT( _handle );
}

Database::~Database()
{
    (void)sqlite3_close(_handle);
}


} } // namespace mmcs::lite
