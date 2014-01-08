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


#include "Midplane.h"

#include "../utility.h"

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

struct midplane_record_t
{
  char serialnumber[19];
  char serialnumber_nullind;
  char productid[16];
  char machine_serialnumber[19];
  char machine_serialnumber_nullind;
  char pos_in_machine[6];
  char status;
  char is_master;
  uint32_t vpd_vc;
  char vpd_nullind;
  SQLBIGINT seq_id;
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr Midplane::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    if ( operation_type != OperationType::UPDATE ) {
        LOG_TRACE_MSG( "Non-update to midplane ignored" );
        return NO_CHANGE;
    }

    const midplane_record_t *old_mp_record_p(
        reinterpret_cast<const midplane_record_t*>(old_user_data_buffer)
      );

    char old_status(old_mp_record_p->status);
    bgsched::SequenceId old_seq_id(old_mp_record_p->seq_id);

    const midplane_record_t *new_mp_record_p(
        reinterpret_cast<const midplane_record_t*>(user_data_buffer)
      );

    char new_status(new_mp_record_p->status);
    bgsched::SequenceId new_seq_id(new_mp_record_p->seq_id);

    if ( (old_status == new_status) && (old_seq_id == new_seq_id) ) {
        LOG_TRACE_MSG( "Change to midplane ignored" );
        return NO_CHANGE;
    }

    string bp_pos(charFieldToString( old_mp_record_p->pos_in_machine, sizeof ( old_mp_record_p->pos_in_machine ) ));

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListener::MidplaneStateChangedEventInfo::Impl(
                    bp_pos,
                    hardwareStateCodeToValue( new_status ),
                    new_seq_id,
                    hardwareStateCodeToValue( old_status ),
                    old_seq_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
