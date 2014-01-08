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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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


#include "ServerStats.hpp"

#include <boost/bind.hpp>


using boost::bind;


namespace bgws {


ServerStats::ServerStats(
        boost::asio::io_service& io_service
    ) :
        _strand(io_service)
{
    _snapshot.total_requests = 0;
    _snapshot.requests_complete = 0;
    _snapshot.max_time_to_process_request_microseconds = 0;
    _snapshot.total_time_to_process_requests_microseconds = 0;
}


void ServerStats::notifyNewRequest(
        void* id,
        RequestData::Ptr request_data_ptr
    )
{
    _strand.post( bind( &ServerStats::_notifyNewRequestImpl, this,
            id, request_data_ptr
        ) );
}


void ServerStats::notifyRequestComplete(
        void* id,
        const boost::posix_time::time_duration& time_to_process_request
    )
{
    _strand.post( bind( &ServerStats::_notifyRequestCompleteImpl, this,
            id,
            time_to_process_request
        ) );
}


void ServerStats::getSnapshot( SnapshotCbFn cb_fn )
{
    _strand.post( bind( &ServerStats::_getSnapshotImpl, this, cb_fn ) );
}


void ServerStats::_notifyNewRequestImpl(
        void* id,
        RequestData::Ptr request_data_ptr
    )
{
    ++_snapshot.total_requests;

    _snapshot.requests[id] = request_data_ptr;
}


void ServerStats::_notifyRequestCompleteImpl(
        void* id,
        const boost::posix_time::time_duration& time_to_process_request
    )
{
    ++_snapshot.requests_complete;

    uint64_t time_to_process_request_microseconds(time_to_process_request.total_microseconds());

    _snapshot.total_time_to_process_requests_microseconds += time_to_process_request_microseconds;

    if ( time_to_process_request_microseconds > _snapshot.max_time_to_process_request_microseconds ) {
        _snapshot.max_time_to_process_request_microseconds = time_to_process_request_microseconds;
    }

    _snapshot.requests.erase( id );
}


void ServerStats::_getSnapshotImpl(
        SnapshotCbFn cb_fn
    )
{
    Snapshot snapshot_copy(_snapshot);

    cb_fn( snapshot_copy );
}


} // namespace bgws
