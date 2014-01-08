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

#ifndef RTDBVIEW_H
#define RTDBVIEW_H

#include "bg_realtime.h"

#include <string>


// These declarations must match the ones in bg_realtime.h

typedef SQL_API_RC SQL_API_FN (*realtime_initialize_fp_t)(
    struct sqludf_vc_fbd *db_properties_filename,
    struct sqludf_vc_fbd *log_filename,
    sqlint32 *log_level,
    SQLUDF_NULLIND *db_properties_filename_nullind,
    SQLUDF_NULLIND *log_filename_nullind,
    SQLUDF_NULLIND *log_level_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_shutdown_fp_t)(
        SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_block_added_fp_t)(
    char blockid[BLOCKSIZE],
    char status[STATESIZE],
    sqlint64 *newseqid,
    SQLUDF_NULLIND *blockid_nullind,
    SQLUDF_NULLIND *status_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_block_state_change_fp_t)(
    char blockid[BLOCKSIZE],
    char newstatus[STATESIZE],
    char oldstatus[STATESIZE],
    sqlint64 *newseqid,
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *blockid_nullind,
    SQLUDF_NULLIND *newstatus_nullind,
    SQLUDF_NULLIND *oldstatus_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_block_deleted_fp_t)(
    char blockid[BLOCKSIZE],
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *blockid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_job_added_fp_t)(
    int *jobid,
    char blockid[BLOCKSIZE],
    char status[STATESIZE],
    sqlint64 *newseqid,
    SQLUDF_NULLIND *jobid_nullind,
    SQLUDF_NULLIND *blockid_nullind,
    SQLUDF_NULLIND *status_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_job_state_change_fp_t)(
    int *jobid,
    char blockid[BLOCKSIZE],
    char newstatus[STATESIZE],
    char oldstatus[STATESIZE],
    sqlint64 *newseqid,
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *jobid_nullind,
    SQLUDF_NULLIND *blockid_nullind,
    SQLUDF_NULLIND *newstatus_nullind,
    SQLUDF_NULLIND *oldstatus_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_job_deleted_fp_t)(
    int *jobid,
    char blockid[BLOCKSIZE],
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *jobid_nullind,
    SQLUDF_NULLIND *blockid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_BP_state_change_fp_t)(
    char bp_position[BPSIZE],
    char newstatus[STATESIZE],
    char oldstatus[STATESIZE],
    sqlint64 *newseqid,
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *bp_position_nullind,
    SQLUDF_NULLIND *newstatus_nullind,
    SQLUDF_NULLIND *oldstatus_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_nodecard_state_change_fp_t)(
    char nodecard_position[NODECARDSIZE],
    char bp_position[BPSIZE],
    char newstatus[STATESIZE],
    char oldstatus[STATESIZE],
    sqlint64 *newseqid,
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *nodecard_position_nullind,
    SQLUDF_NULLIND *bp_position_nullind,
    SQLUDF_NULLIND *newstatus_nullind,
    SQLUDF_NULLIND *oldstatus_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );

typedef SQL_API_RC SQL_API_FN (*realtime_switch_state_change_fp_t)(
    char* switchid_in,
    char bp_position[BPSIZE],
    char newstatus[STATESIZE],
    char oldstatus[STATESIZE],
    sqlint64 *newseqid,
    sqlint64 *oldseqid,
    SQLUDF_NULLIND *switchid_nullind,
    SQLUDF_NULLIND *bp_position_nullind,
    SQLUDF_NULLIND *newstatus_nullind,
    SQLUDF_NULLIND *oldstatus_nullind,
    SQLUDF_NULLIND *newseqid_nullind,
    SQLUDF_NULLIND *oldseqid_nullind,
    SQLUDF_TRAIL_ARGS
 );


class symbol_not_found_t
{
    std::string symname;
    public:
        symbol_not_found_t( const std::string& symname_in ) : symname( symname_in ) {}
        const std::string& get_symname() const { return symname; }
};


class rtdbview_t
{
    static const std::string initialize_fn;
    static const std::string shutdown_fn;
    static const std::string block_added_fn;
    static const std::string block_state_change_fn;
    static const std::string block_deleted_fn;
    static const std::string job_added_fn;
    static const std::string job_state_change_fn;
    static const std::string job_deleted_fn;
    static const std::string bp_state_change_fn;
    static const std::string nodecard_state_change_fn;
    static const std::string switch_state_change_fn;


    void *handle;

    realtime_initialize_fp_t initialize_fp;
    realtime_shutdown_fp_t shutdown_fp;
    realtime_block_added_fp_t block_added_fp;
    realtime_block_state_change_fp_t block_state_change_fp;
    realtime_block_deleted_fp_t block_deleted_fp;
    realtime_job_added_fp_t job_added_fp;
    realtime_job_state_change_fp_t job_state_change_fp;
    realtime_job_deleted_fp_t job_deleted_fp;
    realtime_BP_state_change_fp_t bp_state_change_fp;
    realtime_nodecard_state_change_fp_t nodecard_state_change_fp;
    realtime_switch_state_change_fp_t switch_state_change_fp;

    int seq_id;

    char sqlst[SQLUDF_SQLSTATE_LEN+1];
    char qualname[SQLUDF_FQNAME_LEN+1];
    char specname[SQLUDF_SPECNAME_LEN+1];
    char diagmsg[SQLUDF_MSGTEXT_LEN+1];

    void check_rc( SQL_API_RC rc, const std::string &function_name );

    public:

        /*!
         * @param filename The name of the realtime server so file.
         */
        rtdbview_t( const std::string &filename );

        ~rtdbview_t();

        int init( const std::string *db_properties_filename,
                  const std::string *log_filename,
                  int *log_level );

        int shutdown();

        int block_added( const std::string *blockid, char status );
        int block_state_change( const std::string *blockid, char newstatus, char oldstatus );
        int block_deleted( const std::string *blockid );

        int job_added( int jobid, const std::string *blockid, char status );
        int job_state_change( int jobid, const std::string *blockid, char newstatus, char oldstatus );
        int job_deleted( int jobid, const std::string *blockid );

        int bp_state_change( const std::string *bp_position, char newstatus, char oldstatus );
        int nodecard_state_change( const std::string *nodecard_position, const std::string *bp_position, char newstatus, char oldstatus );
        int switch_state_change( const std::string *switchid, const std::string *bp_position, char newstatus, char oldstatus );
};


#endif
