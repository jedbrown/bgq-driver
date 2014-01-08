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


#include "Switch.h"

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

namespace statics {

static bgsched::Dimension::Value dimensionCodeToValue( char code )
{
    if ( code == 'A' )  { return bgsched::Dimension::A; }
    if ( code == 'B' )  { return bgsched::Dimension::B; }
    if ( code == 'C' )  { return bgsched::Dimension::C; }
    if ( code == 'D' )  { return bgsched::Dimension::D; }
    if ( code == 'E' )  { return bgsched::Dimension::E; }
    return bgsched::Dimension::InvalidDimension;
}

} // namespace statics


#pragma pack(push,1)

struct switch_record_t
{
  uint32_t switch_id_vc;
  char midplane_pos[6];
  char machine_serialnumber[19];
  char machine_serialnumber_nullind;
  char dimension;
  char status;
  SQLBIGINT seq_id;
  char switch_id[8]; // varchar data is at the end.
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr Switch::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    if ( operation_type != OperationType::UPDATE ) {
        LOG_TRACE_MSG( "Non-update to switch ignored" );
        return NO_CHANGE;
    }



    const switch_record_t *old_switch_record_p(
        reinterpret_cast<const switch_record_t*>(old_user_data_buffer)
      );

    char old_status(old_switch_record_p->status);
    bgsched::SequenceId old_seq_id(old_switch_record_p->seq_id);

    const switch_record_t *new_switch_record_p(
        reinterpret_cast<const switch_record_t*>(user_data_buffer)
      );

    char new_status(new_switch_record_p->status);
    bgsched::SequenceId new_seq_id(new_switch_record_p->seq_id);

    if ( (old_status == new_status) && (old_seq_id == new_seq_id) ) {
        LOG_TRACE_MSG( "Change to midplane ignored" );
        return NO_CHANGE;
    }

    string midplane_location(charFieldToString( old_switch_record_p->midplane_pos, sizeof ( old_switch_record_p->midplane_pos ) ));

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListener::SwitchStateChangedEventInfo::Impl(
                    midplane_location,
                    statics::dimensionCodeToValue( old_switch_record_p->dimension ),
                    hardwareStateCodeToValue( new_status ),
                    new_seq_id,
                    hardwareStateCodeToValue( old_status ),
                    old_seq_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
