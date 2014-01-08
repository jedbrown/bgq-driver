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

#ifndef REALTIME_SERVER_STATUS_H_
#define REALTIME_SERVER_STATUS_H_

#include "DbChangesMonitor.h"

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <string>

namespace realtime {
namespace server {

class Status
{
public:

    struct Details {
        int connected_clients;
        DbChangesMonitor::State::Value db_changes_monitor_state;
        std::string db2_version;
    };

    typedef boost::function<void ( Details details )> StatusCallback;

    Status( boost::asio::io_service& io_service );

    void clientConnected();
    void clientDisconnected();

    void setDbMonitorState(
            DbChangesMonitor::State::Value db_changes_monitor_state
        );

    void requestStatus(
            StatusCallback status_cb
        );

private:

    boost::asio::strand _strand;

    Details _details;

    void _clientConnectedImpl();
    void _clientDisconnectedImpl();

    void _setDbMonitorStateImpl(
            DbChangesMonitor::State::Value db_changes_monitor_state
        );

    void _requestStatusImpl(
            StatusCallback status_cb
        );

};

} // namespace realtime::server
} // namespace realtime

#endif
