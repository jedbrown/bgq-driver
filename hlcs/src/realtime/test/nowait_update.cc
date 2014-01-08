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
/* -------------------------------------------------------------  */

/* Make sure that multiple realtime clients get all the messages when
 * make a lot of updates without waiting.
 *
 * First connect x clients to the realtime server.
 * Make y updates to the database.
 * Read all the y messages.
 *
 * Last time I tried this the server disconnected all clients at 1500 updates.
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


#include "sql_util.h"

#include <getopt.h>
#include <poll.h>
#include <rt_api.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>


using namespace std;


/* ---------------------------------------------------------------------- */
/* class LockGuard                                                        */

class LockGuard {
    pthread_mutex_t *_mtx_p;

public:
    LockGuard( pthread_mutex_t* mtx_p ) : _mtx_p(mtx_p) {
        pthread_mutex_lock( _mtx_p );
    }

    ~LockGuard() {
        pthread_mutex_unlock( _mtx_p );
    }
}; // class LockGuard

/* ---------------------------------------------------------------------- */
/* struct UpdateInfo                                                      */

struct UpdateInfo {
    rm_sequence_id_t seq_id;
    rm_sequence_id_t previous_seq_id;
    rt_raw_state_t block_raw_new_state;
    rt_raw_state_t block_raw_old_state;

    UpdateInfo(
            rm_sequence_id_t seq_id_in,
            rm_sequence_id_t previous_seq_id_in,
            rt_raw_state_t block_raw_new_state_in,
            rt_raw_state_t block_raw_old_state_in ) :
        seq_id(seq_id_in),
        previous_seq_id(previous_seq_id_in),
        block_raw_new_state(block_raw_new_state_in),
        block_raw_old_state(block_raw_old_state_in)
    {
        // Nothing to do.
    }
}; // struct UpdateInfo


struct RequestStateInfo
{
    int count;
    rm_sequence_id_t exp_seq_id;
    rt_raw_state_t exp_state;

    RequestStateInfo() :
        count(0),
        exp_seq_id(-1),
        exp_state(-1)
    {
        // Nothing to do.
    }
};


/* ---------------------------------------------------------------------- */
/* class SharedState                                                      */

class SharedState {
    int _updates;
    map<rt_handle_t*,RequestStateInfo> _handle_to_rsi;
    pthread_mutex_t _mtx;

public:
    SharedState( int updates, vector<rt_handle_t*>& handles );

    bool checkFinished();
    void realtimeEnded( rt_handle_t* my_rt_handle );
    bool updateReceived( rt_handle_t* my_rt_handle, const UpdateInfo &update_info );
}; // class SharedState

SharedState::SharedState( int updates, vector<rt_handle_t*>& handles )
{
    _updates = updates;
    for ( vector<rt_handle_t*>::iterator i(handles.begin()) ; i != handles.end() ; ++i ) {
        _handle_to_rsi[*i] = RequestStateInfo();
    }
    pthread_mutex_init( &_mtx, 0 );
} // SharedState::SharedState()

bool SharedState::checkFinished()
{
    LockGuard lg(&_mtx);

    bool ret(_handle_to_rsi.empty());
    return ret;
} // SharedState::SharedState()

void SharedState::realtimeEnded( rt_handle_t* my_rt_handle )
{
    LockGuard lg(&_mtx);

    RequestStateInfo *rsi_p(&(_handle_to_rsi[my_rt_handle]));
    cout << "Realtime ended for " << my_rt_handle << " after " << rsi_p->count << " updates!\n";

    _handle_to_rsi.erase( my_rt_handle );
} // SharedState::realtimeEnded()

bool SharedState::updateReceived( rt_handle_t* my_rt_handle, const UpdateInfo &update_info )
{
    LockGuard lg(&_mtx);

    RequestStateInfo *rsi_p(&(_handle_to_rsi[my_rt_handle]));

    /*
    cout << "<" << my_rt_handle << ">:"
            " [" << rsi_p->exp_seq_id << " == " << update_info.previous_seq_id << "]"
            "->" << update_info.seq_id <<
    " [" << rsi_p->exp_state << " == " << update_info.block_raw_old_state << "]\n"; */

    if ( (rsi_p->exp_seq_id != rm_sequence_id_t(-1)) &&
         (rsi_p->exp_seq_id != update_info.previous_seq_id) ) {
        cout << "Got a message out of order! "
             << rsi_p->exp_seq_id << " != " << update_info.previous_seq_id << "\n";
    }
    rsi_p->exp_seq_id = update_info.seq_id;

    if ( (rsi_p->exp_state != rt_raw_state_t(-1)) &&
         (rsi_p->exp_state != update_info.block_raw_old_state) ) {
        cerr << "Got a message with unexpected state!\n";
    }
    rsi_p->exp_state = update_info.block_raw_new_state;

    int new_count(++(rsi_p->count));
    if ( new_count < _updates ) {
        return false;
    }

    _handle_to_rsi.erase( my_rt_handle );
    return true;
} // SharedState::updateReceived()


/* ---------------------------------------------------------------------- */
/* Static function declarations                                           */

static void do_db_updates( int updates, SQLHANDLE henv, SQLHANDLE hdbc );

static void nowait_update( int clients, int updates );



/* ---------------------------------------------------------------------- */
/* Realtime callbacks                                                     */

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

/* ----------------------------------------------------------------------- */


int main( int argc, char* argv[] ) {
    cout << "Working...\n";

    setProgramName( argv[0] );

    int clients(1);
    int updates(500);

    const int CLIENTS_OPT_IND(1);
    const int UPDATES_OPT_IND(2);

    struct option longopts[] = {
        {"clients", 1, 0, CLIENTS_OPT_IND},
        {"updates", 1, 0, UPDATES_OPT_IND},
        {0, 0, 0, 0}
    };

    int longindex(0);

    while ( true ) {
        int getopts_rc(getopt_long( argc, argv, "", longopts, &longindex ));
        if ( -1 == getopts_rc ) {
            break;
        }
        switch ( getopts_rc ) {
            case CLIENTS_OPT_IND:
                {
                    istringstream iss(optarg);
                    iss >> clients;
                }
                break;
            case UPDATES_OPT_IND:
                {
                    istringstream iss(optarg);
                    iss >> updates;
                }
                break;
        }
    }

    nowait_update( clients, updates );

    cout << "Done!\n";
    exit(0);
} // main()


void do_db_updates( int updates, SQLHANDLE henv, SQLHANDLE hdbc )
{
    cout << "Performing " << updates << " updates.\n";

    string block_id("testblock_0");

    SQLRETURN sqlrc;

    SQLHSTMT hstmt(SQL_NULL_HANDLE);
    sqlrc = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &hstmt );
    if ( SQL_SUCCESS != sqlrc ) {
        hstmt = SQL_NULL_HANDLE;
        report_sql_error( "SQLAllocHandle update block status statement", SQL_HANDLE_DBC, hdbc );
    }

    if ( SQL_SUCCESS == sqlrc ) {
        string sql_str("UPDATE tbgpBlock SET status = ? WHERE blockId = '");
        sql_str += block_id;
        sql_str += "'";
        SQLCHAR *sql(new SQLCHAR[sql_str.size() + 1]);
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

    for ( int i(0) ; (i < updates - 1) && (SQL_SUCCESS == sqlrc) ; ++i ) {
        // Update the block status in the db.
        new_status_val[0] = status_vals[i % (sizeof ( status_vals )-1)];
        new_status_val_ind = SQL_NTS;

        sqlrc = SQLExecute( hstmt );
        if ( sqlrc != SQL_SUCCESS ) {
            report_sql_error( "SQLExecute update block status to I", SQL_HANDLE_STMT, hstmt );
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
} // do_db_updates()


void nowait_update( int clients, int updates )
{
    cout << "Options:\n"
         << "  Clients: " << clients << "\n"
         << "  Updates: " << updates << "\n"
         << "\n";

    SQLHANDLE henv(SQL_NULL_HANDLE);
    SQLHANDLE hdbc(SQL_NULL_HANDLE);

    bool status_ok(true);

    int db_connect_rc(connect_to_db( &henv, &hdbc ));
    if ( db_connect_rc != 0 ) {
        return;
    }


    // Initialize realtime callbacks.

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


    // Open a bunch of connections to the realtime server.

    rt_status_t my_rt_status;

    cout << "Initializing " << clients << " realtime handles.\n";
    vector<rt_handle_t*> my_rt_handles;
    for ( int i(0) ; i < clients ; ++i ) {
        rt_handle_t *my_rt_handle(0);

        my_rt_status = rt_init( &my_rt_handle, RT_BLOCKING, &my_rt_callbacks );
        if ( my_rt_status != RT_STATUS_OK ) {
            cerr << "rt_init failed. rc=" << my_rt_status << "\n";
            status_ok = false;
            break;
        }

        my_rt_status = rt_request_realtime( &my_rt_handle );
        if ( my_rt_status != RT_STATUS_OK ) {
            cerr << "rt_request_realtime failed. rc=" << my_rt_status << "\n";
            status_ok = false;
        }

        my_rt_status = rt_set_nonblocking( &my_rt_handle );
        if ( my_rt_status != RT_STATUS_OK ) {
            cerr << "rt_set_nonblocking failed. rc=" << my_rt_status << "\n";
            status_ok = false;
        }

        my_rt_handles.push_back( my_rt_handle );
    }

    do_db_updates( updates, henv, hdbc );

    map<int,rt_handle_t*> sd_to_handle;

    struct pollfd *fds(new struct pollfd[clients]);
    for ( int i(0) ; i < clients ; ++i ) {
        int sd;
        rt_get_socket_descriptor( &(my_rt_handles[i]), &sd );
        fds[i].fd = sd;
        fds[i].events = POLLIN;

        sd_to_handle[sd] = my_rt_handles[i];
    }

    SharedState shared_state(updates,my_rt_handles);

    while ( status_ok && (! shared_state.checkFinished()) ) {
        int poll_rc(poll( fds, clients, -1 ));
        if ( poll_rc < 0 ) {
            cerr << "poll failed. rc=" << poll_rc << "\n";
            status_ok = false;
        }
        if ( poll_rc > 0 ) {
            for ( int i(0) ; i < clients ; ++i ) {
                if ( fds[i].revents | POLLIN ) {
                    int sd(fds[i].fd);
                    rt_handle_t *my_rt_handle(sd_to_handle[sd]);
                    rt_read_msgs( &my_rt_handle, &shared_state );
                }
            }
        }
    }

    cout << "All " << clients << " clients finished.\n";

    // Cleanup.

    cout << "Closing " << my_rt_handles.size() << " realtime handles.\n";
    for ( vector<rt_handle_t*>::iterator i(my_rt_handles.begin()) ;
          i != my_rt_handles.end() ; ++i ) {
        rt_handle_t *my_rt_handle(*i);
        my_rt_status = rt_close( &my_rt_handle );
        if ( my_rt_status != RT_STATUS_OK ) {
            cerr << "Failed to close a realtime handle.\n";
        }
    }

    disconnect_from_db( henv, hdbc );
} // nowait_update()


/* ---------------------------------------------------------------------- */
/* Realtime callbacks                                                     */

cb_ret_t my_rt_end(
        rt_handle_t **handle,
        void* extended_args,
        void* data )
{
    SharedState *shared_state_p(static_cast<SharedState*>(data));
    shared_state_p->realtimeEnded( *handle );

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
    if ( shared_state_p->updateReceived( *handle,
            UpdateInfo(seq_id, previous_seq_id,
                       block_raw_new_state, block_raw_old_state) ) ) {
        // cout << "Handle " << *handle << " received all updates.\n";
    }
    return RT_CALLBACK_CONTINUE;
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

/* ---------------------------------------------------------------------- */
