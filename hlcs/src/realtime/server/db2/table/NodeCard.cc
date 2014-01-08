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


#include "NodeCard.h"

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

struct node_card_record_t
{
  char serialnumber[19];
  char serialnumber_nullind;
  char product_id[16];
  char midplane_pos[6];
  char position[3];
  char status;
  uint32_t vpd_vc;
  char vpd_nullind;
  SQLBIGINT seq_id;
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr NodeCard::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    if ( operation_type != OperationType::UPDATE ) {
        LOG_TRACE_MSG( "Non-update to NodeCard ignored" );
        return NO_CHANGE;
    }


    const node_card_record_t *old_nc_record_p(
        reinterpret_cast<const node_card_record_t*>(old_user_data_buffer)
      );

    char old_status(old_nc_record_p->status);
    bgsched::SequenceId old_seq_id(old_nc_record_p->seq_id);

    const node_card_record_t *new_nc_record_p(
        reinterpret_cast<const node_card_record_t*>(user_data_buffer)
      );

    char new_status(new_nc_record_p->status);
    bgsched::SequenceId new_seq_id(new_nc_record_p->seq_id);

    if ( old_status == new_status && old_seq_id == new_seq_id ) {
        LOG_TRACE_MSG( "Change to node card ignored" );
        return NO_CHANGE;
    }

    string midplane_location(charFieldToString( old_nc_record_p->midplane_pos, sizeof ( old_nc_record_p->midplane_pos ) ));
    string node_board_position(charFieldToString( old_nc_record_p->position, sizeof ( old_nc_record_p->position ) ));

    string node_board_location(midplane_location + "-" + node_board_position);

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListener::NodeBoardStateChangedEventInfo::Impl(
                    node_board_location,
                    hardwareStateCodeToValue( new_status ),
                    new_seq_id,
                    hardwareStateCodeToValue( old_status ),
                    old_seq_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
