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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "EventLog.h"

#include "bgsched/realtime/ClientEventListenerImpl.h"

#include <utility/include/Log.h>

#include <sqlcli.h>

#include <map>


using std::map;
using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {
namespace db2 {
namespace table {


namespace statics {

typedef map<string,bgsched::realtime::RasSeverity::Value> db_str_to_ras_severity_t;

db_str_to_ras_severity_t build_db_str_to_ras_severity()
{
    db_str_to_ras_severity_t ret;

    ret[string("FATAL")] = bgsched::realtime::RasSeverity::FATAL;
    ret[string("WARN")] = bgsched::realtime::RasSeverity::WARN;
    ret[string("INFO")] = bgsched::realtime::RasSeverity::INFO;
    ret[string("UNKNOWN")] = bgsched::realtime::RasSeverity::UNKNOWN;

    return ret;
}

const db_str_to_ras_severity_t DB_STR_TO_RAS_SEVERITY = build_db_str_to_ras_severity();

bgsched::realtime::RasSeverity::Value ras_severity_convert_db_to_enum( const string* str_p )
{
    if ( ! str_p )  return bgsched::realtime::RasSeverity::UNKNOWN;
    db_str_to_ras_severity_t::const_iterator i(DB_STR_TO_RAS_SEVERITY.find( *str_p ));
    if ( i == DB_STR_TO_RAS_SEVERITY.end() ) {
        // didn't find it.
        return bgsched::realtime::RasSeverity::UNKNOWN;
    }
    return i->second;
}

} // namespace statics


#pragma pack(push,1)

struct event_log_record_t
{
    SQLINTEGER recid;
    char msg_id[8];
    char msg_id_nullind;
    char category[16];
    char category_nullind;
    char component[16];
    char component_nullind;
    char severity[8];
    char severity_nullind;
    char event_time[10];
    SQLBIGINT jobid;
    char jobid_nullind;
    char block[32];
    char block_nullind;
    char location[64];
    char location_nullind;
    char serialnumber[19];
    char serialnumber_nullind;
    char ecid[32];
    char ecid_nullind;
    SQLINTEGER cpu;
    char cpu_nullind;
    SQLINTEGER count;
    char count_nullind;
    char ctlaction[32];
    char ctlaction_nullind;
    // Ignore Message, RawData, Diags.
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr EventLog::handleRecord(
        const char* user_data_buffer,
        const char* ,//old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    if ( operation_type != OperationType::INSERT ) {
        LOG_TRACE_MSG( "Non-insert to eventLog ignored" );
        return NO_CHANGE;
    }


    const event_log_record_t *event_log_rec_p(
            reinterpret_cast<const event_log_record_t*>(user_data_buffer)
        );

    if ( event_log_rec_p->msg_id_nullind ) {
        LOG_DEBUG_MSG( "ignoring event log entry with NULL message ID" );
        return NO_CHANGE;
    }

    string msg_id(charFieldToString( event_log_rec_p->msg_id, sizeof ( event_log_rec_p->msg_id ) ));

    bgsched::realtime::RasSeverity::Value severity(bgsched::realtime::RasSeverity::UNKNOWN);
    if ( ! event_log_rec_p->severity_nullind ) {
        string severity_str(charFieldToString( event_log_rec_p->severity, sizeof ( event_log_rec_p->severity ) ));
        severity = statics::ras_severity_convert_db_to_enum( &severity_str );
    }

    bgsched::Job::Id db_job_id(bgsched::Job::Id(-1));
    if ( ! event_log_rec_p->jobid_nullind ) {
        db_job_id = event_log_rec_p->jobid;
    }

    string block_id;
    if ( ! event_log_rec_p->block_nullind ) {
        block_id = event_log_rec_p->block;
    }

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListener::RasEventInfo::Impl(
                    event_log_rec_p->recid,
                    msg_id,
                    severity,
                    block_id,
                    db_job_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
