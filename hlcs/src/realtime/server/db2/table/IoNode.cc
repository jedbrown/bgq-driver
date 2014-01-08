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


#include "IoNode.h"

#include "../utility.h"

#include "bgsched/realtime/ClientEventListenerV2Impl.h"

#include <utility/include/Log.h>

#include <sqlcli.h>


using std::string;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {
namespace db2 {
namespace table {


#pragma pack(push,1)

struct io_node_record_t
{
    char serialnumber[19];
    char serialnumber_nullind;
    char product_id[16];
    char product_id_nullind;
    char ecid[32];
    char ecid_nullind;
    char iopos[6];
    char position[3];
    uint32_t ip_address_vc;
    char ip_address_nullind;
    char mac_address[20];
    char mac_address_nullind;
    char status;
    SQLINTEGER memory_module_size;
    SQLINTEGER memory_size;
    SQLINTEGER psro;
    uint32_t vpd_vc;
    char vpd_nullind;
    SQLFLOAT voltage;
    char voltage_nullind;
    SQLINTEGER bitsteering;
    uint32_t faildata_vc;
    char faildata_nullind;
    SQLBIGINT seq_id;
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr IoNode::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    if ( operation_type != OperationType::UPDATE ) {
        LOG_TRACE_MSG( "Non-update to I/O node ignored" );
        return NO_CHANGE;
    }


    const io_node_record_t *old_record_p(
        reinterpret_cast<const io_node_record_t*>(old_user_data_buffer)
      );

    char old_status(old_record_p->status);
    bgsched::SequenceId old_seq_id(old_record_p->seq_id);

    const io_node_record_t *new_record_p(
        reinterpret_cast<const io_node_record_t*>(user_data_buffer)
      );

    char new_status(new_record_p->status);
    bgsched::SequenceId new_seq_id(new_record_p->seq_id);

    if ( (old_status == new_status) && (old_seq_id == new_seq_id) ) {
        LOG_TRACE_MSG( "Change to I/O node ignored" );
        return NO_CHANGE;
    }

    string io_node_location(
            charFieldToString( old_record_p->iopos, sizeof ( old_record_p->iopos ) ) + "-" +
            charFieldToString( old_record_p->position, sizeof ( old_record_p->position ) )
        );

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
            new bgsched::realtime::ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl(
                    io_node_location,
                    hardwareStateCodeToValue( new_status ),
                    new_seq_id,
                    hardwareStateCodeToValue( old_status ),
                    old_seq_id
                )
        );

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
