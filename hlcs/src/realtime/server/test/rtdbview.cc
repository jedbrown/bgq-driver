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

#include "rtdbview.h"

#include <dlfcn.h>
#include <string.h>

#include <iostream>
#include <new>

using namespace std;

class varchar_t
{
    static const string EMPTY_STR;

    char *buf;
    public:
        struct sqludf_vc_fbd *vc;
        SQLUDF_NULLIND nullind;

        varchar_t( const string *str );
        ~varchar_t();
};

const string varchar_t::EMPTY_STR( "" );

varchar_t::varchar_t( const string *str )
{
    buf = new char[sizeof ( struct sqludf_vc_fbd ) + (0 == str ? 0 : str->length())];

    nullind = (0 == str ? -1 : 0);

    if ( 0 == str ) {
        str = &EMPTY_STR;
    }

    vc = (struct sqludf_vc_fbd*) (buf);
    vc->length = str->length();
    strcpy( vc->data, str->c_str() );
}

varchar_t::~varchar_t()
{
    delete[] buf;
}


class fixed_char_t
{
    public:
        char *buf;
        SQLUDF_NULLIND nullind;

        fixed_char_t( const std::string *str, int buf_size );

        ~fixed_char_t();
};

fixed_char_t::fixed_char_t( const std::string *str, int buf_size )
{
    buf = new char[buf_size];
    std::memset( buf, ' ', buf_size );
    buf[buf_size - 1] = '\0';

    nullind = (0 == str ? -1 : 0);

    if ( 0 == str ) {
        return;
    }

    int copy_len = (str->length() > (buf_size - 1) ? buf_size - 1 : str->length());
    std::memcpy( buf, str->c_str(), copy_len );
}

fixed_char_t::~fixed_char_t()
{
    delete[] buf;
}


class char_code_t
{
    public:
        char buf[2];
        SQLUDF_NULLIND nullind;

        char_code_t( char code ) {
            buf[0] = code;
            buf[1] = '\0';
            nullind = 0;
        }
};


const string rtdbview_t::initialize_fn( "realtime_initialize" );
const string rtdbview_t::shutdown_fn( "realtime_shutdown" );
const string rtdbview_t::block_added_fn( "realtime_block_added" );
const string rtdbview_t::block_state_change_fn( "realtime_block_state_change" );
const string rtdbview_t::block_deleted_fn( "realtime_block_deleted" );
const string rtdbview_t::job_added_fn( "realtime_job_added" );
const string rtdbview_t::job_state_change_fn( "realtime_job_state_change" );
const string rtdbview_t::job_deleted_fn( "realtime_job_deleted" );
const string rtdbview_t::bp_state_change_fn( "realtime_BP_state_change" );
const string rtdbview_t::nodecard_state_change_fn( "realtime_nodecard_state_change" );
const string rtdbview_t::switch_state_change_fn( "realtime_switch_state_change" );


void rtdbview_t::check_rc( SQL_API_RC rc, const string &function_name )
{
    if ( rc != 0 ) {
        char diagmsg2[sizeof ( diagmsg ) + 1];
        memcpy( diagmsg2, diagmsg, sizeof ( diagmsg ) );
        diagmsg2[sizeof ( diagmsg2 ) - 1] = '\0';
        cout << function_name << " returned " << rc << " diagmsg is " << diagmsg2 << "\n";
    }
}


rtdbview_t::rtdbview_t( const string &filename )
{
    memset( sqlst, '\0', sizeof ( sqlst ) );
    memset( qualname, ' ', sizeof ( qualname ) );
    qualname[sizeof ( qualname ) - 1] = '\0';
    memset( specname, ' ', sizeof ( specname ) );
    specname[sizeof ( specname ) - 1] = '\0';
    memset( diagmsg, ' ', sizeof ( diagmsg ) );
    diagmsg[sizeof ( diagmsg ) - 1] = '\0';

    handle = dlopen( filename.c_str(), RTLD_NOW );
    if ( NULL == handle )  cerr << "Failed to open " << filename << ": " << dlerror() << endl;

    initialize_fp = (realtime_initialize_fp_t) dlsym( handle, initialize_fn.c_str() );
    if ( 0 == initialize_fp )  throw new symbol_not_found_t( initialize_fn );
    shutdown_fp = (realtime_shutdown_fp_t) dlsym( handle, shutdown_fn.c_str() );
    if ( 0 == shutdown_fp )  throw new symbol_not_found_t( shutdown_fn );
    block_added_fp = (realtime_block_added_fp_t) dlsym( handle, block_added_fn.c_str() );
    if ( 0 == block_added_fp )  throw new symbol_not_found_t( block_added_fn );
    block_state_change_fp = (realtime_block_state_change_fp_t) dlsym( handle, block_state_change_fn.c_str() );
    if ( 0 == block_state_change_fp )  throw new symbol_not_found_t( block_state_change_fn );
    block_deleted_fp = (realtime_block_deleted_fp_t) dlsym( handle, block_deleted_fn.c_str() );
    if ( 0 == block_deleted_fp )  throw new symbol_not_found_t( block_deleted_fn );
    job_added_fp = (realtime_job_added_fp_t) dlsym( handle, job_added_fn.c_str() );
    if ( 0 == job_added_fp )  throw new symbol_not_found_t( job_added_fn );
    job_state_change_fp = (realtime_job_state_change_fp_t) dlsym( handle, job_state_change_fn.c_str() );
    if ( 0 == job_state_change_fp )  throw new symbol_not_found_t( job_state_change_fn );
    job_deleted_fp = (realtime_job_deleted_fp_t) dlsym( handle, job_deleted_fn.c_str() );
    if ( 0 == job_deleted_fp )  throw new symbol_not_found_t( job_deleted_fn );
    bp_state_change_fp = (realtime_BP_state_change_fp_t) dlsym( handle, bp_state_change_fn.c_str() );
    if ( 0 == bp_state_change_fp )  throw new symbol_not_found_t( bp_state_change_fn );
    nodecard_state_change_fp = (realtime_nodecard_state_change_fp_t) dlsym( handle, nodecard_state_change_fn.c_str() );
    if ( 0 == nodecard_state_change_fp )  throw new symbol_not_found_t( nodecard_state_change_fn );
    switch_state_change_fp = (realtime_switch_state_change_fp_t) dlsym( handle, switch_state_change_fn.c_str() );
    if ( 0 == switch_state_change_fp )  throw new symbol_not_found_t( switch_state_change_fn );
} // rtdbview_t::rtdbview_t()

int rtdbview_t::init( const string *db_properties_filename, const string *log_filename,
                      int *log_level )
{
    varchar_t db_properties_filename_vc( db_properties_filename );
    varchar_t log_filename_vc( log_filename );

    sqlint32 log_level_val( 0 == log_level ? 0 : *log_level );
    SQLUDF_NULLIND log_level_nullind( 0 == log_level ? -1 : 0 );

    SQL_API_RC rc = (*initialize_fp)(
        db_properties_filename_vc.vc,
        log_filename_vc.vc,
        &log_level_val,
        &db_properties_filename_vc.nullind,
        &log_filename_vc.nullind,
        &log_level_nullind,
        sqlst, qualname, specname, diagmsg );

    check_rc( rc, initialize_fn );

    return rc;
}

int rtdbview_t::shutdown()
{
    SQL_API_RC rc = (*shutdown_fp)( sqlst, qualname, specname, diagmsg );
    check_rc( rc, shutdown_fn );
    return rc;
}

int rtdbview_t::block_added( const string *blockid, char status )
{
    fixed_char_t blockid_fc( blockid, BLOCKSIZE );
    char_code_t status_cc( status );

    sqlint64 newseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 );

    SQL_API_RC rc = (*block_added_fp)( blockid_fc.buf, status_cc.buf, &newseqid,
    &blockid_fc.nullind, &status_cc.nullind, &newseqid_nullind,
    sqlst, qualname, specname, diagmsg );
    check_rc( rc, block_added_fn );
    return rc;
}

int rtdbview_t::block_state_change( const string *blockid, char newstatus, char oldstatus )
{
    fixed_char_t blockid_fc( blockid, BLOCKSIZE );
    char_code_t newstatus_cc( newstatus );
    char_code_t oldstatus_cc( oldstatus );

    sqlint64 newseqid( 0 ), oldseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 ), oldseqid_nullind( 0 );

    SQL_API_RC rc = (*block_state_change_fp)( blockid_fc.buf,
    newstatus_cc.buf, oldstatus_cc.buf,
    &newseqid, &oldseqid,
    &blockid_fc.nullind,
    &newstatus_cc.nullind, &oldstatus_cc.nullind,
    &newseqid_nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );
    check_rc( rc, block_state_change_fn );
    return rc;
}

int rtdbview_t::block_deleted( const string *blockid )
{
    fixed_char_t blockid_fc( blockid, BLOCKSIZE );

    sqlint64 oldseqid( 0 );
    SQLUDF_NULLIND oldseqid_nullind( 0 );

    SQL_API_RC rc = (*block_deleted_fp)( blockid_fc.buf, &oldseqid,
    &blockid_fc.nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    check_rc( rc, block_deleted_fn );
    return rc;
}

int rtdbview_t::job_added( int jobid, const string *blockid, char status )
{
    SQLUDF_NULLIND jobid_nullind( 0 );

    fixed_char_t blockid_fc( blockid, BLOCKSIZE );
    char_code_t status_cc( status );

    sqlint64 newseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 );

    SQL_API_RC rc = (*job_added_fp)( &jobid, blockid_fc.buf, status_cc.buf, &newseqid,
    &jobid_nullind, &blockid_fc.nullind, &status_cc.nullind, &newseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    check_rc( rc, job_added_fn );
    return rc;
}

int rtdbview_t::job_state_change( int jobid, const string *blockid, char newstatus, char oldstatus )
{
    SQLUDF_NULLIND jobid_nullind( 0 );
    fixed_char_t blockid_fc( blockid, BLOCKSIZE );
    char_code_t newstatus_cc( newstatus );
    char_code_t oldstatus_cc( oldstatus );

    sqlint64 newseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 );
    sqlint64 oldseqid( 0 );
    SQLUDF_NULLIND oldseqid_nullind( 0 );

    SQL_API_RC rc = (*job_state_change_fp)( &jobid, blockid_fc.buf,
    newstatus_cc.buf, oldstatus_cc.buf,
    &newseqid, &oldseqid,
    &jobid_nullind, &blockid_fc.nullind,
    &newstatus_cc.nullind, &oldstatus_cc.nullind,
    &newseqid_nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    check_rc( rc, job_state_change_fn );
    return rc;
}

int rtdbview_t::job_deleted( int jobid, const string *blockid )
{
    SQLUDF_NULLIND jobid_nullind( 0 );
    fixed_char_t blockid_fc( blockid, BLOCKSIZE );

    sqlint64 oldseqid( 0 );
    SQLUDF_NULLIND oldseqid_nullind( 0 );

    SQL_API_RC rc = (*job_deleted_fp)( &jobid, blockid_fc.buf, &oldseqid,
    &jobid_nullind, &blockid_fc.nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    check_rc( rc, job_deleted_fn );
    return rc;
}

int rtdbview_t::bp_state_change( const string *bp_position, char newstatus, char oldstatus )
{
    fixed_char_t bp_position_fc( bp_position, BPSIZE );
    char_code_t newstatus_cc( newstatus );
    char_code_t oldstatus_cc( oldstatus );

    sqlint64 newseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 );
    sqlint64 oldseqid( 0 );
    SQLUDF_NULLIND oldseqid_nullind( 0 );

    SQL_API_RC rc = (*bp_state_change_fp)( bp_position_fc.buf, newstatus_cc.buf, oldstatus_cc.buf,
    &newseqid, &oldseqid,
    &bp_position_fc.nullind, &newstatus_cc.nullind, &oldstatus_cc.nullind,
    &newseqid_nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    check_rc( rc, bp_state_change_fn );
    return rc;
}

int rtdbview_t::nodecard_state_change( const string *nodecard_position, const string *bp_position,
                                       char newstatus, char oldstatus )
{
    fixed_char_t nodecard_position_fc( nodecard_position, NODECARDSIZE );
    fixed_char_t bp_position_fc( bp_position, BPSIZE );
    char_code_t newstatus_cc( newstatus );
    char_code_t oldstatus_cc( oldstatus );

    sqlint64 newseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 );
    sqlint64 oldseqid( 0 );
    SQLUDF_NULLIND oldseqid_nullind( 0 );

    SQL_API_RC rc = (*nodecard_state_change_fp)( nodecard_position_fc.buf, bp_position_fc.buf,
    newstatus_cc.buf, oldstatus_cc.buf,
    &newseqid, &oldseqid,
    &nodecard_position_fc.nullind, &bp_position_fc.nullind,
    &newstatus_cc.nullind, &oldstatus_cc.nullind,
    &newseqid_nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    check_rc( rc, nodecard_state_change_fn );
    return rc;
}

int rtdbview_t::switch_state_change( const string *switchid, const string *bp_position, char newstatus, char oldstatus )
{
    char empty_str[] = "";

    char *switchid_param( strdup( 0 == switchid ? empty_str : switchid->c_str() ) );

    SQLUDF_NULLIND switchid_nullind( 0 == switchid ? -1 : 0 );

    fixed_char_t bp_position_fc( bp_position, BPSIZE );
    char_code_t newstatus_cc( newstatus );
    char_code_t oldstatus_cc( oldstatus );

    sqlint64 newseqid( 0 );
    SQLUDF_NULLIND newseqid_nullind( 0 );
    sqlint64 oldseqid( 0 );
    SQLUDF_NULLIND oldseqid_nullind( 0 );

    SQL_API_RC rc = (*switch_state_change_fp)( switchid_param, bp_position_fc.buf,
    newstatus_cc.buf, oldstatus_cc.buf,
    &newseqid, &oldseqid,
    &switchid_nullind, &bp_position_fc.nullind,
    &newstatus_cc.nullind, &oldstatus_cc.nullind,
    &newseqid_nullind, &oldseqid_nullind,
    sqlst, qualname, specname, diagmsg );

    free( switchid_param );

    check_rc( rc, switch_state_change_fn );

    return rc;
}

rtdbview_t::~rtdbview_t()
{
    shutdown();
    dlclose( handle );
}
