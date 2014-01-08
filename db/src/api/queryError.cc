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
#include "tableapi/gensrc/DBVIonode.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

STATUS
queryError(
        const std::string& block,
        std::vector<std::string>& missing
)
{
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    std::ostringstream sql;
    sql << "select numionodes from bgqblock where blockid='" << block << "'";
    SQLHANDLE hstmt;
    SQLRETURN sqlrc = tx.execQuery( sql.str().c_str(), &hstmt );
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    SQLLEN ind1;
    unsigned numionodes=0;
    sqlrc = SQLBindCol( hstmt, 1, SQL_C_LONG, &numionodes, 0, &ind1 );
    if ( sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch( hstmt );
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    if ( numionodes == 0 ) {
        LOG_INFO_MSG( block << " is not an I/O block" );
        return INVALID_ARG;
    }

    sql.str("");
    sql <<

 "WITH badIon AS ("

" SELECT location"
  " FROM bgqIoNode"
  " WHERE status = 'E'"
" UNION ALL"
" SELECT ion.location"
  " FROM bgqIoDrawer AS d"
       " JOIN"
       " bgqIoNode AS ion"
       " ON ion.ioPos = d.location"
  " WHERE d.status <> 'A'"

" ), ioLoc AS ("

" SELECT location"
  " FROM bgqIoBlockMap"
  " WHERE blockId = '" << block << "'"
" UNION ALL"
" SELECT ion.location"
  " FROM bgqIoBlockMap AS ibm"
       " JOIN"
       " bgqIoDrawer AS d"
       " ON ibm.location = d.location"
       " JOIN"
       " bgqIoNode AS ion"
       " ON ion.ioPos = d.location"
  " WHERE ibm.blockId = '" << block << "'"

" )"

" SELECT badIon.location"
  " FROM badIon"
       " JOIN"
       " ioLoc"
       " ON badIon.location = ioLoc.location"
  " GROUP BY badIon.location"

       ;

    sqlrc = tx.execQuery( sql.str().c_str(), &hstmt );
    if ( sqlrc != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    char location[DBVIonode::LOCATION_SIZE + 1];
    sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, location, sizeof(location), &ind1);
    if ( sqlrc != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }

    while ( (sqlrc = SQLFetch(hstmt)) == SQL_SUCCESS ) {
        missing.push_back(location);
    }

    SQLCloseCursor(hstmt);

    return OK;
}

} // BGQDB
