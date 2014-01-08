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

#ifndef CXXDB_STATEMENT_HANDLE_H_
#define CXXDB_STATEMENT_HANDLE_H_


#include "ColumnInfo.h"
#include "ParameterInfo.h"

#include <boost/utility.hpp>

#include <sql.h>

#include <string>


namespace cxxdb {


class ConnectionHandle;


/*! \brief Wrapper around an ODBC statement handle. */
class StatementHandle : boost::noncopyable
{
public:

    StatementHandle(
            ConnectionHandle& conn_handle
        );

    /*! \brief Calls SQLSetStmtAttr. */
    void setAttribute(
            SQLINTEGER attribute,
            SQLPOINTER value_p,
            SQLINTEGER value_length
        );

    /*! \brief Calls SQLBindCol. */
    void bindCol(
            SQLUSMALLINT column_number,
            SQLSMALLINT target_type,
            SQLPOINTER target_value,
            SQLLEN buffer_len,
            SQLLEN* strlen_or_ind_ptr
        );

    /*! \brief Calls SQLBindParameter. */
    void bindParameter(
            SQLUSMALLINT parameter_number,
            SQLSMALLINT input_output_type,
            SQLSMALLINT value_type,
            SQLSMALLINT parameter_type,
            SQLULEN column_size,
            SQLSMALLINT decimal_digits,
            SQLPOINTER parameter_value_ptr,
            SQLLEN buffer_length,
            SQLLEN* strlen_or_ind_ptr
        );

    /*! \brief Calls SQLExecDirect(). */
    void execDirect(
            const std::string& sql,
            bool *no_data_out = NULL
        );

    /*! \brief Calls SQLPrepare. */
    void prepare(
            const std::string& sql
        );


    /*! \brief Calls getNumParams. */
    SQLSMALLINT getNumParams();

    /*! \brief Calls SQLDescribeParam. */
    ParameterInfo getParameterInfo(
            SQLUSMALLINT param_no
        );

    /*! \brief Calls getParameterInfo for each parameter. */
    ParameterInfos getParameterInfos();

    /*! \brief Calls SQLNumResultCols. */
    SQLSMALLINT getNumCols();

    /*! \brief Calls SQLDescribeCol. */
    ColumnInfo getColumnInfo(
            SQLUSMALLINT col_no
        );

    /*! \brief Calls getColumnInfo for each column. */
    ColumnInfos getColumnInfos();

    /*! \brief Calls SQLExecute.
     *
     *  If no_data_out is not NULL,
     *  then sets no_data_out to true iff SQLExecute returns SQL_NO_DATA_FOUND.
     *  Otherwise, a return of SQL_NO_DATA_FOUND is ignored.
     *
     */
    void execute(
            bool *no_data_out = NULL
        );

    /*! \brief Calls SQLFetch. */
    bool fetch();

    /*! \brief Calls SQLFetchScroll. */
    bool fetchScroll(
            SQLSMALLINT orientation,
            SQLLEN offset
        );

    /*! \brief calls SQLRowCount. */
    SQLLEN rowCount();

    /*! \brief Close the cursor.
     *
     *  This function does nothing if there is no cursor on the statement.
     */
    void closeCursor();

    /*! \brief Calls SQLCancel. */
    void cancel();

    ~StatementHandle();


private:

    SQLHANDLE _hstmt;
};

} //namespace cxxdb

#endif
