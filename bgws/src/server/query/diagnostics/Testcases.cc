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


#include "Testcases.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <string>


using std::string;


namespace bgws {
namespace query {
namespace diagnostics {


void Testcases::execute(
        const TestcasesOptions& options,
        cxxdb::ConnectionPtr conn_ptr,
        cxxdb::ResultSetPtr *rs_ptr_out
    )
{
    WhereClause where_clause;
    cxxdb::ParameterNames param_names;

    options.block_id.addTo( where_clause, param_names, BGQDB::DBTDiagtests::BLOCKID_COL );
    options.hardware_status.addTo( where_clause, param_names, BGQDB::DBTDiagtests::HARDWARESTATUS_COL );
    options.location.addTo( where_clause, param_names, BGQDB::DBTDiagresults::LOCATION_COL );
    options.testcase.addTo( where_clause, param_names, BGQDB::DBTDiagtests::TESTCASE_COL );

    if ( options.run_id_option.getOpt() ) {
        where_clause.add( "runId = ?" );
        param_names.push_back( "runId" );
    }

    bool only_location(options.location.hasValue() && (! options.block_id.hasValue()) && (! options.hardware_status.hasValue()) && (! options.run_id_option.getOpt()) && (! options.testcase.hasValue()));


    string sql;

    if ( only_location ) {

        sql =

 "WITH mr AS ("
  " SELECT testcase, MAX(endTime) AS et"
    " FROM bgqDiagResults"
    " WHERE location = ?"
    " GROUP BY testcase"
" )"
" SELECT dr.runId, dr.blockId, dr.testcase, dr.location, dr.serialnumber, dr.endTime, dr.hardwareStatus, dr.hardwareReplace, dr.logfile, dr.analysis"
  " FROM bgqDiagResults AS dr"
       " JOIN"
       " mr"
       " ON dr.testcase = mr.testcase AND dr.endTime = mr.et"
  " " + where_clause.getString() +
  " ORDER BY testcase"

            ;

        param_names.insert( param_names.begin(), "location" );

    } else {

        sql = string() +

 "SELECT runId, blockId, testcase, location, serialnumber, endTime, hardwareStatus, hardwareReplace, logfile, analysis"
  " FROM bgqDiagResults"
  " " + where_clause.getString() +
  " ORDER BY runId, blockId, testcase, location"

            ;

    }

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

    cxxdb::Parameters &params(stmt_ptr->parameters());

    options.block_id.bindParameters( params );
    options.hardware_status.bindParameters( params );
    options.location.bindParameters( params );
    options.testcase.bindParameters( params );

    if ( options.run_id_option.getOpt() ) {
        DiagnosticsRunIdOption::RunIdOpt run_id_opt(options.run_id_option.getOpt());
        params["runId"].cast( *run_id_opt );
    }

    *rs_ptr_out = stmt_ptr->execute();
    (*rs_ptr_out)->internalize( stmt_ptr );
}


} } } // namespace bgws::query::diagnostics
