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

#include "Query.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../dbConnectionPool.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace query {
namespace env {


Query::Query(
        boost::shared_ptr<AbstractOptions> abstract_options_ptr
    ) :
        _options_ptr(abstract_options_ptr)
{
    // Nothing to do.
}


void Query::execute(
        cxxdb::ConnectionPtr conn_ptr,
        uint64_t* row_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out
    )
{
    // first need to get the row count...

    *row_count_out = _queryRowCount( conn_ptr );

    if ( *row_count_out == 0 ) {
        return;
    }

    *rs_ptr_out = _queryRows( conn_ptr );
}


void Query::executeAsync(
        BlockingOperationsThreadPool& blocking_operations_thread_pool,
        ExecuteCbFn cb_fn
    )
{
    blocking_operations_thread_pool.post( boost::bind( &Query::_executeAsyncImpl, this, cb_fn ) );
}


void Query::cancel()
{
    cxxdb::QueryStatementPtr stmt_ptr(_stmt_ptr);

    if ( ! stmt_ptr )  return;

    stmt_ptr->cancel();
}


uint64_t Query::_queryRowCount( cxxdb::ConnectionPtr conn_ptr )
{
    string where_clause_sql;
    cxxdb::ParameterNames parameter_names;

    _options_ptr->calcWhereClauseSql(
            true,
            &where_clause_sql,
            &parameter_names
        );


    string key_cols_sql = "location";
    string key_cols_cmp = "env.location = mrt.location";

    if ( _options_ptr->getExtraKeyColumnName() != string() ) {
        key_cols_sql += ", " + _options_ptr->getExtraKeyColumnName();
        key_cols_cmp += " AND env." + _options_ptr->getExtraKeyColumnName() + " = mrt." + _options_ptr->getExtraKeyColumnName();
    }

    string sql;

    sql +=

 "WITH"
        ;

    if ( _options_ptr->isMostRecent() ) {
        sql +=
" mrt AS ("

" SELECT " + key_cols_sql + ", MAX(time) AS mrt"
  " FROM " + _options_ptr->getTableName() +
  " WHERE time > CURRENT_TIMESTAMP - 1 DAY"
  " GROUP BY " + key_cols_sql +

" ),"
      ;

    }

    sql +=

" ar AS ("

" SELECT env.*"
  " FROM " + _options_ptr->getTableName() + " AS env"
      ;

    if ( _options_ptr->isMostRecent() ) {
        sql +=

       " JOIN"
       " mrt"
       " ON " + key_cols_cmp + " AND env.time = mrt.mrt"
           ;
    }

    sql +=

  where_clause_sql +

" )"

" SELECT COUNT(*) AS c FROM ar"
      ;

    LOG_DEBUG_MSG( "Preparing " << sql );

    _stmt_ptr = conn_ptr->createQuery();

    _stmt_ptr->prepare( sql, parameter_names );

    _options_ptr->bindParameters( true, _stmt_ptr );

    cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());

    if ( rs_ptr->fetch() ) {
        uint64_t ret(rs_ptr->columns()["c"].as<uint64_t>());
        _stmt_ptr.reset();
        return ret;
    }

    _stmt_ptr.reset();
    return 0;
}


cxxdb::ResultSetPtr Query::_queryRows( cxxdb::ConnectionPtr conn_ptr )
{
    string where_clause_sql;
    cxxdb::ParameterNames parameter_names;

    _options_ptr->calcWhereClauseSql(
            false,
            &where_clause_sql,
            &parameter_names
        );


    string sort_clause_sql( _options_ptr->calcSortClauseSql() );


    string key_cols_sql = "location";
    string key_cols_cmp = "env.location = mrt.location";

    if ( _options_ptr->getExtraKeyColumnName() != string() ) {
        key_cols_sql += ", " + _options_ptr->getExtraKeyColumnName();
        key_cols_cmp += " AND env." + _options_ptr->getExtraKeyColumnName() + " = mrt." + _options_ptr->getExtraKeyColumnName();
    }


    string sql;

    sql +=

 "WITH"
        ;

    if ( _options_ptr->isMostRecent() ) {
        sql +=
" mrt AS ("

" SELECT " + key_cols_sql + ", MAX(time) AS mrt"
  " FROM " + _options_ptr->getTableName() +
  " WHERE time > CURRENT_TIMESTAMP - 1 DAY"
  " GROUP BY " + key_cols_sql +

" ),"
      ;

    }

    sql +=

" ar AS ("

" SELECT env.*,"
      " ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql + " ) AS row_num"
  " FROM " + _options_ptr->getTableName() + " AS env"
      ;

    if ( _options_ptr->isMostRecent() ) {
        sql +=

       " JOIN"
       " mrt"
       " ON " + key_cols_cmp + " AND env.time = mrt.mrt"
           ;
    }

    sql +=

  where_clause_sql +

" )"

  " SELECT * FROM ar"
    " WHERE ar.row_num BETWEEN ? AND ?" // rangeStart, rangeEnd
    " ORDER BY ar.row_num"

      ;


    _stmt_ptr = conn_ptr->createQuery();

    LOG_DEBUG_MSG( "Preparing " << sql );
    _stmt_ptr->prepare( sql, parameter_names );

    _options_ptr->bindParameters( false, _stmt_ptr );

    LOG_DEBUG_MSG( "Executing" );
    cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());
    LOG_DEBUG_MSG( "Complete" );

    rs_ptr->internalize( _stmt_ptr );
    _stmt_ptr.reset();
    return rs_ptr;
}


void Query::_executeAsyncImpl(
        ExecuteCbFn cb_fn
    )
{
    Result res;

    try {

        res.connection_ptr = dbConnectionPool::getConnection();

        execute(
                res.connection_ptr,
                &res.all_count,
                &res.rs_ptr
            );

    } catch ( std::exception& e ) {

        res.exc_ptr = std::current_exception();

    }

    cb_fn( res );
}


} } } // namespace bgws::query::env
