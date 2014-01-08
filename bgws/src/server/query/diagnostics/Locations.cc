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

#include "Locations.hpp"

#include <string>


using std::string;


namespace bgws {
namespace query {
namespace diagnostics {


const string Locations::HARDWARE_STATUS_STRING_TO_ORD_CLAUSE =
        "WHEN 'success' THEN 0"
       " WHEN 'marginal' THEN 1"
       " WHEN 'unknown' THEN 2"
       " WHEN 'failed' THEN 3"
       " WHEN 'uninitialized' THEN 4"
    ;

const string Locations::HARDWARE_STATUS_ORD_TO_STRING_CLAUSE =
        "WHEN 0 THEN 'success'"
       " WHEN 1 THEN 'marginal'"
       " WHEN 2 THEN 'unknown'"
       " WHEN 3 THEN 'failed'"
       " WHEN 4 THEN 'uninitialized'"
    ;


Locations::Locations(
        const LocationsOptions& options
    ) :
        _options(options)
{
    // Nothing to do.
}


void Locations::execute(
        cxxdb::ConnectionPtr conn_ptr,
        uint64_t* all_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out
    )
{

    *all_count_out = _calcRowCount( conn_ptr );

    if ( *all_count_out == 0 ) {
        return;
    }

    _doQuery( conn_ptr, rs_ptr_out );
}


uint64_t Locations::_calcRowCount( cxxdb::ConnectionPtr conn_ptr )
{
    WhereClause loc_wc;
    WhereClause main_wc;
    cxxdb::ParameterNames param_names;

    _options.calcWhereClause( &loc_wc, &main_wc, &param_names );

    string sql = string() +

 "SELECT COUNT(*) AS rowCount"
  " FROM ( SELECT location, hardwareStatusOrd, hardwareReplaceOrd, lastrun"
           " FROM ( SELECT locrunid.location,"
                         " MAX( CASE res.hardwareStatus " + HARDWARE_STATUS_STRING_TO_ORD_CLAUSE + " END ) AS hardwareStatusOrd,"
                         " MAX( CASE res.hardwareReplace WHEN 'T' THEN 1 ELSE 0 END ) AS hardwareReplaceOrd,"
                         " MAX( endTime ) AS lastrun"
                     " FROM ( SELECT loclastrun.location, run.runId"
                             " FROM ( SELECT res.location, MAX(run.endTime) AS lastrun"
                                      " FROM bgqDiagResults AS res"
                                           " JOIN"
                                           " bgqDiagRuns AS run"
                                           " ON res.runid = run.runid"
                                        + loc_wc.getString() +
                                      " GROUP BY res.location"
                                  " ) AS loclastrun"
                                  " JOIN"
                                  " bgqDiagRuns AS run"
                                  " ON loclastrun.lastrun = run.endTime"
                         " ) AS locrunid"
                         " JOIN"
                         " bgqDiagResults AS res"
                         " ON locrunid.location = res.location AND locrunid.runid = res.runid"
                    " GROUP BY locrunid.location"
                " ) AS grouped" +
             main_wc.getString() +
       " ) AS groupedWithRowNum"

        ;

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

    cxxdb::Parameters &params(stmt_ptr->parameters());

    _options.bindParameters( params );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {
        return 0;
    }

    return rs_ptr->columns()["rowCount"].as<uint64_t>();
}


void Locations::_doQuery(
        cxxdb::ConnectionPtr conn_ptr,
        cxxdb::ResultSetPtr* rs_ptr_out
    )
{
    WhereClause loc_wc;
    WhereClause main_wc;
    cxxdb::ParameterNames param_names;

    _options.calcWhereClause( &loc_wc, &main_wc, &param_names );

    string sort_clause_sql(_options.calcSortClauseSql());


    string sql = string() +

 "SELECT RTRIM(location) AS location, lastrun,"
       " CASE hardwareStatusOrd " + HARDWARE_STATUS_ORD_TO_STRING_CLAUSE + " END AS hardwareStatus,"
       " CASE hardwareReplaceOrd WHEN 1 THEN 'T' ELSE 'F' END AS hardwareReplace"
  " FROM ( SELECT location, hardwareStatusOrd, hardwareReplaceOrd, lastrun,"
                " ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql + " ) AS rownum"
           " FROM ( SELECT locrunid.location,"
                         " MAX( CASE res.hardwareStatus " + HARDWARE_STATUS_STRING_TO_ORD_CLAUSE + " END ) AS hardwareStatusOrd,"
                         " MAX( CASE res.hardwareReplace WHEN 'T' THEN 1 ELSE 0 END ) AS hardwareReplaceOrd,"
                         " MAX( endTime ) AS lastrun"
                     " FROM ( SELECT loclastrun.location, run.runId"
                             " FROM ( SELECT res.location, MAX(run.endTime) AS lastrun"
                                      " FROM bgqDiagResults AS res"
                                           " JOIN"
                                           " bgqDiagRuns AS run"
                                           " ON res.runid = run.runid"
                                        + loc_wc.getString() +
                                      " GROUP BY res.location"
                                  " ) AS loclastrun"
                                  " JOIN"
                                  " bgqDiagRuns AS run"
                                  " ON loclastrun.lastrun = run.endTime"
                         " ) AS locrunid"
                         " JOIN"
                         " bgqDiagResults AS res"
                         " ON locrunid.location = res.location AND locrunid.runid = res.runid"
                    " GROUP BY locrunid.location"
                " ) AS grouped" +
             main_wc.getString() +
       " ) AS groupedWithRowNum"
   " WHERE rownum BETWEEN ? AND ?"
   " ORDER BY rownum"

        ;

    param_names.push_back( "row_start" );
    param_names.push_back( "row_end" );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

    cxxdb::Parameters &params(stmt_ptr->parameters());

    _options.bindParameters( params );
    _options.getRange().bindParameters( params, "row_start", "row_end" );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());
    rs_ptr->internalize( stmt_ptr );

    *rs_ptr_out = rs_ptr;
}


} } } // namespace bgws::query::diagnostics
