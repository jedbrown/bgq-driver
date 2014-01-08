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

#include "cxxdb/exceptions.h"

#include <sstream>

#include <execinfo.h>
#include <sqlext.h>

using std::ostringstream;
using std::runtime_error;
using std::string;

namespace cxxdb {

std::string gatherStack()
{
    void *trace_elems[20];

    int trace_elem_count(backtrace( trace_elems, 20 ));

    char **stack_syms(backtrace_symbols( trace_elems, trace_elem_count ));

    ostringstream oss;
    oss << "\n";
    for ( int i(1) ; i < trace_elem_count ; ++i ) { // skip the first one because it's this function.
        oss << stack_syms[i] << "\n";
    }

    free( stack_syms );

    return oss.str();
}


static string generateMessage(
        const std::string& sql_function_name,
        SQLRETURN sqlrc,
        const DiagnosticRecords& diagnostic_records
    )
{
    // Look here: http://publib.boulder.ibm.com/infocenter/db2luw/v9r7/topic/com.ibm.db2.luw.apdv.cli.doc/doc/r0006874.html

    ostringstream oss;

    bool diagnostics_expected(false);

    if ( sqlrc == SQL_SUCCESS ) {
        oss << sql_function_name << " returned SQL_SUCCESS, should not have gotten here";
    } else if ( sqlrc == SQL_SUCCESS_WITH_INFO  ) {
        oss << sql_function_name << " returned SQL_SUCCESS_WITH_INFO, should not have gotten here.\n";
        diagnostics_expected = true;
    } else if ( sqlrc == SQL_STILL_EXECUTING ) {
        oss << sql_function_name << " returned SQL_STILL_EXECUTING, should not get this result code";
    } else if ( sqlrc == SQL_NO_DATA_FOUND ) {
        oss << sql_function_name << " returned SQL_NO_DATA_FOUND";
        diagnostics_expected = true;
    } else if ( sqlrc == SQL_NEED_DATA  ) {
        oss << sql_function_name << " returned SQL_NEED_DATA";
    } else if ( sqlrc == SQL_ERROR ) {
        oss << "error result from " << sql_function_name;
        diagnostics_expected = true;
    } else if ( sqlrc == SQL_INVALID_HANDLE ) {
        oss << sql_function_name << " returned invalid handle. The program is not correct.";
    } else {
        oss << sql_function_name << " returned unexpected return code " << sqlrc;
        if ( ! diagnostic_records.empty() ) {
            oss << "\n";
            formatDiagnosticRecords( diagnostic_records, oss );
        }
    }

    if ( diagnostics_expected ) {
        if ( diagnostic_records.empty() ) {
            oss << " (no diagnostic records available)";
        } else {
            oss << ", diagnostic records follow:\n";
            formatDiagnosticRecords( diagnostic_records, oss );
        }
    }

    return oss.str();
}


//---------------------------------------------------------------------
// class DatabaseException

DatabaseException::DatabaseException(
        const std::string& sql_function_name,
        SQLRETURN sqlrc,
        const DiagnosticRecords& diagnostic_records
    ) :
        runtime_error( generateMessage( sql_function_name, sqlrc, diagnostic_records ) ),
        _sql_function_name(sql_function_name),
        _sqlrc(sqlrc),
        _diagnostic_records(diagnostic_records)

{
    // Nothing to do.
}


//---------------------------------------------------------------------
// class InvalidObject

InvalidObjectException::InvalidObjectException(
        const std::string& object_type,
        const std::string& function_name
    ) :
        std::logic_error( "Called " + function_name + " on invalid " + object_type + ". There is an error in the program." )
{
    // Nothing to do.
}


//---------------------------------------------------------------------
// class ValueTooBig

ValueTooBig::ValueTooBig(
        const std::string& msg
    ) :
        std::range_error( msg )
{
    // Nothing to do.
}


//---------------------------------------------------------------------
// class WrongType

WrongType::WrongType(
        const std::string& msg
    ) :
        std::logic_error( msg )
{
    // Nothing to do.
}


} // namespace cxxdb
