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


#include "rt_api.h"

#include <unistd.h>

#include <iostream>
#include <string>

using namespace std;

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

static void connect_disconnect()
{
    rt_status_t my_rt_status;

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

    rt_handle_t *my_rt_handle;

    my_rt_status = rt_init( &my_rt_handle, RT_BLOCKING, &my_rt_callbacks );

    int sd;
    rt_get_socket_descriptor( &my_rt_handle, &sd );

    my_rt_status = rt_request_realtime( &my_rt_handle );

    my_rt_status = rt_read_msgs( &my_rt_handle, NULL );

    cout << "closing " << sd;
    rt_close( &my_rt_handle );
} // connect_disconnect()


static void test_disconnect_problem_client()
{
    const int DISCONNECTS(100);

    for ( int i = 0 ; i < DISCONNECTS ; ++i ) {
        cout << "disconnect " << (i+1) << " of " << DISCONNECTS << endl;
        connect_disconnect();
    }

    cout << "Done disconnecting." << endl;
} // test_disconnect_problem_client()


int main( int argc, char* argv[] )
{
    test_disconnect_problem_client();
} // main()

/* ---------------------------------------------------------------------- */
/* Realtime callbacks                                                     */

cb_ret_t my_rt_end(
        rt_handle_t **handle,
        void* extended_args,
        void* data )
{
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
    return RT_CALLBACK_QUIT;
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
    cout << "my_rt_block_state_changed!" <<endl;
    return RT_CALLBACK_QUIT;
} // my_rt_block_state_changed()


cb_ret_t my_rt_block_deleted_fn_p(
        rt_handle_t **handle,
        rm_sequence_id_t previous_seq_id,
        pm_block_id_t block_id,
        void* extended_args,
        void* data )
{
    return RT_CALLBACK_QUIT;
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
    return RT_CALLBACK_QUIT;
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
    return RT_CALLBACK_QUIT;
} // my_rt_job_state_changed()


cb_ret_t my_rt_job_deleted(
        rt_handle_t **handle,
        rm_sequence_id_t previous_seq_id,
        db_job_id_t job_id,
        pm_block_id_t block_id,
        void* extended_args,
        void* data )
{
    return RT_CALLBACK_QUIT;
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
    return RT_CALLBACK_QUIT;
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
    return RT_CALLBACK_QUIT;
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
    return RT_CALLBACK_QUIT;
} // my_rt_nodecard_state_changed()

/* ---------------------------------------------------------------------- */
