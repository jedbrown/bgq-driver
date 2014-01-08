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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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


#ifndef SQL_UTIL_H
#define SQL_UTIL_H

#include <sqlcli.h>

#include <string>


void setProgramName( const std::string& program_name );

int connect_to_db( SQLHANDLE* henv_out, SQLHDBC* hdbc_out );

void disconnect_from_db( SQLHANDLE henv, SQLHDBC hdbc );

void report_sql_error(
      const char *function_name,
      SQLSMALLINT handleType,
      SQLHANDLE handle );

#endif
