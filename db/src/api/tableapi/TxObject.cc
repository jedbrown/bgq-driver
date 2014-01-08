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

#include "tableapi/TxObject.h"

#include "tableapi/gensrc/bgqtableapi.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/utilcli.h"

#include <utility/include/Log.h>

#include <iostream>
#include <stdio.h>
#include <string>

LOG_DECLARE_FILE( "database" );

#define SELECT   0
#define UPDATE   1
#define INSERT   2
#define DELETE   3
#define EXEC     4
#define QUERY    5

namespace BGQDB {

TxObject::TxObject(
        DBConnectionPool& pool
) :
    _dbcon(pool.checkout()), // Empty if connection is not obtained
    _hstmt(SQL_NULL_HSTMT),
    _autocommit(true),
    _do_rollback(false)
{
    LOG_TRACE_MSG("TxObject constructor");
}

TxObject::~TxObject()
{
    if ( (! _autocommit) && _do_rollback )
    {
        LOG_WARN_MSG( "Rolling back transaction automatically because it was not committed." );

        SQLRETURN sqlrc = rollback();
        if ( sqlrc != SQL_SUCCESS ) {
            LOG_ERROR_MSG( "Failed to rollback the transaction." );
        }
    }

    // Free statement handle if it's a valid handle
    if (_hstmt != SQL_NULL_HSTMT)  {
        SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
    }
}

SQLRETURN
TxObject::commit()
{
    SQLRETURN sqlrc = _dbcon->commit();
    if ( sqlrc == SQL_SUCCESS ) {
        _do_rollback = false;
    }
    return sqlrc;
}

int
TxObject::count(
        const char *tbname,
        const char *whereclause
)
{
    SQLHANDLE         hdbc = _dbcon->getConnHandle();  // connection handle

    SQLRETURN         sqlrc = SQL_SUCCESS;
    int               numRecords = -1;

    // Allocate one or more statement handle
    if (_hstmt != SQL_NULL_HSTMT) {
        SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
        STMT_HANDLE_CHECK(_hstmt, hdbc, rc);
    }
    sqlrc = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &_hstmt ) ;
    DBC_HANDLE_CHECK(hdbc, sqlrc);
    sqlrc = SQLBindCol(_hstmt, 1, SQL_C_LONG, &numRecords, 4, NULL);
    STMT_HANDLE_CHECK(_hstmt, hdbc, sqlrc);

    char *fullstmt;
    if (whereclause == NULL) {
        fullstmt = new char [21+strlen(tbname)+1];
        sprintf(fullstmt,"select count(*) from %s", tbname);
    } else {
        fullstmt = new char [21+strlen(tbname)+strlen(whereclause)+1+1];
        sprintf(fullstmt,"select count(*) from %s %s", tbname, whereclause);
    }

    sqlrc = SQLExecDirect(_hstmt,(SQLCHAR*) fullstmt, SQL_NTS);
    delete[] fullstmt;

    STMT_HANDLE_CHECK(_hstmt, hdbc, sqlrc);

    sqlrc = SQLFetch(_hstmt);
    STMT_HANDLE_CHECK(_hstmt, hdbc, sqlrc);

    int rc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
    STMT_HANDLE_CHECK(_hstmt, hdbc, rc);

    _hstmt = SQL_NULL_HSTMT;
    return numRecords;
}

SQLRETURN
TxObject::delByKey(
        DBObj *o
)
{
    return (execSql(o->getDeleteStatement(true).c_str(), DELETE, o));
}

SQLRETURN
TxObject::del(
        DBObj *o,
        const char *whereclause
)
{
    _do_rollback = true;
    return (execSql(o->getDeleteStatement(false).c_str(), DELETE, o, whereclause));
}

SQLRETURN
TxObject::execStmt(
        const char *stmt
)
{
    _do_rollback = true;
    return (execSql(stmt, EXEC));
}

SQLRETURN
TxObject::execQuery(
        const char *stmt,
        SQLHANDLE *stmthand
)
{
    SQLRETURN rc = execSql(stmt, QUERY);
    *stmthand = _hstmt;
    return rc;
}

SQLRETURN
TxObject::fetch(
        DBObj *o
)
{
    SQLHANDLE   hdbc = _dbcon->getConnHandle();  // connection handle

    LOG_TRACE_MSG("Fetch each row and return");

    SQLRETURN   sqlrc = o->fetch_col( hdbc, _hstmt);
    // Free the statement handle
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_TRACE_MSG("No data found on fetch");
        SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
        STMT_HANDLE_CHECK(_hstmt, hdbc, rc);
        _hstmt = SQL_NULL_HSTMT;
    }
    return sqlrc;
}

SQLRETURN
TxObject::close(
        DBObj*
)
{
    SQLRETURN sqlrc = SQL_SUCCESS;
    if (_hstmt != SQL_NULL_HSTMT) {
        sqlrc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
        _hstmt = SQL_NULL_HSTMT;
    }
    return sqlrc;
}

SQLRETURN
TxObject::insert(
        DBObj *o,
        bool deleteFirst
)
{
    _do_rollback = true;

    if (deleteFirst) {
        delByKey(o);
    }

    return (execSql(o->getInsertStatement().c_str(), INSERT, o));
}

SQLRETURN
TxObject::query(
        DBObj *o,
        const char *whereclause
)
{
    return (execSql(o->getSelectStatement(false).c_str(), SELECT, o, whereclause));
}

SQLRETURN
TxObject::queryByKey(
        DBObj *o
)
{
    return (execSql(o->getSelectStatement(true).c_str(), SELECT, o));
}

SQLRETURN
TxObject::rollback()
{
    _do_rollback = false;
    return _dbcon->rollback();
}

int
TxObject::setAutoCommit(
        bool enableAutoCommit
)
{
    SQLRETURN sqlrc = _dbcon->setAutoCommit(enableAutoCommit);
    if ( sqlrc == SQL_SUCCESS ) {
        _autocommit = enableAutoCommit;
        if ( ! _autocommit ) {
            _do_rollback = false;
        }
    }
    return sqlrc;
}

SQLRETURN
TxObject::updateByKey(
        DBObj *o
)
{
    _do_rollback = true;

    return (execSql(o->getUpdateStatement(true).c_str(), UPDATE, o));
}

SQLRETURN
TxObject::update(DBObj *o, const char *whereclause)
{
    _do_rollback = true;

    return (execSql(o->getUpdateStatement(false).c_str(), UPDATE, o, whereclause));
}

SQLRETURN
TxObject::alloc(
        SQLHANDLE *stmthand
)
{
    SQLHANDLE   hdbc = _dbcon->getConnHandle();  // connection handle
    SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &_hstmt) ;
    *stmthand = _hstmt;
    return sqlrc;
}

SQLRETURN
TxObject::execSql(
        const char *stmt,
        int op,
        DBObj *o,
        const char *whereclause
)
{
    _do_rollback = true;

    // Check statement for null string or blank value
    if (stmt == NULL) {
        LOG_ERROR_MSG("SQL statement to execute is empty");
        return SQL_ERROR;
    } else if (strlen(stmt) == 0) {
        LOG_ERROR_MSG("SQL statement to execute is blank");
        return SQL_ERROR;
    }

    SQLHANDLE   hdbc = _dbcon->getConnHandle();  // connection handle
    SQLRETURN   sqlrc;

    // allocate one or more statement handles
    if (_hstmt != SQL_NULL_HSTMT) {
        SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
        STMT_HANDLE_CHECK(_hstmt, hdbc, rc);
    }
    sqlrc = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &_hstmt ) ;
    DBC_HANDLE_CHECK(hdbc, sqlrc);

    char *fullstmt;
    if (whereclause==NULL) {
        fullstmt= new char [strlen(stmt)+1];
        sprintf(fullstmt,"%s", stmt);
    } else {
        fullstmt= new char [strlen(stmt)+strlen(whereclause)+1+1];
        sprintf(fullstmt,"%s %s", stmt, whereclause);
    }

    switch (op) {
    case SELECT:
        LOG_TRACE_MSG("Binding columns");
        o->bind_col( hdbc, _hstmt);
        break;
    case UPDATE:
    case INSERT:
        LOG_TRACE_MSG("Binding parameters");
        o->bind_param( hdbc, _hstmt);
        break;
    case DELETE:
    case QUERY:
    case EXEC:
    default:
        break;
    }

    LOG_TRACE_MSG("Exec direct: " << fullstmt);
    sqlrc = SQLExecDirect(_hstmt,(SQLCHAR*) fullstmt, SQL_NTS);

    delete[] fullstmt;

    STMT_HANDLE_CHECK(_hstmt, hdbc, sqlrc);

    if ((op != SELECT) && (op != QUERY)) {
        SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_STMT, _hstmt);
        STMT_HANDLE_CHECK(_hstmt, hdbc, rc);
        _hstmt = SQL_NULL_HSTMT;
    }

    return sqlrc;
}

} // BGQDB
