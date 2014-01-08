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

#include "tableapi/DBConnectionPool.h"
#include "tableapi/TxObject.h"
#include "tableapi/gensrc/DBTBlock.h"

#include <utility/include/Log.h>

#include <string>

LOG_DECLARE_FILE( "database" );

using std::string;

namespace BGQDB {

STATUS
copyBlock(
        const string& block,
        const string& new_block,
        const string& owner
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    string insertstm;
    SQLRETURN sqlrc;

    if (!isBlockIdValid(new_block, __FUNCTION__)) {
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.NUMCNODES);
    colBitmap.set(dbo.DESCRIPTION);
    colBitmap.set(dbo.ISTORUS);
    colBitmap.set(dbo.SIZEA);
    colBitmap.set(dbo.SIZEB);
    colBitmap.set(dbo.SIZEC);
    colBitmap.set(dbo.SIZED);
    colBitmap.set(dbo.SIZEE);
    colBitmap.set(dbo.MLOADERIMG);
    colBitmap.set(dbo.NODECONFIG);
    colBitmap.set(dbo.OWNER);
    colBitmap.set(dbo.BOOTOPTIONS);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", block.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    sqlrc = tx.setAutoCommit(false);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database auto commit failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", new_block.c_str());
    sprintf(dbo._description,"%s","Generated via copyblock");
    snprintf(dbo._owner, sizeof(dbo._owner), "%s", owner.c_str());

    sqlrc = tx.insert(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    if (dbo._numionodes > 0 ) {  // io block
        insertstm = "insert into tbgqioblockmap (BLOCKID,LOCATION) "
            + string(" select '") + new_block + string("' ,LOCATION ")
            + string(" from tbgqioblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }
    } else
    if (dbo._numcnodes < 512) {  //smallblock
        insertstm = "insert into tbgqsmallblock (BLOCKID,POSINMACHINE,NODECARDPOS) "
            + string(" select '") + new_block + string("' ,POSINMACHINE,NODECARDPOS ")
            + string(" from tbgqsmallblock where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }
    } else {
        insertstm = "insert into tbgqbpblockmap (BLOCKID,BPID, ACOORD, BCOORD, CCOORD, DCOORD) "
            + string(" select '") + new_block + string("' ,BPID, ACOORD, BCOORD, CCOORD, DCOORD ")
            + string(" from tbgqbpblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }

        insertstm = "insert into tbgqswitchblockmap (BLOCKID,SWITCHID,INCLUDE,ENABLEPORTS) "
            + string(" select '") + new_block + string("' ,SWITCHID,INCLUDE,ENABLEPORTS ")
            + string(" from tbgqswitchblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }

        insertstm = "insert into tbgqlinkblockmap (BLOCKID,LINKID) "
            + string(" select '") + new_block + string("' ,LINKID ")
            + string(" from tbgqlinkblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }
    }

    if ( (sqlrc = tx.commit()) != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database commit failed with error: " << sqlrc << " at " << __LINE__  );
        return DB_ERROR;
    }

    return OK;
}

} // BGQDB
