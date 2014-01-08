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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "areBlocksActive.h"

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

bool
areBlocksActive()
{
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_WARN_MSG( "Could not get database connection" );
        return false;
    }

    cxxdb::ResultSetPtr result = connection->query("SELECT count(*) as count from tbgqblock where status <> 'F'");
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "Could not get active block count" );
        return false;
    }
    if ( result->columns()[ "count" ].getInt32() ) {
        LOG_INFO_MSG( "Found " << result->columns()[ "count" ].getInt32() << " block(s) active" );
        return true;
    }

    result = connection->query(
            "SELECT count(*) as count from tbgqnodecard where status = 'S' "
            "UNION ALL "
            "SELECT count(*) as count from tbgqiodrawer where status = 'S'"
            );
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "Could not get node boards in Service count" );
        return false;
    }
    if ( result->columns()[ "count" ].getInt32() ) {
        LOG_INFO_MSG( "Found " << result->columns()[ "count" ].getInt32() << " I/O drawer(s) in Service" );
        return true;
    }
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "Could not get I/O drawers in Service count" );
        return false;
    }
    if ( result->columns()[ "count" ].getInt32() ) {
        LOG_INFO_MSG( "Found " << result->columns()[ "count" ].getInt32() << " node board(s) in Service" );
        return true;
    }

    // Getting here means no blocks are active, and no hardware is in service
    LOG_DEBUG_MSG( "no blocks are active" );
    return false;
}

} } // namespace mmcs::server
