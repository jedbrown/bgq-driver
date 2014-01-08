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


#include "Cable.h"

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

struct cable_record_t
{
    char from_location[16];
    char to_location[16];
    char status;
    char create_date[10]; // timestamp
    SQLBIGINT seq_id;
};

#pragma pack(pop)


bgsched::realtime::AbstractDatabaseChange::Ptr Cable::handleRecord(
        const char* user_data_buffer,
        const char* old_user_data_buffer,
        OperationType::Values operation_type
    )
{
    if ( operation_type != OperationType::UPDATE ) {
        LOG_TRACE_MSG( "Non-update to cable ignored" );
        return NO_CHANGE;
    }

    const cable_record_t *old_cable_record_p(
            reinterpret_cast<const cable_record_t*>(old_user_data_buffer)
        );

    char old_status(old_cable_record_p->status);
    bgsched::SequenceId old_seq_id(old_cable_record_p->seq_id);

    const cable_record_t *new_cable_record_p(
            reinterpret_cast<const cable_record_t*>(user_data_buffer)
        );

    char new_status(new_cable_record_p->status);
    bgsched::SequenceId new_seq_id(new_cable_record_p->seq_id);

    if ( (old_status == new_status) && (old_seq_id == new_seq_id) ) {
        return NO_CHANGE;
    }

    string from_location(charFieldToString( old_cable_record_p->from_location, sizeof ( old_cable_record_p->from_location ) ));
    string to_location(charFieldToString( old_cable_record_p->to_location, sizeof ( old_cable_record_p->to_location ) ));

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr;

    // If to_location is like R00-M0-N00-T04 then it's a torus cable. Just check that 5th char is M. An I/O cable would be like R00-ID-T04 or Q00-I0-T04.
    if ( to_location[4] == 'M' ) {
        db_change_ptr = bgsched::realtime::AbstractDatabaseChange::Ptr(
                new bgsched::realtime::ClientEventListener::TorusCableStateChangedEventInfo::Impl(
                        from_location,
                        to_location,
                        hardwareStateCodeToValue( new_status ),
                        new_seq_id,
                        hardwareStateCodeToValue( old_status ),
                        old_seq_id
                    )
            );
    } else {
        db_change_ptr = bgsched::realtime::AbstractDatabaseChange::Ptr(
                new bgsched::realtime::ClientEventListener::IoCableStateChangedEventInfo::Impl(
                        from_location,
                        to_location,
                        hardwareStateCodeToValue( new_status ),
                        new_seq_id,
                        hardwareStateCodeToValue( old_status ),
                        old_seq_id
                    )
            );
    }

    return db_change_ptr;
}


} } } } // namespace realtime::server::db2::table
