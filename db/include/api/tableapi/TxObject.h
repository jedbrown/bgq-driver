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

#ifndef TXOBJECT
#define TXOBJECT

#include "DBObj.h"
#include "DBConnection.h"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <boost/utility.hpp>

namespace BGQDB {

class DBConnectionPool;

/*!
 * \brief
 */
class TxObject : boost::noncopyable
{
public:
    /*!
     * \brief ctor
     */
    explicit TxObject(DBConnectionPool& pool);

    /*!
     * \brief dtor
     */
    ~TxObject();

    /*!
     * whereclause is any sql boolean expression using table columns
     * Ex: serialnumber = 'thisserialnumber' AND description = 'this description'
     */
    int count(const char *tbname, const char *whereclause=NULL);
    SQLRETURN del(DBObj *o, const char *whereclause=NULL);   // delete from db
    SQLRETURN delByKey(DBObj *o);
    SQLRETURN execStmt(const char *stmt);  // open call to the database
    SQLRETURN execQuery(const char *stmt, SQLHANDLE *handle); // open query to the database
    SQLRETURN fetch(DBObj *o);             // returns the query's next row from db
    SQLRETURN close(DBObj *o);                                  // close the query
    SQLRETURN insert(DBObj *o, bool deleteFirst = false);       // insert a row
    SQLRETURN query(DBObj *o, const char *whereclause=NULL);    // issues a db query
    SQLRETURN queryByKey(DBObj *o);
    SQLRETURN update(DBObj *o, const char *whereclause=NULL);   // issues a db update
    SQLRETURN updateByKey(DBObj *o);

    SQLRETURN rollback();                    // rollback all connection's transactions
    SQLRETURN commit();                      // commit all connection's transaction

    int setAutoCommit(bool enableAutoCommit);
    DBConnection::Ptr getConnection() { return _dbcon; };
    SQLRETURN alloc(SQLHANDLE *stmt);

protected:
    DBConnection::Ptr         _dbcon;
    SQLHANDLE                 _hstmt;
    bool                      _autocommit;
    bool                      _do_rollback;
    SQLRETURN  execSql(const char *stmt, int op, DBObj *o=NULL, const char *whereclause=NULL);
};

} // BGQDB

#endif
