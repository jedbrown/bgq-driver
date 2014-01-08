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

#include "Ras.hpp"

#include <utility/include/Log.h>

#include <boost/assign.hpp>


using namespace boost::assign;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {


Ras::Ras( const RasOptions& options ) :
        _options(options)
{
    // Nothing to do.
}


void Ras::execute(
        cxxdb::ConnectionPtr conn_ptr,
        uint64_t* row_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out
    )
{
    string where_clause_sql;
    cxxdb::ParameterNames parameter_names;

    _options.calcWhereClauseSql(
            &where_clause_sql,
            &parameter_names
        );


    *row_count_out = _queryRowCount(
            where_clause_sql,
            parameter_names,
            conn_ptr
        );

    if ( *row_count_out == 0 ) {
        return;
    }


    string sort_clause_sql(_options.calcSortClauseSql());

    string sql =

 "WITH all_ordered AS ("

" SELECT ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql + " ) AS row_num,"
       " recid, msg_id, category, component, severity, event_time, jobid, block, location, serialnumber, count, ctlAction, message"
  " FROM bgqEventLog AS el" +
  where_clause_sql +

" )"

" SELECT recid, msg_id, category, component, severity, event_time, jobid, block, location, serialnumber, count, ctlAction, message"
  " FROM all_ordered"
  " WHERE row_num BETWEEN ? AND ?" // rowNumStart, rowNumEnd
  " ORDER BY row_num"

        ;

    parameter_names += "rowNumStart";
    parameter_names += "rowNumEnd";

    LOG_DEBUG_MSG( "Preparing\n" << sql );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            sql,
            parameter_names
        ));


    _options.bindParameters( stmt_ptr );

    _options.getRange().bindParameters( stmt_ptr->parameters(), "rowNumStart", "rowNumEnd" );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());


    rs_ptr->internalize( stmt_ptr );
    *rs_ptr_out = rs_ptr;
}


uint64_t Ras::_queryRowCount(
        const std::string& where_clause_sql,
        const cxxdb::ParameterNames& parameter_names,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    // Build SQL statement and parameters to count rows...

    string sql =

"SELECT COUNT(*) AS c"
" FROM bgqEventLog" +
where_clause_sql

        ;


    LOG_DEBUG_MSG( "Preparing\n" << sql );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery(
            sql,
            parameter_names
        ));

    _options.bindParameters( stmt_ptr );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( rs_ptr->fetch() ) {
        return rs_ptr->columns()["c"].as<int64_t>();
    }

    return 0;
}


} } // namespace bgws::query
