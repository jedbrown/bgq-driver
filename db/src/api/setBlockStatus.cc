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
#include "isBlockFree.h"
#include "swfailure.h"
#include "tableapi/gensrc/DBTBlock.h"
#include "tableapi/TxObject.h"
#include "tableapi/DBConnectionPool.h"

#include <bgq_util/include/string_tokenizer.h>
#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/assign/list_of.hpp>

#include <deque>
#include <map>
#include <string>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

STATUS
setBlockStatus(
        const std::string& block,
        BLOCK_STATUS targetState,
        const std::deque<std::string>& options
        )
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    bool sharedAllocate = false;                // if true, skip isBlockFree checking during allocate
    const char* errtext = NULL;
    std::string user;
    std::string qualifier;
    bool setAction(false);
    bool setNoCheckAction(false);

    typedef std::map<BGQDB::BLOCK_STATUS, std::string> StatusMap;
    static const StatusMap statusStrings = boost::assign::map_list_of
        (BGQDB::FREE, "FREE")
        (BGQDB::ALLOCATED, "ALLOCATED")
        (BGQDB::INITIALIZED, "INITIALIZED")
        (BGQDB::BOOTING, "BOOTING")
        (BGQDB::TERMINATING, "TERMINATING")
        ;

    if (block.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG("Block name is too long.");
        return INVALID_ID;
    }

    // parse options
    if ( !options.empty() ) {
        for (std::deque<std::string>::const_iterator arg = options.begin(); arg != options.end(); ++arg) {
            StringTokenizer tokens;
            tokens.tokenize(*arg, "="); // split based on the equal sign.

            if (tokens[0] == "errmsg") {
                if (tokens.size() > 1) {
                    // optional error message for setBlockStatus(DEALLOCATING)
                    char errtext_buf[sizeof(dbo._errtext)];
                    strncpy(errtext_buf, tokens[1].c_str(), sizeof(dbo._errtext));
                    errtext_buf[sizeof(dbo._errtext)-1] = '\0';
                    errtext = errtext_buf;
                }
            } else if (tokens[0] == "shared") {
                sharedAllocate = true;
            } else if (tokens[0] == "user") {
                if (tokens.size() > 1) {
                    user = tokens[1];
                }
            } else if (tokens[0] == "qualifier") {
                if (tokens.size() > 1) {
                    qualifier = tokens[1];
                }
            } else if (tokens[0] == "action") {
                setAction = true;
            } else if (tokens[0] == "no_check") {
                setNoCheckAction = true;
            } else {
                LOG_WARN_MSG( "unknown option: '" << tokens[0] << "'" );
            }
        }
    }

    // Get the block table entry
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.NUMCNODES);
    colBitmap.set(dbo.ERRTEXT);        // to see if errtext is already set
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", block.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection.");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG("Block name not found.");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    // ok, we got valid block info from the DB
    colBitmap.reset(dbo.ERRTEXT);
    colBitmap.reset(dbo.NUMIONODES);
    dbo._columns = colBitmap.to_ulong();

    BLOCK_STATUS currentState = blockCodeToStatus( dbo._status );

    if ( currentState == INVALID_STATE ) {
        LOG_ERROR_MSG("Block is in unknown state.");
        return FAILED;
    }

    const bool ioBlock = dbo._numionodes != 0;

    const char *targetStateString = NULL;

    switch(targetState) {
    case ALLOCATED:
        if ((currentState == FREE && (sharedAllocate || isBlockFree(&tx, block, ioBlock)))
                || (currentState == TERMINATING && setAction == false))
        {
            // Set status to (A)llocated
            targetStateString = BLOCK_ALLOCATED;
        } else {
            if (currentState == FREE) {
                LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because required block hardware resources are unavailable.");
            } else {
                LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because the block is " << statusStrings.at(currentState));
            }
            return FAILED;
        }
        break;
    case BOOTING:
        if (
                currentState == INITIALIZED ||  // for the reboot case
                currentState == ALLOCATED
           )
        {
            targetStateString = BLOCK_BOOTING;
        } else {
            LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because the block is " << statusStrings.at(currentState));
            return FAILED;
        }
        break;
    case INITIALIZED:
        if (currentState == BOOTING) {
            targetStateString = BLOCK_INITIALIZED;
        } else {
            LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because the block is " << statusStrings.at(currentState));
            return FAILED;
        }
        break;
    case FREE:                // allow free from all states for mmcs initialization
        if (currentState == ALLOCATED   ||
                currentState == INITIALIZED ||
                currentState == BOOTING ||
                currentState == TERMINATING)
        {
            targetStateString = BLOCK_FREE;

            // save any error text in the block record
            // error text is only saved if it is not already set
            if (errtext) {
                if (dbo._ind[DBTBlock::ERRTEXT] == SQL_NULL_DATA || dbo._errtext[0] == '\0') {
                    colBitmap.set(dbo.ERRTEXT);
                    dbo._columns = colBitmap.to_ulong();
                    strncpy(dbo._errtext, errtext, sizeof(dbo._errtext) - 1);
                    dbo._errtext[sizeof(dbo._errtext) - 1] = '\0';
                }
            }
        } else {
            LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because the block is " << statusStrings.at(currentState));
            return FAILED;
        }
        break;
    case TERMINATING:
        if (currentState == ALLOCATED   ||
                currentState == INITIALIZED ||
                currentState == BOOTING)
        {
            targetStateString = BLOCK_TERMINATING;

            // If any jobs are running on the block then cannot set the state to TERMINATING.
            std::string whereClause = std::string() +
                    " where blockid='" + dbo._blockid + "'";

            int nrows = tx.count("BGQJob",whereClause.c_str());

            if (nrows >=1) {
                LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because the block has " << nrows << " jobs." );
                return FAILED;
            }
        } else {
            LOG_WARN_MSG("Cannot set block to " << statusStrings.at(targetState) << " because the block is " << statusStrings.at(currentState));
            return FAILED;
        }
        break;
    default:
        LOG_ERROR_MSG("Block is in unknown state.");
        return FAILED;
    }

    if  (targetStateString == NULL) {
        LOG_ERROR_MSG("Block is in unknown state.");
        return FAILED;
    }

    sprintf(dbo._status, "%s", targetStateString);

    if (targetState == FREE) {
        colBitmap.set(dbo.BLOCKID);
        colBitmap.set(dbo.STATUS);
        colBitmap.set(dbo.ERRTEXT);
        colBitmap.set(dbo.OPTIONS);
        colBitmap.set(dbo.USERNAME);
        dbo._columns = colBitmap.to_ulong();

        // clear the options and user name if going to FREE state
        strcpy(dbo._options,"");
        strcpy(dbo._username,"");
        //        strcpy(dbo._owner,"");    WE NO LONGER CLEAR THE OWNER ON FREE, WE CLEAR THE USERNAME
    }

    if (targetState == ALLOCATED && currentState == FREE ) {
        // Update action field only if allocate request and it came from setBlockAction
        if (setAction) {
            colBitmap.set(dbo.ACTION);
            if (setNoCheckAction) {
                // Set "N" action to indicate boot of I/O block with I/O nodes in error
                strcpy(dbo._action, BLOCK_BOOTING_NO_CHECK);
            } else {
                // Set "B" action to indicate boot action
                strcpy(dbo._action, BLOCK_BOOTING);
            }
        }

        // Update username if current state is free
        colBitmap.set(dbo.USERNAME);

        if (user.empty()) {
            // Get current uid, if no user name is provided (although it should always be provided)
            try {
                bgq::utility::UserId uid;
                user = uid.getUser();
                LOG_WARN_MSG( "Missing user parameter, using current uid: " << user );
            } catch ( const std::runtime_error& e ) {
                LOG_ERROR_MSG( "Could not get current uid: " << e.what() );
                return FAILED;
            }
        }
        // Subtract 1 from size of column for null terminator
        if ( user.size() >= sizeof(dbo._username) - 1 ) {
            LOG_ERROR_MSG( "uid too large: " << user );
            return FAILED;
        }
        (void)strncpy(dbo._username, user.c_str(), sizeof(dbo._username) );

        dbo._columns = colBitmap.to_ulong();
    }

    // Clear the errtext if going from FREE state to anything else
    if (currentState == FREE) {
        colBitmap.set(dbo.ERRTEXT);
        strcpy(dbo._errtext, "");
        dbo._columns = colBitmap.to_ulong();
    }

    // Clear the action if the target is FREE
    if (targetState == FREE) {
        colBitmap.set(dbo.ACTION);
        strcpy(dbo._action, BLOCK_NO_ACTION);
        dbo._columns = colBitmap.to_ulong();
    }

    if (targetState == BOOTING) {
        // Update the qualifier if one was provided
        if (!qualifier.empty() && qualifier.size() < sizeof(dbo._qualifier) )  {
            colBitmap.set(dbo.QUALIFIER);
            (void)strncpy(dbo._qualifier, qualifier.c_str(), sizeof(dbo._qualifier) );
        }

        dbo._columns = colBitmap.to_ulong();
    }

    if (targetState == FREE) {
        clearSoftwareFailure( dbo );
    }

    sqlrc = tx.updateByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    return OK;
}

} // BGQDB
