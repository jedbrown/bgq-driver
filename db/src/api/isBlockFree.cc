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

#include "isBlockFree.h"
#include "tableapi/gensrc/DBTBpblockmap.h"
#include "tableapi/gensrc/DBVBpblockstatus.h"
#include "tableapi/gensrc/DBTSmallblock.h"
#include "tableapi/gensrc/DBTLinkblockmap.h"
#include "tableapi/gensrc/DBTSwitchblockmap.h"
#include "tableapi/gensrc/DBVSwitchblockstatus.h"
#include "tableapi/gensrc/DBVLinkblockstatus.h"
#include "tableapi/gensrc/DBTIoblockmap.h"
#include "tableapi/TxObject.h"
#include "tableapi/DBConnectionPool.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

// getting the caller TxObject to do the database access in the same transaction
bool
isBlockFree(
        TxObject *tx,
        const std::string& block,
        const bool isIOBlock
        )
{
    DBTBpblockmap bmap;
    DBVBpblockstatus bstat;
    DBTSmallblock sb;
    DBTLinkblockmap lmap;
    DBTSwitchblockmap smap;
    DBVSwitchblockstatus sstat;
    DBVLinkblockstatus lstat;
    DBTIoblockmap io;
    ColumnsBitmap colBitmap;

    SQLRETURN sqlrc;

    if (isIOBlock) {
        // Check for I/O block overlap
        colBitmap.set(io.BLOCKID);
        io._columns = colBitmap.to_ulong();

        std::string where = "  where (substr(location,1,6)  in (select location from bgqioblockmap a, bgqblock b ";
        where += "        where b.status<>'F' and a.blockid = b.blockid )    ";
        where += "   or   location              in (select location from bgqioblockmap a, bgqblock b ";
        where += "        where b.status<>'F' and a.blockid = b.blockid )    ";
        where += "   or   location              in (select substr(location,1,6) from bgqioblockmap a, bgqblock b ";
        where += "        where b.status<>'F' and a.blockid = b.blockid )  )   ";
        where += "   and location not in (select location from bgqiodrawer where status = 'S') ";
        where += "   and  blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&io, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
            return false;
        }

        // Since we are querying for IO nodes/drawers that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&io);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more I/O nodes/drawers required by I/O block "  << block << " are not available.");
            return false;
        }
    } else {
        // Check for compute block overlap
        colBitmap.set(bmap.BLOCKID);
        bmap._columns = colBitmap.to_ulong();

        std::string where = " where ( bpid in (select bpid from ";
        where += bstat.getTableName();
        where += " where blockstatus<>'F' and blockstatus<>'E' and blockstatus<>' ') or bpid in (select posinmachine from bgqsmallblock s, bgqblock b ";
        where += " where b.blockid = s.blockid and status <> 'F' and status <>'E'))  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&bmap, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
            return false;
        }

        // Since we are querying for BPs that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&bmap);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more midplanes required by compute block "  << block << " are not available.");
            return false;
        }

        colBitmap.reset();
        colBitmap.set(lmap.BLOCKID);
        lmap._columns = colBitmap.to_ulong();

        where = " where linkid in (select linkid from ";
        where += lstat.getTableName();
        where += " where blockstatus<>'F' and blockstatus<>'E')  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&lmap, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
            return false;
        }

        // Since we are querying for links that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&lmap);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more links required by compute block "  << block << " are not available.");
            return false;
        }

        colBitmap.reset();
        colBitmap.set(smap.BLOCKID);
        smap._columns = colBitmap.to_ulong();

        where = " colBitmap where exists  (select switchid from ";
        where += sstat.getTableName();
        where += " stat where stat.switchid = colBitmap.switchid and (stat.include <>  colBitmap.include)  ";
        where += " and blockstatus<>'F' and blockstatus<>'E' and blockstatus<>' ')  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&smap, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
            return false;
        }

        // Since we are querying for switches that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&smap);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more switches required by compute block "  << block << " are not available.");
            return false;
        }

        colBitmap.reset();
        colBitmap.set(sb.BLOCKID);
        sb._columns = colBitmap.to_ulong();

        where = " sb  where ( posinmachine in (select bpid from ";
        where += bstat.getTableName();
        where += " where blockstatus<>'F' and blockstatus<>'E' and blockstatus<>' ') or exists (select s.nodecardpos from bgqsmallblock s, bgqblock b ";
        where += " where b.blockid = s.blockid and status <> 'F' and status <>'E' and ( " ;
        where += " sb.nodecardpos=s.nodecardpos  ";
        where += ")  and sb.posinmachine=s.posinmachine ))  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&sb, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
            return false;
        }

        // Since we are querying for node boards that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&sb);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more node boards required by compute block "  << block << " are not available.");
            return false;
        }
    }
    // No overlapping hardware resources found
    return true;
}

} // BGQDB

