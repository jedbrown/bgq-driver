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

#include "tableapi/DBObj.h"

#include "cxxdb/Connection.h"
#include "cxxdb/QueryStatement.h"
#include "cxxdb/UpdateStatement.h"

#include <sstream>

using std::ostringstream;
using std::string;

namespace BGQDB {

const std::string DBObj::calcInsertSql() const
{
    if ( _columns == 0 ) {
        return string();
    }

    ostringstream oss;
    oss << "INSERT INTO " << getTableName() << " ( " << calcColumnNamesCommaSeparated() << " ) VALUES ( ";

    bool first(true);

    for ( unsigned i(0) ; i < getColumnCount() ; ++i ) {
        if ( _columns & (1ULL << i) ) {
            if ( first ) { first = false; }
            else { oss << ", "; }
            oss << "? ";
        }
    }

    oss << ")";

    return oss.str();
}


const std::string DBObj::calcSelectSql( const std::string& where_clause ) const
{
    ostringstream oss;
    oss << "SELECT ";

    if ( _columns == 0 ) {
        oss << "COUNT(*)";
    } else {
        oss << calcColumnNamesCommaSeparated();
    }

    oss << " FROM " << getTableName() <<
            " " << where_clause;
    return oss.str();
}


const std::string DBObj::calcColumnNamesCommaSeparated() const
{
    ostringstream oss;

    ColumnNames col_names(calcColumnNames());

    for ( ColumnNames::const_iterator i(col_names.begin()) ; i != col_names.end() ; ++i ) {
        if ( i != col_names.begin() ) {
            oss << ", ";
        }
        oss << *i;
    }

    return oss.str();
}


cxxdb::UpdateStatementPtr DBObj::prepareInsert( cxxdb::Connection& conn ) const
{
    if ( _columns == 0 ) {
        return cxxdb::UpdateStatementPtr();
    }
    return conn.prepareUpdate( calcInsertSql(), calcColumnNames() );
}


cxxdb::QueryStatementPtr DBObj::prepareSelect(
        cxxdb::Connection& conn,
        const std::string& where_clause,
        const cxxdb::ParameterNames& param_names
    ) const
{
    return conn.prepareQuery(
            calcSelectSql( where_clause ),
            param_names
        );
}

} // BGQDB
