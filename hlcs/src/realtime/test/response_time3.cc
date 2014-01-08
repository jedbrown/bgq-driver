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

/* Test performance of multiple realtime clients connected to the server.
 *
 * Clients will connect to the realtime server, each in their own thread.
 * One thread will be doing updates in a loop. After every update, it will wait until all the
 * realtime clients have received it.
 *
 * Had some problems with response_time2.cc getting a ECONNRESET when connecting so will try to
 * do rt_inits in the main thread.
 *
 */

/* The realtime server must be running.
 *
 * Set DB_PROPERTY to the name of the db.properties file to use.
 *
 * There must be a db.properties in the cwd to use.
 *
 * Prime the database using:
 * db2 "INSERT INTO tbgpBlock ( blockId ) VALUES ('testblock_0' )"
 */


#include <pthread.h>
#include <sqlcli.h>
#include <time.h>
#include <rt_api.h>

#include <iostream>
#include <sstream>
#include <vector>

#include "DbProperties.h"


using namespace std;


static char *s_program_name;


class SharedState
{
public:
   SharedState( string block_id, int client_count )
      : _block_id(block_id),
        _client_count(client_count),
        _counter(0),
        _quit(false)
   {
      pthread_mutex_init( &_mtx, 0 );
      pthread_cond_init( &_cond, 0 );
   }

   ~SharedState()
   {
      pthread_mutex_destroy( &_mtx );
      pthread_cond_destroy( &_cond );
   }

   const string& getBlockId() const { return _block_id; }

   int getClientCount() const { return _client_count; }

   void incrCounter() {
      pthread_mutex_lock( &_mtx );
      ++_counter;
      if ( _counter == _client_count ) {
         pthread_cond_broadcast( &_cond );
      }
      pthread_mutex_unlock( &_mtx );
   }

   void waitForCounter() {
      pthread_mutex_lock( &_mtx );
      while ( _counter != _client_count ) {
         pthread_cond_wait( &_cond, &_mtx );
      }
      pthread_mutex_unlock( &_mtx );
   }

   void resetCounter() {
      pthread_mutex_lock( &_mtx );
      _counter = 0;
      pthread_mutex_unlock( &_mtx );
   }

   void setQuit() {
      pthread_mutex_lock( &_mtx );
      _quit = true;
      pthread_mutex_unlock( &_mtx );
   }

   bool checkQuit() {
      pthread_mutex_lock( &_mtx );
      bool ret(_quit);
      pthread_mutex_unlock( &_mtx );
      return ret;
   }

private:
   string _block_id;
   int _client_count;

   pthread_mutex_t _mtx;
   pthread_cond_t _cond;
   int _counter;
   bool _quit;
}; // struct SharedState


struct ThreadArgs {
    rt_handle_t *my_rt_handle;
    SharedState *shared_state_p;

    ThreadArgs( rt_handle_t* my_rt_handle_in, SharedState* shared_state_p_in ) :
            my_rt_handle(my_rt_handle_in), shared_state_p(shared_state_p_in)
    {
        // Nothing to do.
    }
}; // struct ThreadArgs


struct ThreadInfo {
    pthread_t thread;
    rt_handle_t *my_rt_handle;

    ThreadInfo( pthread_t thread_in, rt_handle_t *my_rt_handle_in ) :
            thread( thread_in ), my_rt_handle( my_rt_handle_in )
    {
        // Nothing to do.
    }

    ThreadInfo()
    {
        // Nothing to do.
    }

    ThreadInfo( const ThreadInfo& other ) :
            thread( other.thread ), my_rt_handle( other.my_rt_handle )
    {
        // Nothing to do.
    }

    ThreadInfo& operator=( const ThreadInfo& other ) {
        thread = other.thread;
        my_rt_handle = other.my_rt_handle;
        return *this;
    }
}; // struct ThreadInfo


extern "C" void* client_thread( void* arg_void );

static cb_ret_t my_rt_end(
      rt_handle_t **handle,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_block_added(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      pm_block_id_t block_id,
      rm_block_state_t block_new_state,
      rt_raw_state_t block_raw_new_state,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_block_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      pm_block_id_t block_id,
      rm_block_state_t block_new_state,
      rm_block_state_t block_old_state,
      rt_raw_state_t block_raw_new_state,
      rt_raw_state_t block_raw_old_state,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_block_deleted_fn_p(
      rt_handle_t **handle,
      rm_sequence_id_t previous_seq_id,
      pm_block_id_t block_id,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_job_added(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      db_job_id_t job_id,
      pm_block_id_t block_id,
      rm_job_state_t job_new_state,
      rt_raw_state_t job_raw_new_state,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_job_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      db_job_id_t job_id,
      pm_block_id_t block_id,
      rm_job_state_t job_new_state,
      rm_job_state_t job_old_state,
      rt_raw_state_t job_raw_new_state,
      rt_raw_state_t job_raw_old_state,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_job_deleted(
      rt_handle_t **handle,
      rm_sequence_id_t previous_seq_id,
      db_job_id_t job_id,
      pm_block_id_t block_id,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_BP_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      rm_bp_id_t bp_id,
      rm_BP_state_t BP_new_state,
      rm_BP_state_t BP_old_state,
      rt_raw_state_t BP_raw_new_state,
      rt_raw_state_t BP_raw_old_state,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_switch_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      rm_switch_id_t switch_id,
      rm_bp_id_t bp_id,
      rm_switch_state_t switch_new_state,
      rm_switch_state_t switch_old_state,
      rt_raw_state_t switch_raw_new_state,
      rt_raw_state_t switch_raw_old_state,
      void* extended_args,
      void* data );

static cb_ret_t my_rt_nodecard_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      rm_nodecard_id_t nodecard_id,
      rm_bp_id_t bp_id,
      rm_nodecard_state_t nodecard_new_state,
      rm_nodecard_state_t nodecard_old_state,
      rt_raw_state_t nodecard_raw_new_state,
      rt_raw_state_t nodecard_raw_old_state,
      void* extended_args,
      void* data );

static void report_sql_error(
      const char *function_name,
      SQLSMALLINT handleType,
      SQLHANDLE handle );

static void response_time2( int client_count );



int main(
      int argc,
      char *argv[] )
{
   cout << argv[0] << ": started.\n";

   int client_count(1);

   if ( argc > 1 ) {
      istringstream iss(argv[1]);
      iss >> client_count;
      if ( ! iss ) {
         cerr << "Failed to get client count from '" << argv[1] << "'.\n";
         return 1;
      }
   }

   response_time2( client_count );
   cout << argv[0] << ": finished.\n";
} // main()


void* client_thread( void* arg_void )
{
    ThreadArgs *thread_args_p(static_cast<ThreadArgs*>(arg_void));
    SharedState *shared_state_p(static_cast<SharedState*>(thread_args_p->shared_state_p));
    rt_handle_t *my_rt_handle(thread_args_p->my_rt_handle);

    rt_status_t my_rt_status(RT_STATUS_OK);

    my_rt_status = rt_read_msgs( &my_rt_handle, shared_state_p );
    if ( my_rt_status != RT_STATUS_OK ) {
        cerr << "rt_read_msgs failed. status=" << my_rt_status << "\n";
    }

    my_rt_status = rt_close( &my_rt_handle );
    if ( my_rt_status != RT_STATUS_OK ) {
        cerr << "Failed to close realtime handle. status=" << my_rt_status << "\n";
    }

    cerr << "Disconnected " << &my_rt_handle << ".\n";

    delete thread_args_p;

    return 0;
} // client_thread()


cb_ret_t my_rt_end(
      rt_handle_t **handle,
      void* extended_args,
      void* data )
{
   cout << "\nRealtime ended for " << *handle << "!\n";
   SharedState* shared_state_p(static_cast<SharedState*>(data));
   shared_state_p->setQuit();
   return RT_CALLBACK_QUIT;
} // my_rt_end()


cb_ret_t my_rt_block_added(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      pm_block_id_t block_id,
      rm_block_state_t block_new_state,
      rt_raw_state_t block_raw_new_state,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_block_added()


cb_ret_t my_rt_block_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      pm_block_id_t block_id,
      rm_block_state_t block_new_state,
      rm_block_state_t block_old_state,
      rt_raw_state_t block_raw_new_state,
      rt_raw_state_t block_raw_old_state,
      void* extended_args,
      void* data )
{
   SharedState *shared_state_p(static_cast<SharedState*>(data));

   if ( shared_state_p->getBlockId() != string(block_id) ) {
      return RT_CALLBACK_CONTINUE;
   }

   shared_state_p->incrCounter();

   cb_ret_t ret(shared_state_p->checkQuit() ? RT_CALLBACK_QUIT : RT_CALLBACK_CONTINUE);
   return ret;
} // my_rt_block_state_changed()


cb_ret_t my_rt_block_deleted_fn_p(
      rt_handle_t **handle,
      rm_sequence_id_t previous_seq_id,
      pm_block_id_t block_id,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_block_deleted_fn_p()


cb_ret_t my_rt_job_added(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      db_job_id_t job_id,
      pm_block_id_t block_id,
      rm_job_state_t job_new_state,
      rt_raw_state_t job_raw_new_state,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_job_added()


cb_ret_t my_rt_job_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      db_job_id_t job_id,
      pm_block_id_t block_id,
      rm_job_state_t job_new_state,
      rm_job_state_t job_old_state,
      rt_raw_state_t job_raw_new_state,
      rt_raw_state_t job_raw_old_state,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_job_state_changed()


cb_ret_t my_rt_job_deleted(
      rt_handle_t **handle,
      rm_sequence_id_t previous_seq_id,
      db_job_id_t job_id,
      pm_block_id_t block_id,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_job_deleted()


cb_ret_t my_rt_BP_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      rm_bp_id_t bp_id,
      rm_BP_state_t BP_new_state,
      rm_BP_state_t BP_old_state,
      rt_raw_state_t BP_raw_new_state,
      rt_raw_state_t BP_raw_old_state,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_BP_state_changed()


cb_ret_t my_rt_switch_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      rm_switch_id_t switch_id,
      rm_bp_id_t bp_id,
      rm_switch_state_t switch_new_state,
      rm_switch_state_t switch_old_state,
      rt_raw_state_t switch_raw_new_state,
      rt_raw_state_t switch_raw_old_state,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_switch_state_changed()


cb_ret_t my_rt_nodecard_state_changed(
      rt_handle_t **handle,
      rm_sequence_id_t seq_id,
      rm_sequence_id_t previous_seq_id,
      rm_nodecard_id_t nodecard_id,
      rm_bp_id_t bp_id,
      rm_nodecard_state_t nodecard_new_state,
      rm_nodecard_state_t nodecard_old_state,
      rt_raw_state_t nodecard_raw_new_state,
      rt_raw_state_t nodecard_raw_old_state,
      void* extended_args,
      void* data )
{
   return RT_CALLBACK_CONTINUE;
} // my_rt_nodecard_state_changed()


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


void response_time2( int client_count )
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
                          db_name, SQL_NTS,
                          db_user, SQL_NTS,
                          db_pass, SQL_NTS );

      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLConnect", SQL_HANDLE_DBC, hdbc );
      } else {
         disconnect = true;
      }
   }

   string block_id("testblock_0");

   SQLHSTMT hstmt(SQL_NULL_HANDLE);
   if ( SQL_SUCCESS == sqlrc ) {
      sqlrc = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &hstmt );
      if ( SQL_SUCCESS != sqlrc ) {
         hstmt = SQL_NULL_HANDLE;
         report_sql_error( "SQLAllocHandle update block status statement", SQL_HANDLE_DBC, hdbc );
      }
   }

   if ( SQL_SUCCESS == sqlrc ) {
      string sql_str(string("UPDATE tbgpBlock SET status = ? WHERE blockId = '") + block_id + "'");
      SQLCHAR *sql = new SQLCHAR[sql_str.size() + 1];
      memcpy( sql, sql_str.c_str(), sql_str.size() + 1 );
      sqlrc = SQLPrepare( hstmt, sql, SQL_NTS );
      delete[] sql;
      if ( sqlrc != SQL_SUCCESS ) {
         report_sql_error( "SQLPrepare update block status", SQL_HANDLE_STMT, hstmt );
      }
   }

   char status_vals[] = "IF";
   SQLCHAR new_status_val[] = { status_vals[0], '\0' };
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

    // Start up all the clients.
    SharedState shared_state(block_id, client_count);

    rt_callbacks_t my_rt_callbacks;

    my_rt_callbacks.end_cb = &my_rt_end;
    my_rt_callbacks.block_added_cb = &my_rt_block_added;
    my_rt_callbacks.block_state_changed_cb = &my_rt_block_state_changed;
    my_rt_callbacks.block_deleted_cb = &my_rt_block_deleted_fn_p;
    my_rt_callbacks.job_added_cb = &my_rt_job_added;
    my_rt_callbacks.job_state_changed_cb = &my_rt_job_state_changed;
    my_rt_callbacks.job_deleted_cb = &my_rt_job_deleted;
    my_rt_callbacks.bp_state_changed_cb = &my_rt_BP_state_changed;
    my_rt_callbacks.switch_state_changed_cb = &my_rt_switch_state_changed;
    my_rt_callbacks.nodecard_state_changed_cb = &my_rt_nodecard_state_changed;

    cout << "Connecting " << client_count << " realtime clients.\n";

    vector<ThreadInfo> thread_infos;

    for ( int i(0) ; i < client_count ; ++i ) {
        rt_status_t my_rt_status(RT_STATUS_OK);

        rt_handle_t *my_rt_handle(0);

        my_rt_status = rt_init( &my_rt_handle, RT_BLOCKING, &my_rt_callbacks );
        if ( my_rt_status != RT_STATUS_OK ) {
            cerr << "Failed to initialize realtime handle. status=" << my_rt_status << "\n";
        }

        if ( RT_STATUS_OK == my_rt_status ) {
            my_rt_status = rt_request_realtime( &my_rt_handle );
            if ( my_rt_status != RT_STATUS_OK ) {
                cerr << "Failed to request realtime. status=" << my_rt_status << "\n";
            }
        }

        ThreadArgs *thread_args_p(new ThreadArgs(my_rt_handle, &shared_state));

        pthread_t client_pthread;
        int create_rc(pthread_create( &client_pthread, 0, &client_thread, thread_args_p ));
        if ( create_rc != 0 ) {
            cerr << "Failed to create thread " << i << ". rc=" << create_rc << endl;
        }

        ThreadInfo thread_info(client_pthread, my_rt_handle);
        thread_infos.push_back( thread_info );
    }

    int total_iterations(500);

    cout << "Performing " << total_iterations << " update + wait.\n";

    // Get the start time before running the tests for real.
    struct timespec start_time;
    clock_gettime( CLOCK_REALTIME, &start_time );

    // Loop: update the status, then wait for all clients to receive the change.

    for ( int i(0) ; (i < total_iterations) && (SQL_SUCCESS == sqlrc) ; ++i ) {
        shared_state.resetCounter();

        // If this is the last iteration tell the realtime clients to disconnect after receiving.
        if ( i == total_iterations-1 ) {
            shared_state.setQuit();
        }

        // Update the block status in the db.
        new_status_val[0] = status_vals[i % (sizeof ( status_vals )-1)];
        new_status_val_ind = SQL_NTS;

        sqlrc = SQLExecute( hstmt );
        if ( sqlrc != SQL_SUCCESS ) {
            report_sql_error( "SQLExecute update block status to I", SQL_HANDLE_STMT, hstmt );
        }

        shared_state.waitForCounter();
    }

    // Get the end time.
    struct timespec end_time;
    clock_gettime( CLOCK_REALTIME, &end_time );

    // Wait for the client threads to complete.
    cout << "Waiting for the client threads to complete.\n";
    for ( vector<ThreadInfo>::iterator i(thread_infos.begin()) ; i != thread_infos.end() ; ++i ) {
        pthread_t thread(i->thread);
        int join_rc(pthread_join( thread, 0 ));
        if ( join_rc != 0 ) {
            cerr << "Failed to join thread " << thread << ". rc=" << join_rc << endl;
        }
    }

    // Set the status to F.
    if ( SQL_SUCCESS == sqlrc ) {
        new_status_val[0] = 'F';
        new_status_val_ind =  SQL_NTS;

        sqlrc = SQLExecute( hstmt );
        if ( sqlrc != SQL_SUCCESS ) {
            report_sql_error( "SQLExecute update block status to F", SQL_HANDLE_STMT, hstmt );
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

    double start_time_dbl = double(start_time.tv_sec) + (double(start_time.tv_nsec) / 1000000000.0);
    double end_time_dbl = double(end_time.tv_sec) + (double(end_time.tv_nsec) / 1000000000.0);
    double total_time = end_time_dbl - start_time_dbl;

    cout << "\nTotals:\n";
    cout << "  Ran " << total_iterations << " iterations.\n";
    cout << "  Total time: " << total_time << endl;
    cout << "  Iterations per sec: " << (total_iterations / total_time) << endl;

    delete[] db_name;
    delete[] db_user;
    delete[] db_pass;
} // response_time2()
