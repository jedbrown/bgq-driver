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

#include "Status.h"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

using boost::bind;

LOG_DECLARE_FILE( "realtime.server" );

namespace realtime {
namespace server {

Status::Status( boost::asio::io_service& io_service ) :
        _strand( io_service )
{
    _details.connected_clients = 0;
    _details.db_changes_monitor_state = DbChangesMonitor::State::Idle;
    _details.db2_version = DB2VERSION;
}

void Status::clientConnected()
{
    _strand.post( bind( &Status::_clientConnectedImpl, this ) );
}

void Status::clientDisconnected()
{
    _strand.post( bind( &Status::_clientDisconnectedImpl, this ) );
}

void Status::setDbMonitorState(
        DbChangesMonitor::State::Value db_changes_monitor_state
    )
{
    _strand.post( bind( &Status::_setDbMonitorStateImpl, this, db_changes_monitor_state ) );
}

void Status::requestStatus(
        StatusCallback status_cb
    )
{
    _strand.post( bind( &Status::_requestStatusImpl, this, status_cb ) );
}

void Status::_clientConnectedImpl()
{
    ++_details.connected_clients;

    LOG_INFO_MSG( "Notified client connected, now have " << _details.connected_clients );
}

void Status::_clientDisconnectedImpl()
{
    --_details.connected_clients;

    LOG_INFO_MSG( "Notified client disconnected, now have " << _details.connected_clients );
}

void Status::_setDbMonitorStateImpl(
        DbChangesMonitor::State::Value db_changes_monitor_state
    )
{
    _details.db_changes_monitor_state = db_changes_monitor_state;

    LOG_INFO_MSG( "DB changes monitor state is now " << _details.db_changes_monitor_state );
}

void Status::_requestStatusImpl(
        StatusCallback status_cb
    )
{
    LOG_DEBUG_MSG( "Reporting details={clients=" << _details.connected_clients << "}" );
    status_cb( _details );
}

} // namespace realtime::server
} // namespace realtime
