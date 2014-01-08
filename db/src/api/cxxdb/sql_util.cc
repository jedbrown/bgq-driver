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

#include "cxxdb/sql_util.h"

#include <boost/lexical_cast.hpp>

#include <sqlext.h>

using boost::lexical_cast;

using std::string;

namespace cxxdb {

std::string sql_type_to_str( SQLSMALLINT data_type )
{
    switch( data_type ) {
    case SQL_BIGINT: return "BIGINT";
    case SQL_BINARY: return "BINARY";
    // DB2-only: case SQL_BLOB: return "BLOB";
    // DB2-only: case SQL_BLOCK_LOCATOR: return "BLOB_LOCATOR";
    // DB2-only: case SQL_BOOLEAN: return "BOOLEAN";
    case SQL_CHAR: return "CHAR";
    case SQL_TINYINT: return "TINYINT";
    case SQL_BIT: return "BIT";
    // DB2-only: case SQL_CLOB: return "CLOB";
    // DB2-only: case SQL_CLOB_LOCATOR: return "CLOB_LOCATOR";
    // DB2-only: case SQL_CURSORHANDLE: return "CURSORHANDLE";
    case SQL_TYPE_DATE: return "DATE";
    // DB2-only: case SQL_DBCLOB: return "DBCLOB";
    // DB2-only: case SQL_DBCLOB_LOCATOR: return "DBCLOB_LOCATOR";
    case SQL_DECIMAL: return "DECIMAL";
    // DB2-only: case SQL_DECFLOAT: return "DECFLOAT";
    case SQL_DOUBLE: return "DOUBLE";
    case SQL_FLOAT: return "FLOAT";
    // DB2-only: case SQL_GRAPHIC: return "GRAPHIC";
    case SQL_INTEGER: return "INTEGER";
    case SQL_LONGVARCHAR: return "LONGVARCHAR";
    case SQL_LONGVARBINARY: return "LONGVARBINARY";
    // DB2-only: case SQL_LONGVARGRAPHIC: return "LONGVARGRAPHIC";
    case SQL_WLONGVARCHAR: return "WLONGVARCHAR";
    case SQL_NUMERIC: return "NUMERIC";
    case SQL_REAL: return "REAL";
    // DB2-only: case SQL_ROW: return "ROW";
    case SQL_SMALLINT: return "SMALLINT";
    case SQL_TYPE_TIME: return "TIME";
    case SQL_TYPE_TIMESTAMP: return "TIMESTAMP";
    case SQL_VARBINARY: return "VARBINARY";
    case SQL_VARCHAR: return "VARCHAR";
    // DB2-only: case SQL_VARGRAPHIC: return "VARGRAPHIC";
    case SQL_WVARCHAR: return "WVARCHAR";
    case SQL_WCHAR: return "WCHAR";
    // DB2-only: case SQL_XML: return "XML";
    default: return (string() + "UNEXPECTED(" + lexical_cast<string>(data_type) + ")");
    }
}


} // namespace cxxdb
