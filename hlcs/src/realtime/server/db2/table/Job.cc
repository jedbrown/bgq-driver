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


#include "Job.h"

#include "../utility.h"

#include "bgsched/JobImpl.h"

#include "bgsched/realtime/ClientEventListenerImpl.h"

#include <utility/include/Log.h>

#include <sqlcli.h>


using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {
namespace db2 {
namespace table {


#pragma pack(push,1)

struct job_record_t
{
  SQLBIGINT id;
  char username[32];
  char block_id[32];
  uint32_t executable_vc;
  uint32_t workingdir_vc;
  char starttime[10];
  char status;
  char status_last_modified[10];
  SQLBIGINT seq_id;
  uint32_t args_vc;
  uint32_t envs_vc;
  uint32_t mapping_vc;
  int32_t nodes_used;
  int32_t shape_a;
  int32_t shape_b;
  int32_t shape_c;
  int32_t shape_d;
  int32_t shape_e;
  int32_t processes_per_node;
  uint32_t scheduler_data_vc;
  char scheduler_data_nullind;
  char corner[18];
  char corner_nullind;
  uint32_t hostname_vc;
  int32_t pid;
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr Job::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    const job_record_t *job_record_p(
        reinterpret_cast<const job_record_t*>(user_data_buffer)
      );

    bgsched::Job::Id job_id(job_record_p->id);
    string block_id(charFieldToString(
        job_record_p->block_id, sizeof ( job_record_p->block_id )
      ));
    char status_char(job_record_p->status);
    bgsched::SequenceId seq_id(job_record_p->seq_id);

    if ( operation_type == OperationType::INSERT ) {

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::JobAddedEventInfo::Impl(
                        job_id,
                        block_id,
                        bgsched::Job::Impl::statusDbCharToValue( status_char ),
                        seq_id
                    )
            );

        return db_change_ptr;

    }

    if ( operation_type == OperationType::DELETE ) {

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::JobDeletedEventInfo::Impl(
                        job_id,
                        block_id,
                        seq_id
                    )
            );

        return db_change_ptr;
    }


    // UPDATE

    const job_record_t *old_job_record_p(
        reinterpret_cast<const job_record_t*>(old_user_data_buffer)
      );


    char old_status(old_job_record_p->status);
    bgsched::SequenceId old_seq_id(old_job_record_p->seq_id);

    if ( (status_char == old_status) &&
         (seq_id == old_seq_id) ) {
        LOG_TRACE_MSG( "Ignoring change to job" );
        return bgsched::realtime::AbstractDatabaseChange::Ptr();
    }


    LOG_TRACE_MSG(
        "updated job " << job_id << " block '" << block_id << "':" <<
        " old_status=" << old_status << " new_status=" << status_char <<
        " old_seqId=" << old_seq_id << " new_seqId=" << seq_id
      );

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListener::JobStateChangedEventInfo::Impl(
                    job_id, block_id,
                    bgsched::Job::Impl::statusDbCharToValue( status_char ),
                    seq_id,
                    bgsched::Job::Impl::statusDbCharToValue( old_status ),
                    old_seq_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
