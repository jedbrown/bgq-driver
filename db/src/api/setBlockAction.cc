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
/* (C) Copyright IBM Corp.  2010, 2013                              */
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

#include "BGQDBlib.h"

#include "tableapi/gensrc/DBTBlock.h"
#include "tableapi/TxObject.h"
#include "tableapi/DBConnectionPool.h"

#include <utility/include/Log.h>

#include <deque>
#include <string>


LOG_DECLARE_FILE( "database" );

namespace BGQDB {

STATUS
setBlockAction(
        const std::string& block,
        const BLOCK_ACTION action,
        const std::deque<std::string>& options
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    STATUS rc;

    if (block.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG("block name '" << block << "' too long");
        return INVALID_ID;
    }

    // Get the block table entry
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.ACTION);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", block.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG("block name '" << block << "' not found");
        return NOT_FOUND;
    }

    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    // Make sure it was a valid action requested and there is no action already set
    if ((action != CONFIGURE_BLOCK && action != DEALLOCATE_BLOCK && action != CONFIGURE_BLOCK_NO_CHECK) || strcmp(dbo._action, BLOCK_NO_ACTION) != 0) {
        if (strcmp(dbo._action, BLOCK_NO_ACTION) != 0) {
            LOG_DEBUG_MSG("block action is already pending");
            return DUPLICATE;
        } else {
            LOG_ERROR_MSG("invalid block action request");
        }
        return FAILED;
    }

    // For allocating block we will have setBlockStatus set both the status and action fields
    if (action == CONFIGURE_BLOCK || action == CONFIGURE_BLOCK_NO_CHECK) {
        std::deque<std::string> configure_options(options);
        configure_options.push_back("action");
        // Need to further differentiate which type of action to set on the block
        if (action == CONFIGURE_BLOCK_NO_CHECK) {
            // This indicate to boot I/O block with I/O nodes in error
            configure_options.push_back("no_check");
        }
        if ((rc = setBlockStatus(block, ALLOCATED, configure_options)) != OK) {
            return rc;
        }
    } else {
        strcpy(dbo._action, BLOCK_DEALLOCATING);
        const std::string whereClause = std::string("where status <> 'F' and blockid  = '") + block + std::string("'");
        for (std::deque<std::string>::const_iterator arg = options.begin(); arg != options.end(); ++arg) {
            if ( arg->substr(0,7) == "errmsg=" ) {
                strncpy( dbo._errtext, arg->substr(7).c_str(), sizeof(dbo._errtext) - 1 );
                dbo._errtext[sizeof(dbo._errtext) - 1] = '\0';
                colBitmap.set(dbo.ERRTEXT);
                dbo._columns = colBitmap.to_ulong();
                break;
            }
        }

        sqlrc = tx.update(&dbo, whereClause.c_str());
        if ((sqlrc != SQL_NO_DATA_FOUND) && (sqlrc != SQL_SUCCESS)) {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}

} // BGQDB

