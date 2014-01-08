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

#ifndef REALTIME_SERVER_CLIENT_H_
#define REALTIME_SERVER_CLIENT_H_


#include "types.h"

#include "ClientFilter.h"

#include "bgsched/realtime/AbstractMessage.h"

#include <utility/include/portConfiguration/types.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio/ssl.hpp>

#include <deque>


namespace realtime {
namespace server {


class DbChangesMonitor;
class Status;


    /*! \brief A connected real-time client. */
class Client : public boost::enable_shared_from_this<Client>
{
private:

    typedef bgq::utility::portConfig::Socket _Socket;
    typedef bgq::utility::portConfig::SocketPtr _SocketPtr;

public:

    typedef boost::shared_ptr<Client> Ptr;

    static const unsigned int MAX_MSGS = 500; //!< The maximum size of the out msgs

    Client(
            _SocketPtr socket_ptr,
            DbChangesMonitor& db_changes_monitor,
            Status& status
        );

    void start();

    void notifyConnectedToDbMonitor( bool is_connected );

    void notifyDbChanges(
            const DbChanges& db_changes
        );

    ~Client();

private:

    typedef std::deque<bgsched::realtime::AbstractMessage::ConstPtr> _OutMsgs;

    _SocketPtr _socket_ptr;
    DbChangesMonitor &_db_changes_monitor;
    Status &_status;

    std::string _id;

    boost::asio::io_service::strand _strand;

    bool _started;
    bool _shutdown_ind;

    boost::asio::streambuf _in_sb;

    _OutMsgs _out_msgs;

    std::string _to_write;

    bool _waiting_connect_to_db_monitor_result;
    bool _connected_to_db_monitor;

    ClientFilter _filter;
    bgsched::realtime::Filter::Id _filter_id;


    void _notifyConnectedToDbMonitorImpl( bool is_connected );

    void _notifyDbChangesImpl(
            const DbChanges& db_changes
        );

    void _startRead();

    void _readComplete( const boost::system::error_code& err, size_t bytes_transferred );

    void _handleMessage( const bgsched::realtime::AbstractMessage& msg );

    void _startWriting();

    void _writeComplete( const boost::system::error_code& err, size_t bytes_transferred );

    void _shutdown();
};


/*! \page realtimeClients Real-time clients
 *

Real-time clients connect to the real-time port and request real-time updates.

The default port for real-time clients is <b>32061</b>. By default the %server listens on <b>localhost only</b>.

The %server uses SSL to secure connections.
If the SSL context can't be initialized then
the %server will not start.
The %server presents its certificate to the
client.
The %server requires that the client present
the administrative certificate.

The %server doesn't limit the number of clients that can be
connected at once.

To protect the %server from
allocating too much memory,
a client will be disconnected if it falls too far behind.
A client may fall behind if it's not reading messages often enough,
either because of a fault in the program
that causes it to not read messages or
because the system is overloaded.
This is rather unlikely to occur as long
as the client is written correctly because
of the amount of buffering that's done.
The client will be disconnected if it has <b>500</b> outstanding messages.

\section client_modes Client states

A client can be either
<i>connected</i> to the DBM or
<i>disconnected</i> from the DBM.
The client starts out <i>disconnected</i>.

When a client is <i>disconnected</i>,
it will not get notifications from the DBM.
The client goes to <i>connected</i>
when the %server receives a <i>start real-time</i> message
from the client.
A client can also go to <i>connected</i>
automatically if it was disconnected because the
DBM exited <i>large-transaction</i> mode.

When a client is <i>connected</i>,
the client will get notifications of changes to
the database tables.
The %server applies the current filter to each change
and then sends the messages
that pass the filter
on to the client.
The DBM will disconnect a client when
it enters <i>large-transaction</i> mode.
The DBM will automatically re-connect
the clients when it exits <i>large-transaction</i> mode.

 */

} } // namespace realtime::server


#endif
