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


#include "TableInfos.h"

#include "../Configuration.h"
#include "../log_util.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <ctype.h>
#include <stdio.h>


using std::ostringstream;
using std::ostream;
using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {
namespace db2 {


//-------------------------------------------------------------------------
// Function to throw an exception with info from DB2 regarding the error.


static void throwSqlError(
    const char *function_name,
    SQLSMALLINT handleType,
    SQLHANDLE handle
  )
{
  SQLRETURN sqlrc(SQL_SUCCESS);

  SQLCHAR sqlstate[6] = "";
  SQLINTEGER sqlcode(-1);
  SQLCHAR message[200] = "";
  SQLSMALLINT length(0);

  ostringstream errmsg;

  errmsg << "when calling CLI function " << function_name << "\n";

  SQLSMALLINT record(1);

  while ( SQL_SUCCESS == sqlrc ) {
    sqlrc  = SQLGetDiagRec( handleType, handle, record, sqlstate, &sqlcode,
                            message, sizeof ( message ), &length );

    if ( SQL_SUCCESS == sqlrc ) {
      sqlstate[5] = '\0';

      errmsg << record << ":"
              " sqlstate=" << sqlstate << " sqlcode=" << sqlcode << " '" << message << "'\n";

      ++record;
    }
  }

  if ( SQL_NO_DATA != sqlrc ) {
    errmsg << "while handling an error from " << function_name
        << ", SQLGetDiagRec failed with rc =" << sqlrc;
  }

  DESTRUCTOR_THROW_RUNTIME_ERROR_EXCEPTION( errmsg.str() );
}


//-------------------------------------------------------------------------
// class sql_handle_closer_t -- closes a database handle on destruction

class sql_handle_closer_t
{
  public:
    sql_handle_closer_t(
        SQLSMALLINT handle_type, SQLHANDLE handle,
        SQLSMALLINT error_handle_type, SQLHANDLE error_handle
      ) :
        _handle_type(handle_type), _handle(handle),
        _error_handle_type(error_handle_type), _error_handle(error_handle)
    {}

    void close() {
      if ( SQL_NULL_HANDLE == _handle ) {
        return;
      }

      SQLRETURN sqlrc;

      if ( SQL_HANDLE_DBC == _handle_type ) {
        LOG_INFO_MSG( "disconnecting from database" );
        sqlrc = SQLDisconnect( _handle );
        if ( sqlrc != SQL_SUCCESS ) {
          throwSqlError( "SQLDisconnect", _error_handle_type, _error_handle );
        }
      }

      sqlrc = SQLFreeHandle( _handle_type, _handle );
      if ( sqlrc != SQL_SUCCESS ) {
        throwSqlError( "SQLFreeHandle", _error_handle_type, _error_handle );
      }
      _handle = SQL_NULL_HANDLE;
    }

    void set(
        SQLSMALLINT handle_type, SQLHANDLE handle,
        SQLSMALLINT error_handle_type, SQLHANDLE error_handle
      )
    {
      close();
      _handle_type = handle_type;
      _handle = handle;
      _error_handle_type = error_handle_type;
      _error_handle = error_handle;
    }

    ~sql_handle_closer_t() {
      close();
    }

  private:
    SQLSMALLINT _handle_type;
    SQLHANDLE _handle;
    SQLSMALLINT _error_handle_type;
    SQLHANDLE _error_handle;
};


//-------------------------------------------------------------------------
// Class TableInfos

ostream& operator<<( ostream& os, const TableInfo& ti )
{
  return (os << "{table_info:"
                  " tablespace_id=" << ti.tablespace_id <<
                  " table_id=" << ti.table_id << "}");
}


TableInfos::TableInfos( const Configuration& configuration )
{
    const string &db_name(configuration.get_database_name());
    Configuration::ConstStringPtr db_user_p(configuration.get_database_user());
    Configuration::ConstStringPtr db_schema_p(configuration.get_database_schema_name());

    char buf[L_cuserid + 1];

    string db_schema(db_schema_p ? *db_schema_p : db_user_p ? *db_user_p : cuserid( buf ) );

    LOG_INFO_MSG( "Using schema '" << db_schema << "'" );

    string db_schema_uc(db_schema);
    std::transform( db_schema.begin(), db_schema.end(), db_schema_uc.begin(), (int(*)( int )) toupper );

    SQLRETURN sqlrc(SQL_SUCCESS);

    SQLHANDLE henv(SQL_NULL_HANDLE);
    sqlrc = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv );
    if ( sqlrc != SQL_SUCCESS ) {
        throwSqlError( "SQLAllocHandle ENV", SQL_HANDLE_ENV, henv );
    }
    sql_handle_closer_t henv_closer( SQL_HANDLE_ENV, henv, SQL_HANDLE_ENV, henv );

    SQLHANDLE hdbc(SQL_NULL_HANDLE);
    sqlrc = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );
    if ( sqlrc != SQL_SUCCESS ) {
        throwSqlError( "SQLAllocHandle DBC", SQL_HANDLE_ENV, henv );
    }
    sql_handle_closer_t hdbc_closer( SQL_HANDLE_DBC, hdbc, SQL_HANDLE_ENV, henv );


    LOG_INFO_MSG( "Connecting to database " << db_name << "..." );

    while ( true ) {

        try {

            sqlrc = SQLConnect(
                    hdbc,
                    (SQLCHAR*) db_name.c_str(), SQL_NTS,
                    NULL, SQL_NTS,
                    NULL, SQL_NTS
                );

            if ( sqlrc != SQL_SUCCESS ) {
                throwSqlError( "SQLConnect", SQL_HANDLE_DBC, hdbc );
            }

            break;

        } catch ( std::exception& e ) {

            LOG_WARN_MSG( "Failed to connect to database, will try again in a few seconds." );

            sleep( 5 );

        }

    }

    LOG_INFO_MSG( "Connected to database" << (db_user_p ? string() + " as '" + *db_user_p + "'" : "" ) );

    static const string BLOCK_TABLE_NAME_UC( "TBGQBLOCK" );
    block_table_info = _s_getTableInfo( hdbc, BLOCK_TABLE_NAME_UC, db_schema_uc );

    static const string JOB_TABLE_NAME_UC( "TBGQJOB" );
    job_table_info = _s_getTableInfo( hdbc, JOB_TABLE_NAME_UC, db_schema_uc );

    static const string MIDPLANE_TABLE_NAME_UC( "TBGQMIDPLANE" );
    midplane_table_info = _s_getTableInfo( hdbc, MIDPLANE_TABLE_NAME_UC, db_schema_uc );

    static const string NODE_CARD_TABLE_NAME_UC( "TBGQNODECARD" );
    node_card_table_info = _s_getTableInfo( hdbc, NODE_CARD_TABLE_NAME_UC, db_schema_uc );

    static const string SWITCH_TABLE_NAME_UC( "TBGQSWITCH" );
    switch_table_info = _s_getTableInfo( hdbc, SWITCH_TABLE_NAME_UC, db_schema_uc );

    static const string CABLE_TABLE_NAME_UC( "TBGQCABLE" );
    cable_table_info = _s_getTableInfo( hdbc, CABLE_TABLE_NAME_UC, db_schema_uc );

    static const string EVENT_LOG_TABLE_NAME_UC( "TBGQEVENTLOG" );
    event_log_table_info = _s_getTableInfo( hdbc, EVENT_LOG_TABLE_NAME_UC, db_schema_uc );

    static const string NODE_TABLE_NAME_UC( "TBGQNODE" );
    node_table_info = _s_getTableInfo( hdbc, NODE_TABLE_NAME_UC, db_schema_uc );
}


TableInfos::table::Value TableInfos::calcTable( const TableInfo& table_info ) const
{
    if ( block_table_info == table_info ) {
        return table::BLOCK;
    }
    if ( job_table_info == table_info ) {
        return table::JOB;
    }
    if ( midplane_table_info == table_info ) {
        return table::MIDPLANE;
    }
    if ( node_card_table_info == table_info ) {
        return table::NODE_CARD;
    }
    if ( switch_table_info == table_info ) {
        return table::SWITCH;
    }
    if ( cable_table_info == table_info ) {
        return table::CABLE;
    }
    if ( event_log_table_info == table_info ) {
        return table::EVENT_LOG;
    }
    if ( node_table_info == table_info ) {
        return table::NODE;
    }
    return table::UNKNOWN;
}


TableInfo TableInfos::_s_getTableInfo(
        SQLHANDLE hdbc,
        const string& table_name_uc,
        const string& db_schema_uc
    )
{
    SQLHANDLE hstmt(SQL_NULL_HANDLE);

    TableInfo ret;

    SQLRETURN sqlrc;

    sqlrc = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &hstmt );

    if ( sqlrc != SQL_SUCCESS ) {
      throwSqlError( "SQLAllocHandle STMT", SQL_HANDLE_DBC, hdbc );
    }
    sql_handle_closer_t hstmt_handle_closer( SQL_HANDLE_STMT, hstmt, SQL_HANDLE_DBC, hdbc );

    ostringstream oss;
    oss <<
        "SELECT tbspaceId, tableId"
        " FROM SYSCAT.TABLES"
        " WHERE tabSchema='" << db_schema_uc << "' AND tabname = '" << table_name_uc << "'";

    const string &sql(oss.str());

    LOG_INFO_MSG( "Executing '" << sql << "'" );
    sqlrc = SQLExecDirect( hstmt, (SQLCHAR*) sql.c_str(), SQL_NTS );

    if ( sqlrc != SQL_SUCCESS ) {
      throwSqlError( "SQLExecDirect", SQL_HANDLE_STMT, hstmt );
    }

    SQLINTEGER tablespace_id_nullind;

    sqlrc = SQLBindCol(
        hstmt, 1, SQL_C_LONG,
        &ret.tablespace_id,
        0, &tablespace_id_nullind
      );

    if ( sqlrc != SQL_SUCCESS ) {
      throwSqlError( "SQLBindCol 1", SQL_HANDLE_STMT, hstmt );
    }

    SQLINTEGER table_id_nullind;

    sqlrc = SQLBindCol(
        hstmt, 2, SQL_C_LONG,
        &ret.table_id,
        0, &table_id_nullind
      );

    if ( sqlrc != SQL_SUCCESS ) {
      throwSqlError( "SQLBindCol 2", SQL_HANDLE_STMT, hstmt );
    }

    sqlrc = SQLFetch( hstmt );

    if ( sqlrc != SQL_SUCCESS ) {
      throwSqlError( "SQLFetch", SQL_HANDLE_STMT, hstmt );
    }

    if ( (SQL_NULL_DATA == tablespace_id_nullind) ||
         (SQL_NULL_DATA == table_id_nullind) ) {
      THROW_RUNTIME_ERROR_EXCEPTION( "don't know how to handle a NULL tablespace id or table id" );
    }

    LOG_INFO_MSG(
        table_name_uc << ": tablespace_id=" << ret.tablespace_id <<
        " table_id=" << ret.table_id
      );

    return ret;
}


} } } // namespace realtime::server::db2
