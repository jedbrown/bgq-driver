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

/* There must be a db.properties in the cwd to use.
 *
 * This program takes an optional argument client_count, the default is 1
 * <client_count> threads will be created.
 * Each thread will connect to the db and update the status of its block 500 times.
 * The blocks are named testblock_0, testblock_1, ... (one for each client_count)
 *
 * Prime the database using:
 * db2 "INSERT INTO tbgpBlock ( blockId ) VALUES ('testblock_0' )"
 * db2 "INSERT INTO tbgpBlock ( blockId ) VALUES ('testblock_1' )"
 * ...
 */

#include <pthread.h>
#include <sqlcli.h>
#include <time.h>

#include <iostream>
#include <sstream>

#include "DbProperties.h"


using namespace std;


static char *s_program_name;

struct block_update_thread_args_t {
   SQLCHAR *db_name;
   SQLCHAR *db_user;
   SQLCHAR *db_pass;

   string block_id;

   unsigned int iterations_res;
   double time_res;
};

extern "C" void* block_update_thread_fn(
      void* arg_void );

static void report_sql_error(
      const char *function_name,
      SQLSMALLINT handleType,
      SQLHANDLE handle );

static void test_block_update( int client_count );


int main(
      int argc,
      char *argv[] )
{
   s_program_name = COPYRIGHT; // Use copyright to get rid of warning.
   s_program_name = argv[0];

   int client_count(1);

   if ( argc > 1 ) {
      istringstream iss(argv[1]);
      iss >> client_count;
      if ( ! iss ) {
         cerr << "Failed to get client count from '" << argv[1] << "'.\n";
         return 1;
      }
   }

   test_block_update( client_count );
} // main()


void* block_update_thread_fn(
      void* arg_void )
{
   block_update_thread_args_t *buta(reinterpret_cast<block_update_thread_args_t *>(arg_void));

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

   bool disconnect(false);
   if ( SQL_SUCCESS == sqlrc ) {
      sqlrc = SQLConnect( hdbc,
                          buta->db_name, SQL_NTS,
                          buta->db_user, SQL_NTS,
                          buta->db_pass, SQL_NTS );

      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLConnect", SQL_HANDLE_DBC, hdbc );
      } else {
         disconnect = true;
      }
   }

   SQLHSTMT hstmt(SQL_NULL_HANDLE);
   if ( SQL_SUCCESS == sqlrc ) {
      sqlrc = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &hstmt );
      if ( SQL_SUCCESS != sqlrc ) {
         hstmt = SQL_NULL_HANDLE;
         report_sql_error( "SQLAllocHandle update block status statement", SQL_HANDLE_DBC, hdbc );
      }
   }

   if ( SQL_SUCCESS == sqlrc ) {
      string sql_str(string("UPDATE tbgpBlock SET status = ? WHERE blockId = '") + buta->block_id + "'");
      SQLCHAR *sql = new SQLCHAR[sql_str.size() + 1];
      memcpy( sql, sql_str.c_str(), sql_str.size() + 1 );
      sqlrc = SQLPrepare( hstmt, sql, SQL_NTS );
      delete[] sql;
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLPrepare update block status", SQL_HANDLE_STMT, hstmt );
      }
   }

   SQLCHAR new_status_val[2] = "I";
   SQLLEN new_status_val_ind;

   if ( SQL_SUCCESS == sqlrc ) {
      sqlrc = SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT,
                                SQL_C_CHAR, SQL_CHAR, 1, 0,
                                new_status_val, sizeof ( new_status_val ) - 1,
                                &new_status_val_ind );
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLPrepare update block status", SQL_HANDLE_STMT, hstmt );
      }
   }

   char status_vals[] = "IF";

   // Warm-up
   for ( int i(0) ; (i < 10) && (SQL_SUCCESS == sqlrc); ++i ) {
      new_status_val[0] = status_vals[i % (sizeof ( status_vals )-1)];
      new_status_val_ind =  SQL_NTS;

      sqlrc = SQLExecute( hstmt );
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLExecute update block status to I", SQL_HANDLE_STMT, hstmt );
      }
   }

   // Get the start time before running the tests for real.
   struct timespec start_time;
   clock_gettime( CLOCK_REALTIME, &start_time );

   int total_iterations = 500;

   // Run the tests.
   for ( int i(0) ; (i < total_iterations) && (SQL_SUCCESS == sqlrc); ++i ) {
      new_status_val[0] = status_vals[i % (sizeof ( status_vals )-1)];
      new_status_val_ind =  SQL_NTS;

      sqlrc = SQLExecute( hstmt );
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLExecute update block status to I", SQL_HANDLE_STMT, hstmt );
      }
   }

   // Get the end time.
   struct timespec end_time;
   clock_gettime( CLOCK_REALTIME, &end_time );

   // Set the status to F.
   if ( SQL_SUCCESS == sqlrc ) {
      new_status_val[0] = 'F';
      new_status_val_ind =  SQL_NTS;

      sqlrc = SQLExecute( hstmt );
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLExecute update block status to F", SQL_HANDLE_STMT, hstmt );
      }
   }

   if ( SQL_NULL_HANDLE != hstmt ) {
      sqlrc = SQLFreeStmt( hstmt, SQL_DROP );
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLFreeStmt update block status statement", SQL_HANDLE_STMT, hstmt );
      }
   }

   if ( disconnect ) {
      cout << "Disconnecting from database.\n";
      sqlrc = SQLDisconnect( hdbc );
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLDisconnect", SQL_HANDLE_DBC, hdbc );
      }
   }

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

   buta->iterations_res = total_iterations;

   double start_time_dbl = double(start_time.tv_sec) + (double(start_time.tv_nsec) / 1000000000.0);
   double end_time_dbl = double(end_time.tv_sec) + (double(end_time.tv_nsec) / 1000000000.0);
   double total_time = end_time_dbl - start_time_dbl;
   buta->time_res = total_time;

   return 0;
} // block_update_thread_fn()

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

void test_block_update( int client_count )
{
   const string db_properties_fn("db.properties");
   const DbProperties db_properties(&db_properties_fn);

   if ( ! db_properties.isValid() ) {
      cerr << "Failed to read db properties.\n";
      return;
   }

   const string *db_name_p(db_properties.get( DbProperties::DATABASE_NAME_PROP_NAME ));
   if ( 0 == db_name_p ) {
      cerr << "Must have database name in the DB properties file.\n";
      return;
   }
   const string *db_user_p(db_properties.get( DbProperties::DATABASE_USER_PROP_NAME ));
   if ( 0 == db_user_p ) {
      cerr << "Must have database user in the DB properties file.\n";
      return;
   }
   const string *db_pass_p(db_properties.get( DbProperties::DATABASE_PASSWORD_PROP_NAME ));
   if ( 0 == db_pass_p ) {
      cerr << "Must have database password in the DB properties file.\n";
      return;
   }

   SQLCHAR *db_name(new SQLCHAR[db_name_p->size() + 1]);
   memcpy( db_name, db_name_p->c_str(), db_name_p->size() + 1 );
   SQLCHAR *db_user(new SQLCHAR[db_user_p->size() + 1]);
   memcpy( db_user, db_user_p->c_str(), db_user_p->size() + 1 );
   SQLCHAR *db_pass(new SQLCHAR[db_pass_p->size() + 1]);
   memcpy( db_pass, db_pass_p->c_str(), db_pass_p->size() + 1 );

   pthread_t *threads = new pthread_t[client_count];
   block_update_thread_args_t *thread_args = new block_update_thread_args_t[client_count];

   for ( int i(0) ; i < client_count ; ++i ) {
      thread_args[i].db_name = db_name;
      thread_args[i].db_user = db_user;
      thread_args[i].db_pass = db_pass;

      ostringstream oss;
      oss << "testblock_" << i;
      thread_args[i].block_id = oss.str();

      int create_rc(pthread_create( &(threads[i]), 0, &block_update_thread_fn, &(thread_args[i]) ));
      if ( create_rc != 0 ) {
         cerr << "Failed to create thread " << i << ". rc=" << create_rc << endl;
      }
   }

   unsigned int total_iterations(0);
   double total_time(0);

   for ( int i(0) ; i < client_count ; ++i ) {
      int join_rc(pthread_join( threads[i], 0 ));
      if ( join_rc != 0 ) {
         cerr << "Failed to join thread " << i << ". rc=" << join_rc << endl;
      }
      cout << "\nThread " << i << ":\n";
      cout << "  iterations: " << thread_args[i].iterations_res << endl;
      cout << "  time " << thread_args[i].time_res << endl;

      total_iterations += thread_args[i].iterations_res;
      total_time += thread_args[i].time_res;
   }

   cout << "\nTotals:\n";
   cout << "  Ran " << total_iterations << " iterations.\n";
   cout << "  Total time: " << total_time << endl;
   cout << "  Iterations per sec: " << (total_iterations / total_time) << endl;

   delete[] db_name;
   delete[] db_user;
   delete[] db_pass;
} // test_block_update()
