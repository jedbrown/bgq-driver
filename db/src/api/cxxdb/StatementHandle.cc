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


#include "cxxdb/StatementHandle.h"

#include "cxxdb/ConnectionHandle.h"
#include "cxxdb/diagnostics.h"

#include <utility/include/Log.h>

#include <sqlext.h>

#include <string>


using std::string;


LOG_DECLARE_FILE( "database" );


namespace cxxdb {


StatementHandle::StatementHandle(
        ConnectionHandle& conn_handle
    )
{
    SQLRETURN rc = SQLAllocHandle(
            SQL_HANDLE_STMT,
            conn_handle.getSqlHandle(),
            &_hstmt
        );

    if ( rc == SQL_SUCCESS ) {
        LOG_DEBUG_MSG( "Created database statement handle " << _hstmt );
        return;
    }

    checkResult( "SQLAllocHandle(STMT)", rc, SQL_HANDLE_STMT, _hstmt );
}


void StatementHandle::setAttribute(
        SQLINTEGER attribute,
        SQLPOINTER value_p,
        SQLINTEGER value_length
    )
{
    LOG_DEBUG_MSG( "Setting attribute " << attribute << " on " << _hstmt );

    SQLRETURN rc = SQLSetStmtAttr(
            _hstmt,
            attribute,
            value_p,
            value_length
        );

    checkResult( "SQLSetStmtAttr", rc, SQL_HANDLE_STMT, _hstmt );
}


void StatementHandle::bindCol(
        SQLUSMALLINT column_number,
        SQLSMALLINT target_type,
        SQLPOINTER target_value,
        SQLLEN buffer_len,
        SQLLEN* strlen_or_ind_ptr
    )
{
    LOG_TRACE_MSG( "Binding column " << column_number << " on " << _hstmt );

    SQLRETURN rc = SQLBindCol(
            _hstmt,
            column_number,
            target_type,
            target_value,
            buffer_len,
            strlen_or_ind_ptr
        );

    checkResult( "SQLBindCol", rc, SQL_HANDLE_STMT, _hstmt );
}


void StatementHandle::bindParameter(
        SQLUSMALLINT parameter_number,
        SQLSMALLINT input_output_type,
        SQLSMALLINT value_type,
        SQLSMALLINT parameter_type,
        SQLULEN column_size,
        SQLSMALLINT decimal_digits,
        SQLPOINTER parameter_value_ptr,
        SQLLEN buffer_length,
        SQLLEN* strlen_or_ind_ptr
    )
{
    LOG_TRACE_MSG( "Binding parameter " << parameter_number << " on " << _hstmt );

    SQLRETURN rc = SQLBindParameter(
            _hstmt,
            parameter_number,
            input_output_type,
            value_type,
            parameter_type,
            column_size,
            decimal_digits,
            parameter_value_ptr,
            buffer_length,
            strlen_or_ind_ptr
        );

    checkResult( "SQLBindParameter", rc, SQL_HANDLE_STMT, _hstmt );
}


void StatementHandle::execDirect(
        const std::string& sql,
        bool *no_data_out
    )
{
    if ( no_data_out )  *no_data_out = false;

    LOG_DEBUG_MSG( "Executing '" << sql << "' on " << _hstmt );

    SQLRETURN rc = SQLExecDirect(
            _hstmt,
            (SQLCHAR*)(sql.c_str()),
            SQL_NTS
        );

    if ( rc == SQL_NO_DATA_FOUND ) {
        if ( no_data_out )  *no_data_out = true;
        return;
    }

    checkResult( "SQLExecDirect", rc, SQL_HANDLE_STMT, _hstmt );
}


void StatementHandle::prepare(
        const std::string& sql
    )
{
    LOG_DEBUG_MSG( "Preparing '" << sql << "' on " << _hstmt );

    SQLRETURN rc = SQLPrepare(
            _hstmt,
            (SQLCHAR*)(sql.c_str()),
            SQL_NTS
        );

    checkResult( "SQLExecDirect", rc, SQL_HANDLE_STMT, _hstmt );
}


SQLSMALLINT StatementHandle::getNumParams()
{
    SQLSMALLINT parameter_count;

    SQLRETURN rc = SQLNumParams(
            _hstmt,
            &parameter_count
        );

    checkResult( "SQLNumParams", rc, SQL_HANDLE_STMT, _hstmt );

    return parameter_count;
}


ParameterInfo StatementHandle::getParameterInfo(
        SQLUSMALLINT param_no
    )
{
    ParameterInfo ret;

    SQLSMALLINT nullable;

    SQLRETURN rc = SQLDescribeParam(
            _hstmt,
            param_no,
            &ret.data_type,
            &ret.size,
            &ret.decimal_digits,
            &nullable
        );

    checkResult( "SQLDescribeParam", rc, SQL_HANDLE_STMT, _hstmt );

    ret.nullable = (nullable == SQL_NULLABLE || nullable == SQL_NULLABLE_UNKNOWN);

    return ret;
}


ParameterInfos StatementHandle::getParameterInfos()
{
    ParameterInfos ret;

    SQLSMALLINT num_params(getNumParams());

    for ( int i(1) ; i <= num_params ; ++i ) {
        ret.push_back( getParameterInfo( i ) );
    }

    return ret;
}


SQLSMALLINT StatementHandle::getNumCols()
{
    SQLSMALLINT col_count;

    SQLRETURN rc = SQLNumResultCols(
            _hstmt,
            &col_count
        );

    checkResult( "SQLNumResultCols", rc, SQL_HANDLE_STMT, _hstmt );

    return col_count;
}


ColumnInfo StatementHandle::getColumnInfo(
        SQLUSMALLINT col_no
    )
{
    ColumnInfo ret;

    SQLCHAR col_name_buf[SQL_MAX_COLUMN_NAME_LEN + 1];
    SQLSMALLINT name_len;

    SQLSMALLINT nullable;

    SQLRETURN rc = SQLDescribeCol(
            _hstmt,
            col_no,
            col_name_buf,
            sizeof ( col_name_buf ),
            &name_len,
            &ret.data_type,
            &ret.size,
            &ret.decimal_digits,
            &nullable
        );

    checkResult( "SQLDescribeCol", rc, SQL_HANDLE_STMT, _hstmt );

    ret.name = string( reinterpret_cast<char*>(col_name_buf), name_len );
    ret.nullable = (nullable == SQL_NULLABLE);

    return ret;
}


ColumnInfos StatementHandle::getColumnInfos()
{
    ColumnInfos ret;

    SQLSMALLINT num_cols(getNumCols());

    for ( SQLSMALLINT i(1) ; i <= num_cols ; ++i ) {
        ret.push_back( getColumnInfo( i ) );
    }

    return ret;
}


void StatementHandle::execute(
        bool *no_data_out
    )
{
    if ( no_data_out )  *no_data_out = false;

    LOG_DEBUG_MSG( "Executing on " << _hstmt );

    SQLRETURN rc = SQLExecute( _hstmt );

    if ( rc == SQL_NO_DATA_FOUND ) {
        if ( no_data_out )  *no_data_out = true;
        return;
    }

    checkResult( "SQLExecute", rc, SQL_HANDLE_STMT, _hstmt );
}


bool StatementHandle::fetch()
{
    LOG_DEBUG_MSG( "Fetch on " << _hstmt );

    SQLRETURN rc = SQLFetch( _hstmt );

    if ( rc == SQL_NO_DATA_FOUND ) {
        return false;
    }

    checkResult( "SQLFetch", rc, SQL_HANDLE_STMT, _hstmt );

    return true;
}


bool StatementHandle::fetchScroll(
        SQLSMALLINT orientation,
        SQLLEN offset
    )
{
    LOG_TRACE_MSG( "FetchScroll on " << _hstmt );

    SQLRETURN rc = SQLFetchScroll(
            _hstmt,
            orientation,
            offset
        );

    if ( rc == SQL_NO_DATA_FOUND ) {
        return false;
    }

    checkResult( "SQLFetchScroll", rc, SQL_HANDLE_STMT, _hstmt );

    return true;
}


SQLLEN StatementHandle::rowCount()
{
    LOG_TRACE_MSG( "rowCount on " << _hstmt );

    SQLLEN row_count;

    SQLRETURN rc = SQLRowCount( _hstmt, &row_count );

    checkResult( "SQLRowCount", rc, SQL_HANDLE_STMT, _hstmt );

    return row_count;
}


void StatementHandle::closeCursor()
{
    LOG_TRACE_MSG( "closeCursor on " << _hstmt );

    SQLRETURN rc = SQLFreeStmt( _hstmt, SQL_CLOSE );

    checkResult( "SQLFreeStmt", rc, SQL_HANDLE_STMT, _hstmt );
}


void StatementHandle::cancel()
{
    LOG_DEBUG_MSG( "canceling " << _hstmt );

    SQLRETURN rc = SQLCancel( _hstmt );

    checkResult( "SQLCancel", rc, SQL_HANDLE_STMT, _hstmt );
}


StatementHandle::~StatementHandle()
{
    LOG_DEBUG_MSG( "Destroying database statement handle " << _hstmt );

    SQLRETURN rc = SQLFreeHandle( SQL_HANDLE_STMT, _hstmt );
    _hstmt = SQL_NULL_HSTMT;

    if ( rc == SQL_SUCCESS ) {
        return;
    }

    LOG_ERROR_MSG( "Failed to free database statement handle. rc=" << rc );
}


} // namespace cxxdb
