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

#ifndef CXXDB_DIAGNOTISCS_H_
#define CXXDB_DIAGNOTISCS_H_

#include <sql.h>

#include <iosfwd>
#include <string>
#include <vector>

namespace cxxdb {

struct DiagnosticRecord
{
    std::string sqlstate;
    SQLINTEGER native_error;
    std::string message;

    DiagnosticRecord(
            const std::string& sqlstate_in,
            SQLINTEGER native_error_in,
            const std::string& message_in
        );
};

typedef std::vector<DiagnosticRecord> DiagnosticRecords;


/*! \brief Throws SQLException if the return code indicates failure.
 *
 *  Action depends on rc:
 *  - SQL_SUCCESS, does nothing.
 *  - SQL_SUCCESS_WITH_INFO, just logs diagnostic records.
 *  - SQL_ERROR, throws DatabaseException with the diagnostic records.
 *  - otherwise, throws DatabaseException with the result code and empty diagnostic records.
 */
void checkResult(
        const std::string& sql_function_name,
        SQLRETURN rc,
        SQLSMALLINT handle_type,
        SQLHANDLE handle
    );


/*! \brief Calls SQLGetDiagRec. */
DiagnosticRecords gatherDiagnosticRecords(
        SQLSMALLINT handle_type,
        SQLHANDLE handle
    );


void formatDiagnosticRecords(
        const DiagnosticRecords& recs,
        std::ostream& os
    );


} // namespace cxxdb


#endif
