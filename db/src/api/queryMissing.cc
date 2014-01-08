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

#include "dataconv.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/TxObject.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "database" );

using std::string;

namespace BGQDB {

STATUS
queryMissing(
        const string& block,
        std::vector<string>& missing,
        DIAGS_MODE diags
)
{
    char col1[65];
    string sqlstr;
    SQLLEN ind1,ind2;
    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    unsigned numcnodes=0, numionodes=0;
    bool foundMissing = false;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    sqlstr.append("select numcnodes,numionodes from bgqblock where blockid='");
    sqlstr.append(block);
    sqlstr.push_back('\'');
    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_LONG, &numcnodes, 0, &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 2, SQL_C_LONG, &numionodes, 0, &ind2)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_ERROR_MSG("Block " << block << " not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    const bool ioblock = (numionodes > 0);

    SQLCloseCursor(hstmt);

    // diags NO_DIAGS  (A)    NORMAL_DIAGS (A,E,F)   SVCACTION_DIAGS (A,E,S,F)
    if (diags == NO_DIAGS) {
        if (ioblock) {
            sqlstr = "select 'IODRAWER: ' || a.location from bgqiodrawer a, bgqioblockmap b where b.blockid = '"
                + block
                + string("' and substr(b.location,1,6) = a.location group by a.location having min(status) not in ('A','F') ")
                + string(" union all select 'IONODE: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
                + block
                + string("' and b.location = a.location group by a.location having min(status) not in ('A','F') ")
                + string(" union all select 'IONODE: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
                + block
                + string("' and substr(a.location,1,6) = b.location group by a.location having min(status) not in ('A','F') ");
        } else {
            sqlstr =  "select 'MIDPLANE: ' || location from bgqmidplane a, bgqbpblockmap b where b.blockid = '"
                + block
                + string("' and b.bpid = a.location group by location having min(status) not in ('A','F') ")
                + string(" union all  select 'NODEBOARD: ' || c.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c ")
                + string(" where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and c.midplanepos = a.location  group by c.location ")
                + string(" having min(c.status) <> 'A' ")
                + string("  union all select 'NODE: ' || d.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c, bgqnode d  where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and c.midplanepos = a.location and d.nodecardpos = c.position and d.midplanepos = c.midplanepos  ")
                + string(" group by d.location having min(d.status) not in ('A','F') ")
                + string("  union all select 'NODE: ' || d.location from bgqsmallblock b, bgqnode d  where b.blockid = '")
                + block
                + string("' and b.posinmachine = d.midplanepos  and d.nodecardpos = b.nodecardpos  ")
                + string(" group by d.location having min(d.status) not in ('A','F') ")
                + string(" union  all select 'CABLE: ' || linkid from tbgqlinkblockmap where blockid = '")
                + block
                + string("' and linkid in (select source || destination from bgqlink where status <> 'A' ) ")
                + string(" union  select 'MIDPLANE: ' || location from bgqmidplane a, bgqsmallblock b where b.blockid = '")
                + block
                + string("' and b.posinmachine = a.location group by location having min(status) <> 'A' ")
                + string(" union select 'NODEBOARD: ' || c.location ")
                + string(" from bgqsmallblock b, bgqnodecard c where b.blockid = '")
                + block
                + string("' and b.posinmachine = c.midplanepos and c.position = b.nodecardpos group by c.location having min(c.status) <> 'A' ")
                + string(" union select 'PASS THROUGH NODEBOARD: ' || location from bgqnodecard where midplanepos in (select substr(switchid,3,6) from bgqswitchblockmap where blockid='")
                + block
                + string("' and substr(switchid,3,6) not in (select bpid from bgqbpblockmap where blockid='")
                + block
                + string("')) and status <> 'A'")
                ;
        }
    } else {
        string statusFlags;
        if (diags == NORMAL_DIAGS) {
            statusFlags = "('A', 'E', 'F')";
        } else { // service action
            statusFlags = "('A', 'E', 'S', 'F')";
        }

        if (ioblock) {
            sqlstr = "select 'IODRAWER: ' || a.location from bgqiodrawer a, bgqioblockmap b where b.blockid = '"
                + block
                + string("' and b.location = a.location and status not in ") + statusFlags
                + string(" union all select 'IONODE: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
                + block
                + string("' and substr(a.location,1,6) = b.location and status not in ") + statusFlags;
        } else {
            sqlstr =  "select 'MIDPLANE: ' || location from bgqmidplane a, bgqbpblockmap b where b.blockid = '"
                + block
                + string("' and b.bpid = a.location  and status not in ") + statusFlags
                + string(" union all  select 'NODEBOARD: ' || c.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c ")
                + string(" where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and c.midplanepos = a.location   ")
                + string(" and c.status not in ") + statusFlags + string(" union all select 'NODE: ' || d.location from bgqmidplane a, bgqbpblockmap b, bgqnode d  where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and d.midplanepos = a.location  ")
                + string(" and d.status not in ") + statusFlags;
        }
    }

    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    // bind cols
    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, col1, 65, &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    for (;sqlrc == SQL_SUCCESS;) {
        trim_right_spaces(col1);
        missing.push_back(col1);
        foundMissing = true;
        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);

    if (foundMissing == false) {   //check for duplicate MAC or IP addresses
        sqlstr = "select 'DUPLICATE IP: ' || d.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c, bgqnode d  "
            + string(" where b.blockid = '")
            + block
            + string("' and b.bpid = a.location and c.midplanepos = a.location  and d.nodecardpos = c.position and d.midplanepos = c.midplanepos and d.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ")
            + string(" union select 'DUPLICATE IP: ' || d.location ")
            + string(" from bgqmidplane a, bgqsmallblock b, bgqnodecard c, bgqnode d  where b.blockid = '")
            + block
            + string("' and b.posinmachine = a.location and c.midplanepos = a.location and c.position = b.nodecardpos and d.nodecardpos = c.position and d.midplanepos = c.midplanepos  and d.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ")

            + string(" union  select 'DUPLICATE IP: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
            + block
            + string("' and b.location = a.location and a.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ")
            + string(" union  select 'DUPLICATE IP: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
            + block
            + string("' and substr(a.location,1,6) = b.location  and a.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ");



        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }

        if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, col1, 65, &ind1)) != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __LINE__ );
            return DB_ERROR;
        }

        sqlrc =  SQLFetch(hstmt);
        for(;sqlrc == SQL_SUCCESS;) {
            missing.push_back(col1);
            sqlrc = SQLFetch(hstmt);
        }

        SQLCloseCursor(hstmt);
    }

    return OK;
}

} // BGQDB
