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

#ifndef UTILCLI_H
#define UTILCLI_H

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#define MAX_UID_LENGTH 18
#define MAX_PWD_LENGTH 30
#define MAX_STMT_LEN 255
#define MAX_COLUMNS 255
#ifdef DB2WIN
#define MAX_TABLES 50
#else
#define MAX_TABLES 255
#endif

#define maxval(a,b) (a > b ? a : b)

// Macro for environment handle checking
#define ENV_HANDLE_CHECK(henv, cliRC)              \
if (cliRC != SQL_SUCCESS) {                        \
  BGQDB::HandleInfoPrint(SQL_HANDLE_ENV, henv,            \
                  cliRC, __LINE__, __FILE__);      \
  if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) return cliRC; \
}

#define ENV_HANDLE_CHECK2(henv, cliRC, errRC)      \
if (cliRC != SQL_SUCCESS) {                        \
  BGQDB::HandleInfoPrint(SQL_HANDLE_ENV, henv,            \
                  cliRC, __LINE__, __FILE__);      \
  if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) return errRC; \
}

// Macro for connection handle checking
#define DBC_HANDLE_CHECK(hdbc, cliRC)              \
if (cliRC != SQL_SUCCESS) {                        \
  BGQDB::HandleInfoPrint(SQL_HANDLE_DBC, hdbc,            \
                  cliRC, __LINE__, __FILE__);      \
  if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) return cliRC; \
}

// Macro for statement handle checking
#define STMT_HANDLE_CHECK(hstmt, hdbc, cliRC)      \
if (cliRC != SQL_SUCCESS) {                        \
  BGQDB::HandleInfoPrint(SQL_HANDLE_STMT, hstmt,   \
                  cliRC, __LINE__, __FILE__);      \
  if (cliRC == SQL_ERROR) BGQDB::StmtResourcesFree(hstmt); \
  if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) { \
      BGQDB::TransRollback(hdbc);                  \
      return cliRC;                                \
  }                                                \
}

// Macro for statement handle checking in apps with multiple connections
#define MC_STMT_HANDLE_CHECK(hstmt, henv, cliRC)   \
if (cliRC != SQL_SUCCESS) {                        \
  BGQDB::HandleInfoPrint(SQL_HANDLE_STMT, hstmt,   \
                  cliRC, __LINE__, __FILE__);      \
  if (cliRC == SQL_ERROR) BGQDB::StmtResourcesFree(hstmt); \
  if ((cliRC == SQL_INVALID_HANDLE) || (cliRC == SQL_ERROR)) { \
      BGQDB::MultiConnTransRollback(henv);         \
      return cliRC;                                \
  }                                                \
}

namespace BGQDB {

// Functions used in macros
void HandleInfoPrint(SQLSMALLINT, SQLHANDLE, SQLRETURN, int, const char *);
SQLRETURN StmtResourcesFree(SQLHANDLE);
void TransRollback(SQLHANDLE);
void MultiConnTransRollback(SQLHANDLE);

} // BGQDB

#endif

