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

#include "cxxdb/Column.h"

#include "cxxdb/exceptions.h"
#include "cxxdb/sql_util.h"
#include "cxxdb/StatementHandle.h"

#include <utility/include/Log.h>

#include <boost/algorithm/string.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <sqlext.h>

#include <stdexcept>
#include <string>

using std::logic_error;
using std::string;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

Column::Column(
        StatementHandle& stmt_handle,
        unsigned column_index,
        ColumnInfo& col_info
    ) :
        _info(col_info)
{
    LOG_DEBUG_MSG( "Initializing column with data_type=" << sql_type_to_str( _info.data_type ) << " size=" << _info.size );

    if ( ((_info.data_type == SQL_CHAR) ||
          (_info.data_type == SQL_VARCHAR)) &&
         _info.size > 0
       )
    {
        // Allocate a buffer that fits the column and then bind the col to the buffer.

        _buffer.resize( _info.size + 1 );

        stmt_handle.bindCol(
                column_index,
                SQL_C_CHAR,
                _buffer.data(),
                _buffer.size(),
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_SMALLINT ) {
        stmt_handle.bindCol(
                column_index,
                SQL_C_SHORT,
                &_int16_buf,
                0,
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_INTEGER ) {
        stmt_handle.bindCol(
                column_index,
                SQL_C_LONG,
                &_int_buf,
                0,
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_BIGINT ) {
        stmt_handle.bindCol(
                column_index,
                SQL_C_SBIGINT,
                &_int64_buf,
                0,
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_DOUBLE ) {
        stmt_handle.bindCol(
                column_index,
                SQL_C_DOUBLE,
                &_double_buf,
                0,
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_BINARY || _info.data_type == SQL_VARBINARY ) {
        // Allocate a buffer that fits the column and then bind the col to the buffer.

        _buffer.resize( _info.size );

        stmt_handle.bindCol(
                column_index,
                SQL_C_BINARY,
                _buffer.data(),
                _buffer.size(),
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_TYPE_TIMESTAMP ) {
        stmt_handle.bindCol(
                column_index,
                SQL_C_TYPE_TIMESTAMP,
                &_timestamp_buf,
                0,
                &_strlen_or_null_ind
            );
    } else if ( _info.data_type == SQL_TYPE_DATE ) {
        stmt_handle.bindCol(
                column_index,
                SQL_C_TYPE_DATE,
                &_date_buf,
                0,
                &_strlen_or_null_ind
            );
    }
}


bool Column::isNull() const
{
    return (_strlen_or_null_ind == SQL_NULL_DATA);
}


std::string Column::getString() const
{
    if ( ! (_info.data_type == SQL_CHAR ||
            _info.data_type == SQL_VARCHAR ||
            _info.data_type == SQL_TYPE_DATE ||
            _info.data_type == SQL_TYPE_TIMESTAMP) )
    {
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    LOG_TRACE_MSG( "Getting string from column of type " << sql_type_to_str( _info.data_type ) << " with strlen_null_ind=" << _strlen_or_null_ind );

    CXXDB_COLUMN_CHECK_NULL;

    if ( _info.data_type == SQL_TYPE_TIMESTAMP ) {
        std::ostringstream oss;
        oss.imbue( std::locale( oss.getloc(), new boost::posix_time::time_facet( "%Y-%m-%d %H:%M:%S.%f" ) ) );
        oss << _getTimestamp();

        return oss.str();
    }

    if ( _info.data_type == SQL_TYPE_DATE ) {
        std::ostringstream oss;
        oss.imbue( std::locale( oss.getloc(), new boost::gregorian::date_facet( "%Y-%m-%d" ) ) );
        oss << _getDate();

        return oss.str();
    }

    // It's not a timestamp

    string ret( _buffer.begin(), _buffer.begin() + _strlen_or_null_ind );

    if ( _info.data_type == SQL_CHAR ) {
        boost::trim_right( ret );
    }

    return ret;
}


char Column::getChar() const
{
    if ( ! (_info.data_type == SQL_CHAR && _info.size == 1) ) {
        // Wrong type!
        if ( _info.data_type == SQL_CHAR ) {
            CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is CHAR(n)." ) );
        }

        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    CXXDB_COLUMN_CHECK_NULL;

    return _buffer[0];
}


int16_t Column::getInt16() const
{
    if ( _info.data_type != SQL_SMALLINT ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    CXXDB_COLUMN_CHECK_NULL;

    return _int16_buf;
}


int32_t Column::getInt32() const
{
    if ( _info.data_type == SQL_INTEGER ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int_buf;
    }

    if ( _info.data_type == SQL_SMALLINT ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int16_buf;
    }

    // Wrong type!
    CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );

    return _int_buf;
}


int64_t Column::getInt64() const
{
    if ( _info.data_type == SQL_BIGINT ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int64_buf;
    }

    if ( _info.data_type == SQL_SMALLINT ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int16_buf;
    }
    if ( _info.data_type == SQL_INTEGER ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int_buf;
    }

    // Wrong type!
    CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );

    return _int64_buf;
}


double Column::getDouble() const
{
    if ( _info.data_type == SQL_DOUBLE ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _double_buf;
    }

    if ( _info.data_type == SQL_BIGINT ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int64_buf;
    }
    if ( _info.data_type == SQL_SMALLINT ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int16_buf;
    }
    if ( _info.data_type == SQL_INTEGER ) {
        CXXDB_COLUMN_CHECK_NULL;
        return _int_buf;
    }

    // Wrong type!
    CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );

    return _double_buf;
}


Bytes Column::getBytes() const
{
    if ( ! (_info.data_type == SQL_BINARY || _info.data_type == SQL_VARBINARY) ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    CXXDB_COLUMN_CHECK_NULL;

    if ( _info.data_type == SQL_BINARY ) {
        return _buffer;
    }

    // It's VARBINARY, copy the part.
    Bytes ret( _buffer.begin(), _buffer.begin() + _strlen_or_null_ind );
    return ret;
}


boost::posix_time::ptime Column::getTimestamp() const
{
    if ( _info.data_type != SQL_TYPE_TIMESTAMP ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    CXXDB_COLUMN_CHECK_NULL;

    return _getTimestamp();
}


boost::gregorian::date Column::getDate() const
{
    if ( _info.data_type != SQL_TYPE_DATE ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    CXXDB_COLUMN_CHECK_NULL;

    return _getDate();
}


boost::gregorian::date Column::_getDate() const
{
    return boost::gregorian::date( _date_buf.year, _date_buf.month, _date_buf.day );
}


boost::posix_time::ptime Column::_getTimestamp() const
{
    boost::posix_time::ptime ret(
            boost::gregorian::date( _timestamp_buf.year, _timestamp_buf.month, _timestamp_buf.day ),
            boost::posix_time::time_duration( _timestamp_buf.hour, _timestamp_buf.minute, _timestamp_buf.second ) + boost::posix_time::microsec( _timestamp_buf.fraction / 1000 )
        );

    return ret;
}


} // namespace cxxdb
