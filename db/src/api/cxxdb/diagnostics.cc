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

#include "cxxdb/diagnostics.h"

#include "cxxdb/exceptions.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <sqlext.h>

#include <iostream>
#include <sstream>

using boost::lexical_cast;

using std::ostringstream;
using std::string;

LOG_DECLARE_FILE( "database" );

namespace cxxdb {

DiagnosticRecord::DiagnosticRecord(
        const std::string& sqlstate_in,
        SQLINTEGER native_error_in,
        const std::string& message_in
    ) :
        sqlstate(sqlstate_in),
        native_error(native_error_in),
        message(message_in)
{
    // Nothing to do.
}

void checkResult(
        const std::string& sql_function_name,
        SQLRETURN rc,
        SQLSMALLINT handle_type,
        SQLHANDLE handle
    )
{
    if ( rc == SQL_SUCCESS ) {
        return;
    }

    if ( rc == SQL_SUCCESS_WITH_INFO ) {
        // Gather and log the info with a warning. No exception in this case.

        DiagnosticRecords diag_recs(
                gatherDiagnosticRecords( handle_type, handle )
            );

        ostringstream oss;
        oss << sql_function_name << " returned successful with info. Database diagnostic records follow:\n";
        formatDiagnosticRecords( diag_recs, oss );

        LOG_WARN_MSG( oss.str() );

        return;
    }

    if ( rc == SQL_ERROR ) {
        DiagnosticRecords diag_recs(
                gatherDiagnosticRecords( handle_type, handle )
            );

        CXXDB_THROW_EXCEPTION( DatabaseException( sql_function_name, rc, diag_recs ) );
    }

    if ( rc == SQL_INVALID_HANDLE ) {
        CXXDB_THROW_EXCEPTION( DatabaseException(
                sql_function_name,
                rc,
                DiagnosticRecords()
            ) );
    }

    CXXDB_THROW_EXCEPTION( DatabaseException(
            sql_function_name,
            rc,
            DiagnosticRecords()
        ) );
}


DiagnosticRecords gatherDiagnosticRecords(
        SQLSMALLINT handle_type,
        SQLHANDLE handle
    )
{
    DiagnosticRecords ret;

    SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER sqlcode;
    SQLCHAR message[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLSMALLINT length;

    SQLSMALLINT rec_i(1);

    while ( SQLGetDiagRec(
            handle_type,
            handle,
            rec_i,
            sqlstate,
            &sqlcode,
            message,
            sizeof ( message ),
            &length
        ) == SQL_SUCCESS )
    {
        DiagnosticRecord rec(
                (char*)(sqlstate),
                sqlcode,
                (char*)(message)
            );

        ret.push_back( rec );

        ++rec_i;
    }

    return ret;
}


void formatDiagnosticRecords(
        const DiagnosticRecords& recs,
        std::ostream& os
    )
{
    if ( recs.empty() ) {
        os << "No records.";
        return;
    }

    for ( DiagnosticRecords::const_iterator i(recs.begin()) ; i != recs.end() ; ++i ) {
        if ( i->message.empty() ) {
            os << "SQLSTATE=" << i->sqlstate << " SQLCODE=" << i->native_error << "\n";
        } else {
            os << i->message <<
                  "    SQLSTATE=" << i->sqlstate << " SQLCODE=" << i->native_error << "\n";
        }
    }
}


} // namespace cxxdb
