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

#include "cxxdb/Parameter.h"

#include "cxxdb/exceptions.h"
#include "cxxdb/sql_util.h"
#include "cxxdb/StatementHandle.h"

#include <utility/include/Log.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <sqlext.h>

#include <cmath>

using boost::lexical_cast;

using std::string;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

Parameter::Parameter(
        StatementHandle& stmt_handle,
        unsigned param_index,
        ParameterInfo& info
    ) :
        _stmt_handle(stmt_handle),
        _info(info),
        _bound(false)
{
    _indexes.push_back( param_index );

    LOG_DEBUG_MSG(
            "param " << param_index << ":"
            " data_type=" << sql_type_to_str( _info.data_type ) <<
            " size=" << _info.size <<
            " decimal_digits=" << _info.decimal_digits <<
            " nullable=" << _info.nullable
        );
}


unsigned Parameter::addIndex( unsigned param_index )
{
    unsigned ret(0);
    if ( _indexes.size() == 1 ) {
        ret = _indexes[0];
    }

    _indexes.push_back( param_index );

    return ret;
}


void Parameter::set(
        const std::string& s,
        bool* truncated_out
    )
{
    if ( ! (_info.data_type == SQL_CHAR ||
            _info.data_type == SQL_VARCHAR ||
            _info.data_type == SQL_TYPE_TIMESTAMP) ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(string) when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    // type is valid.

    if ( _info.data_type == SQL_TYPE_TIMESTAMP ) {
        if ( truncated_out )  *truncated_out = false;

        _set(
                boost::posix_time::time_from_string( s )
            );

        return;
    }

    // type is valid and is not TIMESTAMP.

    string::const_iterator end_i;
    SQLLEN strlen;

    if ( s.size() > _info.size ) {
        if ( ! truncated_out ) {
            CXXDB_THROW_EXCEPTION( ValueTooBig( string() + "String parameter too big for column." ) );
        }

        *truncated_out = true;
        end_i = s.begin() + _info.size;
        strlen = _info.size;
    } else {
        if ( truncated_out )  *truncated_out = false;
        end_i = s.end();
        strlen = s.size();
    }

    _bind();

    std::copy( s.begin(), end_i, _buffer.begin() );

    _strlen_or_null_ind = strlen;
}


void Parameter::set(
        const Bytes& bytes,
        bool* truncated_out
    )
{
    if ( ! (_info.data_type == SQL_BINARY || _info.data_type == SQL_VARBINARY) ){
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(double) when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    Bytes::const_iterator end_i;
    SQLLEN bytes_size;

    if ( bytes.size() > _info.size ) {
        if ( ! truncated_out ) {
            CXXDB_THROW_EXCEPTION( ValueTooBig( string() + "Binary parameter too big for column." ) );
        }

        *truncated_out = true;
        end_i = bytes.begin() + _info.size;
        bytes_size = _info.size;
    } else {
        if ( truncated_out )  *truncated_out = false;
        end_i = bytes.end();
        bytes_size = bytes.size();
    }

    _bind();

    std::copy( bytes.begin(), end_i, _buffer.begin() );
    _strlen_or_null_ind = bytes_size;
}


void Parameter::set( const boost::posix_time::ptime& t )
{
    if ( _info.data_type != SQL_TYPE_TIMESTAMP ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(int16_t) when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    _set( t );
}


void Parameter::set( char value )
{
    if ( ! (_info.data_type == SQL_CHAR && _info.size == 1) ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(char) when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    _bind();

    _buffer[0] = value;
    _strlen_or_null_ind = 1;
}


void Parameter::set( const int16_t value )
{
    if ( _info.data_type == SQL_SMALLINT ) {
        _bind();
        _strlen_or_null_ind = 0;
        _int16_buf = value;
        return;
    }

    if ( _setInt32( value ) )  return;
    if ( _setInt64( value ) )  return;
    if ( _setDouble( value ) )  return;

    // Wrong type!
    CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(int16_t) when column type is " + sql_type_to_str( _info.data_type ) ) );
}


void Parameter::set( const int32_t value )
{
    if ( _setInt32( value ) )  return;
    if ( _setInt64( value ) )  return;
    if ( _setDouble( value ) )  return;

    // Wrong type!
    CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(int32_t) when column type is " + sql_type_to_str( _info.data_type ) ) );
}


void Parameter::set( const int64_t value )
{
    if ( _setInt64( value ) )  return;
    if ( _setDouble( value ) )  return;

    // Wrong type!
    CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(int64_t) when column type is " + sql_type_to_str( _info.data_type ) ) );
}


void Parameter::set( double value )
{
    if ( _info.data_type != SQL_DOUBLE ) {
        // Wrong type!
        CXXDB_THROW_EXCEPTION( WrongType( string() + "Called " + __FUNCTION__ + "(double) when column type is " + sql_type_to_str( _info.data_type ) ) );
    }

    _bind();

    _double_buf = value;
    _strlen_or_null_ind = 0;
}


void Parameter::setNull()
{
    _bind();

    _strlen_or_null_ind = SQL_NULL_DATA;
}


void Parameter::_bind()
{
    if ( _bound ) {
        return;
    }

    for ( unsigned i(0) ; i < _indexes.size() ; ++i ) {
        unsigned param_index(_indexes[i]);

        if ( (_info.data_type == SQL_CHAR ||
              _info.data_type == SQL_VARCHAR) &&
             _info.size > 0
           )
        {
            _buffer.resize( _info.size + 1 );

            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_CHAR,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    _buffer.data(),
                    _buffer.size(),
                    &_strlen_or_null_ind
                );
        } else if ( _info.data_type == SQL_SMALLINT ) {
            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_SHORT,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    &_int16_buf,
                    0,
                    &_strlen_or_null_ind
                );
        } else if ( _info.data_type == SQL_INTEGER ) {
            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_LONG,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    &_int32_buf,
                    0,
                    &_strlen_or_null_ind
                );
        } else if ( _info.data_type == SQL_BIGINT ) {
            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_SBIGINT,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    &_int64_buf,
                    0,
                    &_strlen_or_null_ind
                );
        } else if ( _info.data_type == SQL_DOUBLE ) {
            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_DOUBLE,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    &_double_buf,
                    0,
                    &_strlen_or_null_ind
                );
        } else if ( _info.data_type == SQL_BINARY ||
                    _info.data_type == SQL_VARBINARY
                  )
        {
            _buffer.resize( _info.size );

            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_BINARY,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    _buffer.data(),
                    _buffer.size(),
                    &_strlen_or_null_ind
                );
        } else if ( _info.data_type == SQL_TYPE_TIMESTAMP ) {
            _stmt_handle.bindParameter(
                    param_index,
                    SQL_PARAM_INPUT,
                    SQL_C_TIMESTAMP,
                    _info.data_type,
                    _info.size,
                    _info.decimal_digits,
                    &_timestamp_buf,
                    0,
                    &_strlen_or_null_ind
                );
        } else {
            CXXDB_THROW_EXCEPTION( WrongType( string() +
                    "Unexpected type for parameter, the type at " + lexical_cast<string>(param_index) + " is " + sql_type_to_str( _info.data_type )
                ) );
        }
    }
    _bound = true;

    _strlen_or_null_ind = SQL_NULL_DATA;
}


bool Parameter::_setInt32( int32_t value )
{
    if ( _info.data_type != SQL_INTEGER ) {
        return false;
    }

    _bind();
    _int32_buf = value;
    _strlen_or_null_ind = 0;
    return true;
}


bool Parameter::_setInt64( int64_t value )
{
    if ( _info.data_type != SQL_BIGINT ) {
        return false;
    }

    _bind();
    _int64_buf = value;
    _strlen_or_null_ind = 0;
    return true;
}


bool Parameter::_setDouble( double value )
{
    if ( _info.data_type != SQL_DOUBLE ) {
        return false;
    }

    _bind();
    _double_buf = value;
    _strlen_or_null_ind = 0;
    return true;
}


void Parameter::_set( const boost::posix_time::ptime& t )
{
    _bind();

    if ( t.is_not_a_date_time() ) {
        _strlen_or_null_ind = SQL_NULL_DATA;
        return;
    }

    _timestamp_buf.year = t.date().year();
    _timestamp_buf.month = t.date().month();
    _timestamp_buf.day = t.date().day();
    _timestamp_buf.hour = t.time_of_day().hours();
    _timestamp_buf.minute = t.time_of_day().minutes();
    _timestamp_buf.second = t.time_of_day().seconds();
    _timestamp_buf.fraction = t.time_of_day().fractional_seconds() * 1000;

    _strlen_or_null_ind = 0;
}


} // namespace cxxdb
