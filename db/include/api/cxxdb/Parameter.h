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

#ifndef CXXDB_PARAMETER_H_
#define CXXDB_PARAMETER_H_

#include "exceptions.h"
#include "fwd.h"
#include "ParameterInfo.h"
#include "sql_util.h"

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <sqlext.h>
#include <sqltypes.h>

#include <string>
#include <vector>

#include <stdint.h>

namespace cxxdb {

class StatementHandle;


/*! \brief A Parameter in a Statement.
 *
 *  For string types, use set(const std::string&,bool*).
 *
 *  For binary types (defined using FOR BIT DATA), use set(const Bytes&,bool*).
 *
 *  For timestamps, use set(const boost::posix::ptime&), (or set(const std::string&,bool*)).
 *
 *  For numeric types, use set(int16_t), set(int32_t), set(int64_t), set(double), or cast(T).
 *  The cast(T) method is more lenient and uses boost::numeric_cast to convert between
 *  numeric types, throwing an exception if the value doesn't fit in the target type.
 *  The set(*) functions are safe for the column type in the database,
 *  and will throw WrongType if the column type doesn't match the requested numeric type.
 *
 *  \section supportedTypes Supported types
 *
 *  <table>
 *  <tr><th> SQL type </th> <th> Methods </th> </tr>
 *  <tr><td> CHAR(n), VARCHAR(n), TIMESTAMP </td><td> set(const std::string&,bool*) </td></tr>
 *  <tr><td> CHAR(1) </td><td> set(char) </td></tr>
 *  <tr><td> CHAR(n) FOR BIT DATA, VARCHAR(n) FOR BIT DATA </td> <td> set(const Bytes&,bool*) </td></tr>
 *  <tr><td> TIMESTAMP </td><td> set(boost::posix_time::ptime) </td></tr>
 *  <tr><td> SMALLINT </td><td> set(int16_t), cast(T) </td></tr>
 *  <tr><td> INTEGER </td><td> set(int32_t), set(int16_t), cast(T) </td></tr>
 *  <tr><td> BIGINT</td><td> set(int64_t), set(int32_t), set(int16_t), cast(T) </td></tr>
 *  <tr><td> FLOAT / DOUBLE</td><td> set(double), set(int64_t), set(int32_t), set(int16_t), cast(T) </td></tr>
 *  </table>
 *
 *  <b>Note:</b> If a set(*) method is called for a column of the wrong type, then WrongType is thrown.
 *
 *  <b>Note:</b> cast(T) will throw ValueTooBig if the value doesn't fit in the result type.
 *
 */
class Parameter : boost::noncopyable
{
public:

    typedef boost::shared_ptr<Parameter> Ptr;
    typedef std::vector<unsigned> Indexes;

    /*! \brief Don't call this. */
    Parameter(
            StatementHandle& stmt_handle,
            unsigned param_index,
            ParameterInfo& info
        );

    /*! \brief Set a CHAR(n), VARCHAR(n), or TIMESTAMP field.
     *
     *  If <var>s</var> is too long for the parameter and truncated_out is NULL
     *  then ValueTooBig is thrown. If truncated_out is not NULL,
     *  then *truncated_out is set to whether it was truncated or not.
     *
     */
    void set(
            const std::string& s,
            bool* truncated_out = NULL
        );

    /*! \brief Set a CHAR(n) FOR BIT DATA or VARCHAR(n) FOR BIT DATA field.
     *
     *  If <var>bytes</var> is too long for the parameter and truncated_out is NULL
     *  then ValueTooBig is thrown. If truncated_out is not NULL,
     *  then *truncated_out is set to whether it was truncated or not.
     *
     */
    void set(
            const Bytes& bytes,
            bool* truncated_out = NULL
        );

    void set(
            const boost::posix_time::ptime& t
        );

    void set( char value );
    void set( int16_t value );
    void set( int32_t value );
    void set( int64_t value );
    void set( double value );

    template <typename T>
    void cast( T value )
    {
        try {
            switch ( _info.data_type ) {
            case SQL_SMALLINT:
                _int16_buf = boost::numeric_cast<int16_t>( value );
                break;
            case SQL_INTEGER:
                _int32_buf = boost::numeric_cast<int32_t>( value );
                break;
            case SQL_BIGINT:
                _int64_buf = boost::numeric_cast<int64_t>( value );
                break;
            case SQL_DOUBLE:
                _double_buf = boost::numeric_cast<double>( value );
                break;
            default:
                CXXDB_THROW_EXCEPTION( WrongType( std::string() +
                        "called cast when column " + boost::lexical_cast<std::string>(_indexes[0]) + " type is " + sql_type_to_str( _info.data_type )
                    ) );
                break;
            }
        } catch ( std::bad_cast& e ) {
            CXXDB_THROW_EXCEPTION( ValueTooBig( std::string() +
                    "parameter with value too big for column " + boost::lexical_cast<std::string>(_indexes[0]) + " of type " + sql_type_to_str( _info.data_type ) +
                    ". The value is " + boost::lexical_cast<std::string>( value ) + "."
                ) );
        }

        _bind();
        _strlen_or_null_ind = 0;
    }

    /*! \brief Set the value to NULL.
     *
     *  The parameter is NULL until one of the other set(*) or cast(T) methods is called.
     */
    void setNull();

    const ParameterInfo& getInfo() const  { return _info; }
    const Indexes& getIndexes() const  { return _indexes; }

    /*! \brief Don't call this. */
    unsigned addIndex( unsigned param_index );


private:

    StatementHandle& _stmt_handle;
    Indexes _indexes;
    ParameterInfo _info;

    bool _bound;
    Bytes _buffer;
    int16_t _int16_buf;
    int32_t _int32_buf;
    int64_t _int64_buf;
    double _double_buf;
    TIMESTAMP_STRUCT _timestamp_buf;
    SQLLEN _strlen_or_null_ind;

    void _bind();

    bool _setInt32( int32_t value );
    bool _setInt64( int64_t value );
    bool _setDouble( double value );

    void _set( const boost::posix_time::ptime& t );

};


} // namespace cxxdb


#endif
