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

#ifndef CXXDB_COLUMN_H_
#define CXXDB_COLUMN_H_

#include "ColumnInfo.h"
#include "exceptions.h"
#include "fwd.h"
#include "sql_util.h"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <sqlext.h>
#include <sqltypes.h>

#include <stdexcept>
#include <string>

#include <stdint.h>


#define CXXDB_COLUMN_CHECK_NULL \
    if ( _strlen_or_null_ind == SQL_NULL_DATA ) { \
        CXXDB_THROW_EXCEPTION( std::logic_error( std::string() + __FUNCTION__ + " called on column '" + _info.name + "' when value is NULL" ) ); \
    }


namespace cxxdb {

class StatementHandle;


/*! \brief A Column in a result.
 *
 *  For string types, use getString().
 *
 *  For binary types (defined using FOR BIT DATA), use getBytes().
 *
 *  For TIMESTAMP, use getTimestamp() (or getString()).
 *
 *  For numeric types, use getInt16(), getInt32(), getInt64(), getDouble(), or as<T>().
 *  The as<T>() method is more lenient and uses boost::numeric_cast to convert between
 *  numeric types, throwing an exception if the value doesn't fit in the target type.
 *  The get* functions are safe for the column type in the database,
 *  and will throw WrongType if the column type doesn't match the requested numeric type.
 *
 *  \section supportedTypes Supported types
 *
 *  <table>
 *  <tr><th>SQL type</th> <th> Methods </th> </tr>
 *  <tr><td>CHAR(n), VARCHAR(n), DATE, TIMESTAMP</td><td>getString()</td></tr>
 *  <tr><td>DATE</td><td>getDate()</td></tr>
 *  <tr><td>TIMESTAMP</td><td>getTimestamp()</td></tr>
 *  <tr><td>CHAR(1)</td><td>getChar()</td></tr>
 *  <tr><td>CHAR(n) FOR BIT DATA, VARCHAR(n) FOR BIT DATA</td><td>getBytes()</td></tr>
 *  <tr><td>SMALLINT</td><td>getInt16(), getInt32(), getInt64(), getDouble(), as<T>()</td></tr>
 *  <tr><td>INTEGER</td><td>getInt32(), getInt64(), getDouble(), as<T>()</td></tr>
 *  <tr><td>BIGINT</td><td>getInt64(), getDouble(), as<T>()</td></tr>
 *  <tr><td>FLOAT / DOUBLE</td><td>getDouble(), as<T>()</td></tr>
 *  </table>
 *
 *  <b>Note:</b> If a get* method is called for a column of the wrong type, then WrongType is thrown.
 *
 *  <b>Note:</b> as<T>() will throw ValueTooBig if the value doesn't fit in the result type.
 *
 *  \section Null
 *
 *  If the column is NULL and one of the get* functions is called, std::logic_error is thrown.
 *  Use isNull() to check if a column is NULL, or test the column as a boolean.
 *
 */
class Column : boost::noncopyable
{
public:
    typedef boost::shared_ptr<Column> Ptr;


    const ColumnInfo& getInfo() const  { return _info; }

    bool isNull() const;

    std::string getString() const;
    char getChar() const;
    int16_t getInt16() const;
    int32_t getInt32() const;
    int64_t getInt64() const;
    double getDouble() const;
    Bytes getBytes() const;
    boost::posix_time::ptime getTimestamp() const;
    boost::gregorian::date getDate() const;

    template <typename T>
    T as() const
    {
        try {
            switch ( _info.data_type ) {
            case SQL_SMALLINT:
                CXXDB_COLUMN_CHECK_NULL;
                return boost::numeric_cast<T>( _int16_buf );
            case SQL_INTEGER:
                CXXDB_COLUMN_CHECK_NULL;
                return boost::numeric_cast<T>( _int_buf );
            case SQL_BIGINT:
                CXXDB_COLUMN_CHECK_NULL;
                return boost::numeric_cast<T>( _int64_buf );
            case SQL_DOUBLE:
                CXXDB_COLUMN_CHECK_NULL;
                return boost::numeric_cast<T>( _double_buf );
            }
        } catch ( std::bad_cast& e ) {
            CXXDB_THROW_EXCEPTION( ValueTooBig( std::string() +
                    "column " + _info.name + " of type " + sql_type_to_str( _info.data_type ) + " contains value too big for receiver."
                ) );
        }

        CXXDB_THROW_EXCEPTION( WrongType( std::string() + "Called " + __FUNCTION__ + " when column type is " + sql_type_to_str( _info.data_type ) ) );
        return T(0);
    }


    typedef const int16_t Column::*_BoolType; // To support safe bool idiom.


    /*! \brief Boolean comparison.
     *  \return true if not NULL, false if is NULL.
     */
    operator _BoolType() const  { return (isNull() ? 0 : &Column::_int16_buf ); }


private:

    ColumnInfo _info;
    Bytes _buffer;
    int16_t _int16_buf;
    int32_t _int_buf;
    int64_t _int64_buf;
    double _double_buf;
    TIMESTAMP_STRUCT _timestamp_buf;
    DATE_STRUCT _date_buf;
    SQLLEN _strlen_or_null_ind;


    Column(
            StatementHandle& stmt_handle,
            unsigned column_index,
            ColumnInfo& col_info
        );


    boost::gregorian::date _getDate() const;

    boost::posix_time::ptime _getTimestamp() const;

    friend class Columns;
};


} // namespace cxxdb

#endif
