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


#include "Block.h"

#include "bgsched/BlockImpl.h"

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

struct block_record_t
{
  char block_id[32];
  int32_t num_c_nodes;
  int32_t num_io_nodes;
  char owner[32];
  char user_name[32];
  char user_name_nullind;
  char is_torus[5];
  char is_torus_nullind;
  int32_t size_a;
  int32_t size_b;
  int32_t size_c;
  int32_t size_d;
  int32_t size_e;
  int32_t description_vc;
  char description_nullind;
  char options[16];
  char status;
  char action;
  char status_last_modified[10];
  char status_last_mofified_nullind;
  uint32_t mloader_img_vc;
  char mloader_img_nullind;
  char node_config[32];
  uint32_t boot_options_vc;
  char boot_options_nullind;
  char create_date[10];
  uint8_t security_key[32];
  char security_key_nullind;
  uint32_t errtext_vc;
  char errtext_nullind;
  SQLBIGINT seq_id;
  uint32_t creation_id;
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr Block::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    const block_record_t *block_record_p(
            reinterpret_cast<const block_record_t*>(user_data_buffer)
        );

    string block_id(charFieldToString(
            block_record_p->block_id, sizeof ( block_record_p->block_id )
        ));

    char status_char(block_record_p->status);
    bgsched::SequenceId seq_id(block_record_p->seq_id);

    if ( OperationType::INSERT == operation_type ) {
        // Block insert.

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::BlockAddedEventInfo::Impl(
                        block_id,
                        bgsched::Block::Impl::statusDbCharToValue( status_char ),
                        seq_id
                    )
            );

        return db_change_ptr;
    }

    if ( OperationType::DELETE == operation_type ) {
        // Block delete.

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::BlockDeletedEventInfo::Impl(
                        block_id,
                        seq_id
                    )
            );

        return db_change_ptr;
    }


    // Block update.

    const block_record_t *old_block_record_p(
        reinterpret_cast<const block_record_t*>(old_user_data_buffer)
      );

    char old_status_char(old_block_record_p->status);
    bgsched::SequenceId old_seq_id(old_block_record_p->seq_id);


    if ( (status_char == old_status_char) && (seq_id == old_seq_id) ) {
        LOG_TRACE_MSG( "Ignoring block update change:" << old_status_char << "(" << old_seq_id << ") -> " << status_char << "(" << seq_id << ")"  );

        return NO_CHANGE;
    }

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListener::BlockStateChangedEventInfo::Impl(
                    block_id,
                    bgsched::Block::Impl::statusDbCharToValue( status_char ),
                    seq_id,
                    bgsched::Block::Impl::statusDbCharToValue( old_status_char ),
                    old_seq_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
