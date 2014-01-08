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

#include "cxxdb/Columns.h"

#include "cxxdb/exceptions.h"
#include "cxxdb/sql_util.h"
#include "cxxdb/StatementHandle.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string.hpp>

#include <sqlext.h>

#include <stdexcept>
#include <string>

using boost::lexical_cast;

using std::logic_error;
using std::string;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

Columns::Columns(
        StatementHandle& statement_handle
    )
{
    ColumnInfos col_infos(statement_handle.getColumnInfos());

    _col_ptrs.resize( col_infos.size() );

    for ( unsigned i(0) ; i < col_infos.size() ; ++i ) {
        ColumnInfo &col_info(col_infos[i]);

        LOG_TRACE_MSG(
                "col " << i+1 << ": "
                " name=" << col_info.name <<
                " type=" << sql_type_to_str( col_info.data_type ) <<
                " size=" << col_info.size <<
                " nullable=" << col_info.nullable
            );

        Column::Ptr col_ptr( new Column(
                statement_handle,
                i+1,
                col_info
            ) );

        _col_ptrs[i] = col_ptr;
        _name_to_col[boost::to_upper_copy(col_info.name)] = col_ptr;
    }
}


const Column& Columns::operator[]( unsigned column_index ) const
{
    if ( (column_index == 0) || (column_index-1 >= _col_ptrs.size()) ) {
        CXXDB_THROW_EXCEPTION( logic_error( string() + "No column " + lexical_cast<string>(column_index) + " in statement." ) );
    }

    return *(_col_ptrs[column_index-1]);
}


const Column& Columns::operator[]( const std::string& column_name ) const
{
    _NameToCol::const_iterator i(_name_to_col.find( boost::to_upper_copy(column_name) ));
    if ( i == _name_to_col.end() ) {
        CXXDB_THROW_EXCEPTION( logic_error( string() + "No column '" + column_name + "' in statement." ) );
    }

    return *(i->second);
}


unsigned Columns::columnNameToIndex( const std::string& name ) const
{
    const string name_norm(boost::to_upper_copy(name));

    for ( unsigned i(0) ; i < _col_ptrs.size() ; ++i ) {
        if ( _col_ptrs[i]->getInfo().name == name_norm ) {
            return (i+1);
        }
    }

    CXXDB_THROW_EXCEPTION( logic_error( string() + "No column '" + name + "' in statement." ) );
    return 0;
}

} // namespace cxxdb
