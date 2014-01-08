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

#ifndef _DBOBJ_H
#define _DBOBJ_H

#include "../BGQDBlib.h"

#include "../cxxdb/fwd.h"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <iostream>
#include <ostream>
#include <string>
#include <typeinfo>
#include <vector>

#include <boost/shared_ptr.hpp>

#define DBOBJ_MAX_COLUMNS  64

namespace BGQDB {

class DBObj {
public:

    struct ColumnType {
        enum Value {
            Char,
            Varchar,
            Bigint,
            Integer,
            Timestamp,
            Smallint,
            Double,
            Decimal,
            Other
        };
    };

    typedef std::vector<std::string> ColumnNames;

    /*!
     * \brief ctor
     */
    explicit DBObj( const BGQDB::ColumnsBitmap& cols ) :
        _columns(cols.to_ulong()),
        _ind()
    {
        bzero(_ind, sizeof(_ind));
    }

    void setColumns(
            const BGQDB::ColumnsBitmap& cols
        )
    { _columns = cols.to_ulong(); }

    virtual ~DBObj() { }
    virtual SQLRETURN bind_col(SQLHANDLE hdbc, SQLHANDLE hstmt)=0;
    virtual SQLRETURN bind_param(SQLHANDLE hdbc, SQLHANDLE hstmt)=0;
    virtual SQLRETURN fetch_col(SQLHANDLE hstmt, SQLHANDLE  hdbc)=0;
    virtual void dump(std::ostream& out=std::cout)=0;
    virtual const std::type_info* typeinfo() { return &typeid(DBObj); }
    virtual const std::string getDeleteStatement(bool  /*bykey*/) {return std::string("");}
    virtual const std::string getInsertStatement()           {return std::string("");}
    virtual const std::string getUpdateStatement(bool /*bykey*/) {return std::string("");}
    virtual const std::string getSelectStatement(bool /*bykey*/) {return std::string("");}

    unsigned long long _columns;

    SQLLEN  _ind[DBOBJ_MAX_COLUMNS];


    /*!
     * Returns a string like "INSERT INTO table_name ( cols ) VALUES ( ?, ... )"
     * Where table_name is the table name,
     * cols is the names of the columns as set in _columns,
     * and there's a ? for each column name.
     */
    const std::string calcInsertSql() const;

    /*!
     * Returns a string like "SELECT cols FROM table_name where_clause"
     * Where table_name is the table name,
     * cols is the names of the columns as set in _columns.
     */
    const std::string calcSelectSql( const std::string& where_clause ) const;

    /*! \brief Get the name of the table. */
    virtual const char* getTableName() const =0;

    /*! \brief Gets the number of columns. */
    virtual unsigned getColumnCount() const =0;

    /*! \brief Calculates a comma-separated list of the column names, based on _columns. */
    const std::string calcColumnNamesCommaSeparated() const;

    /*! \brief Calculates the column names in a vector, based on _columns. */
    virtual ColumnNames calcColumnNames() const =0;

    /*! \brief Returns a StatementPtr that's set up to insert to the table.
     */
    cxxdb::UpdateStatementPtr prepareInsert( cxxdb::Connection& conn ) const;

    /*! \brief Returns a StatementPtr that's set up to query to the table.
     *
     *  The SQL is like "SELECT cols FROM table where_clause".
     *
     *  where_clause will typically have "where" in it.
     */
    cxxdb::QueryStatementPtr prepareSelect(
            cxxdb::Connection& conn,
            const std::string& where_clause,
            const cxxdb::ParameterNames& param_names
        ) const;

protected:

    /*!
     *  \brief Remove trailing blanks from a c-style string.
     *
     *  DB2 returns CHAR(x) columns space-padded out to x characters. This method
     *  removes those blanks.
     *
     *  From the end of the string, looks for first non-space. Replaces character
     *  after non-space with a null character to re-terminate the string.
     *
     */
    static void _trim_spaces(
            char *str,          //!< [inout] string which is modified to remove any trailing blanks.
            size_t str_size     //!< [in]    the bytes in the string (doesn't include extra char for null-terminator).
            ) {
        if ( str_size < 2 )  return;

        char *p(str + (str_size-1));
        while ( (*p == ' ') && (p != str) )  --p;

        if ( (p == str) && (*p == ' ') )  *p = '\0';
        else  *(p+1) = '\0';
    }
};

} // BGQDB

#endif
