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


#include "sql_util.h"

#include "DbProperties.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>


using namespace std;


namespace {
    string s_program_name("not set");
}


void setProgramName( const string& program_name )
{
    s_program_name = program_name;
} // setProgramName()


int connect_to_db( SQLHANDLE* henv_out, SQLHDBC* hdbc_out )
{
    const string db_properties_fn(getenv( "DB_PROPERTY" ));
    const DbProperties db_properties(&db_properties_fn);

    if ( ! db_properties.isValid() ) {
        cerr << "Failed to read db properties.\n";
        return -1;
    }

    const string *db_name_p(db_properties.get( DbProperties::DATABASE_NAME_PROP_NAME ));
    if ( 0 == db_name_p ) {
        cerr << "Must have database name in the DB properties file.\n";
        return -1;
    }
    const string *db_user_p(db_properties.get( DbProperties::DATABASE_USER_PROP_NAME ));
    if ( 0 == db_user_p ) {
        cerr << "Must have database user in the DB properties file.\n";
        return -1;
    }
    const string *db_pass_p(db_properties.get( DbProperties::DATABASE_PASSWORD_PROP_NAME ));
    if ( 0 == db_pass_p ) {
        cerr << "Must have database password in the DB properties file.\n";
        return -1;
    }

    SQLCHAR *db_name(new SQLCHAR[db_name_p->size() + 1]);
    memcpy( db_name, db_name_p->c_str(), db_name_p->size() + 1 );
    SQLCHAR *db_user(new SQLCHAR[db_user_p->size() + 1]);
    memcpy( db_user, db_user_p->c_str(), db_user_p->size() + 1 );
    SQLCHAR *db_pass(new SQLCHAR[db_pass_p->size() + 1]);
    memcpy( db_pass, db_pass_p->c_str(), db_pass_p->size() + 1 );

    SQLRETURN sqlrc(SQL_SUCCESS);

    SQLHANDLE henv(SQL_NULL_HANDLE);

    sqlrc = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv );
    if ( sqlrc != SQL_SUCCESS ) {
        henv = SQL_NULL_HANDLE;
        report_sql_error( "SQLAllocHandle ENV", SQL_HANDLE_ENV, henv );
    }

    SQLHANDLE hdbc(SQL_NULL_HANDLE);
    if ( SQL_SUCCESS == sqlrc ) {
        sqlrc = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );
        if ( sqlrc != SQL_SUCCESS ) {
            hdbc = SQL_NULL_HANDLE;
            report_sql_error( "SQLAllocHandle DBC", SQL_HANDLE_ENV, henv );
        }
    }

    if ( SQL_SUCCESS == sqlrc ) {
        cout << "Connecting to database '" << db_name << "' as '" << db_user << "'...\n";

        sqlrc = SQLConnect( hdbc,
                            db_name, SQL_NTS,
                            db_user, SQL_NTS,
                            db_pass, SQL_NTS );

        if ( sqlrc != SQL_SUCCESS ) {
            report_sql_error( "SQLConnect", SQL_HANDLE_DBC, hdbc );
        } else {
            cout << "Connected to database.\n";
        }
    }

    if ( sqlrc != SQL_SUCCESS ) {
        // There was an error so clean up.
        if ( SQL_NULL_HANDLE != hdbc ) {
            sqlrc = SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
            if ( sqlrc != SQL_SUCCESS ) {
                report_sql_error( "SQLFreeHandle DBC", SQL_HANDLE_DBC, hdbc );
            }
        }

        if ( SQL_NULL_HANDLE != henv ) {
            SQLFreeHandle( SQL_HANDLE_ENV, henv );
            if ( sqlrc != SQL_SUCCESS ) {
                report_sql_error( "SQLFreeHandle ENV", SQL_HANDLE_ENV, henv );
            }
        }
        return -1;
    }

    *henv_out = henv;
    *hdbc_out = hdbc;

    return 0;
} // connect_to_db()

void disconnect_from_db( SQLHANDLE henv, SQLHDBC hdbc )
{
    SQLRETURN sqlrc;

    cout << "Disconnecting from database.\n";
    sqlrc = SQLDisconnect( hdbc );
    if ( sqlrc != SQL_SUCCESS ) {
        report_sql_error( "SQLDisconnect", SQL_HANDLE_DBC, hdbc );
    }

    sqlrc = SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
    if ( sqlrc != SQL_SUCCESS ) {
        report_sql_error( "SQLFreeHandle DBC", SQL_HANDLE_DBC, hdbc );
    }

    SQLFreeHandle( SQL_HANDLE_ENV, henv );
    if ( sqlrc != SQL_SUCCESS ) {
        report_sql_error( "SQLFreeHandle ENV", SQL_HANDLE_ENV, henv );
    }
} // disconnect_from_db()


void report_sql_error(
        const char *function_name,
        SQLSMALLINT handleType,
        SQLHANDLE handle )
{
    SQLRETURN sqlrc(SQL_SUCCESS);
    SQLSMALLINT record(1);

    SQLCHAR sqlstate[6] = "";
    SQLINTEGER sqlcode(-1);
    SQLCHAR message[200] = "";
    SQLSMALLINT length(0);

    while ( SQL_SUCCESS == sqlrc ) {
        sqlrc  = SQLGetDiagRec( handleType, handle, record, sqlstate, &sqlcode,
                                message, sizeof ( message ), &length );

        if ( SQL_SUCCESS == sqlrc ) {
            sqlstate[5] = '\0';

            cerr << s_program_name << ": " << function_name << " " << record << ": "
                    << sqlstate << " " << sqlcode << " " << message << endl;

            ++record;
        }
    }

    if ( SQL_NO_DATA != sqlrc ) {
        cerr << s_program_name << ": While handling an error from " << function_name
                << ", SQLGetDiagRec failed with rc =" << sqlrc << endl;
    }
} // report_sql_error()
