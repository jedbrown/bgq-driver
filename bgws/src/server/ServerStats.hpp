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

#ifndef BGWS_SERVER_STATS_HPP_
#define BGWS_SERVER_STATS_HPP_


#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <stdint.h>


namespace bgws {


class ServerStats : boost::noncopyable
{
public:

    struct Snapshot {
        uint64_t total_requests;
        uint64_t requests_complete;
        uint64_t max_time_to_process_request_microseconds;
        uint64_t total_time_to_process_requests_microseconds;
    };


    ServerStats(
            boost::asio::io_service& io_service
        );

    void notifyNewRequest();

    void notifyRequestComplete(
            const boost::posix_time::time_duration& time_to_process_request
        );


    typedef boost::function<void ( const Snapshot& )> SnapshotCbFn;

    void getSnapshot( SnapshotCbFn cb_fn );


private:

    boost::asio::strand _strand;

    Snapshot _snapshot;


    void _notifyNewRequestImpl();

    void _notifyRequestCompleteImpl(
            const boost::posix_time::time_duration& time_to_process_request
        );

    void _getSnapshotImpl(
            SnapshotCbFn cb_fn
        );
};

} // namespace bgws

#endif
