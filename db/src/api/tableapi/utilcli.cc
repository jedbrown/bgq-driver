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

#include "tableapi/utilcli.h"

#include <utility/include/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LOG_DECLARE_FILE( "database" );

void
HandleDiagnosticsPrint(
        SQLSMALLINT htype, // handle type identifier
        SQLHANDLE hndl     // handle
)
{
    SQLCHAR message[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER sqlcode;
    SQLSMALLINT length, i;

    i = 1;

    // Get multiple field settings of diagnostic record
    while (SQLGetDiagRec(
                htype,
                hndl,
                i,
                sqlstate,
                &sqlcode,
                message,
                SQL_MAX_MESSAGE_LENGTH + 1,
                &length) == SQL_SUCCESS)
    {
        LOG_ERROR_MSG("  SQLSTATE          = " << sqlstate );
        LOG_ERROR_MSG("  Native Error Code = " << sqlcode );

        // strip extra carriage return in message if present
        const size_t length( strlen(reinterpret_cast<char*>(message)) );
        if ( message[length-1] == '\n' ) {
            message[length-1] = '\0';
        }
        LOG_ERROR_MSG(message);
        i++;
    }

    LOG_ERROR_MSG("-------------------------");
} // HandleDiagnosticsPrint

void
HandleLocationPrint(
        SQLRETURN cliRC,
        int line,
        const char *file
)
{
    LOG_ERROR_MSG("  cliRC = " << cliRC );
    LOG_ERROR_MSG("  file  = " << file << "(" << line << ")" );
} // HandleLocationPrint

namespace BGQDB {

// Logs messages from CLI functions
void
HandleInfoPrint(
        SQLSMALLINT htype,         // handle type identifier
        SQLHANDLE hndl,            // handle used by the CLI function
        SQLRETURN cliRC,           // return code of the CLI function
        int line,
        const char *file
)
{
    switch (cliRC)
    {
    case SQL_SUCCESS:
        break;
    case SQL_INVALID_HANDLE:
        LOG_ERROR_MSG("-CLI INVALID HANDLE-----");
        HandleLocationPrint(cliRC, line, file);
        break;
    case SQL_ERROR:
        LOG_ERROR_MSG("--CLI ERROR--------------");
        HandleLocationPrint(cliRC, line, file);
        HandleDiagnosticsPrint(htype, hndl);
        break;
    case SQL_SUCCESS_WITH_INFO:
        break;
    case SQL_STILL_EXECUTING:
        break;
    case SQL_NEED_DATA:
        break;
    case SQL_NO_DATA_FOUND:
        break;
    default:
        LOG_ERROR_MSG("--default----------------");
        HandleLocationPrint(cliRC, line, file);
        break;
    }

} // HandleInfoPrint

// Free statement handles and print unexpected occurrences
SQLRETURN
StmtResourcesFree(
        SQLHANDLE hstmt
)
{
    SQLRETURN cliRC;

    // Unbind the statement handle
    // SQLFreeStmt can return the following:
    // SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_ERROR or SQL_INVALID_HANDLE
    cliRC = SQLFreeStmt(hstmt, SQL_UNBIND);
    HandleInfoPrint(SQL_HANDLE_STMT, hstmt, cliRC, __LINE__, __FILE__);
    if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) {
        return cliRC;
    }

    // Reset statement handle parameters
    cliRC = SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
    HandleInfoPrint(SQL_HANDLE_STMT, hstmt, cliRC, __LINE__, __FILE__);
    if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) {
        return cliRC;
    }

    // Close the statement handle
    cliRC = SQLFreeStmt(hstmt, SQL_CLOSE);
    HandleInfoPrint(SQL_HANDLE_STMT, hstmt, cliRC, __LINE__, __FILE__);
    if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) {
        return cliRC;
    }

    return SQL_SUCCESS;
} // StmtResourcesFree

// rollback transactions on a single connection
void
TransRollback(
        SQLHANDLE hdbc
)
{
    LOG_TRACE_MSG("  Rolling back the transaction ...");

    SQLRETURN cliRC;
    // End transactions on the connection
    cliRC = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_ROLLBACK);
    HandleInfoPrint(SQL_HANDLE_DBC, hdbc, cliRC, __LINE__, __FILE__);
    if (cliRC == SQL_SUCCESS) {
        LOG_TRACE_MSG( "The transaction rolled back." );
    }
} // TransRollback

// rollback transactions on multiple connections
void
MultiConnTransRollback(
        SQLHANDLE henv
)
{
    LOG_TRACE_MSG("  Rolling back the transactions...");

    SQLRETURN cliRC;
    // End transactions on the connection
    cliRC = SQLEndTran(SQL_HANDLE_ENV, henv, SQL_ROLLBACK);
    HandleInfoPrint(SQL_HANDLE_ENV, henv, cliRC, __LINE__, __FILE__);
    if (cliRC == SQL_SUCCESS) {
        LOG_TRACE_MSG("  The transactions are rolled back.");
    }
} // MultiConnTransRollback

} // BGQDB
