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
#ifndef RUNJOB_SERVER_REALTIME_CONNECTION_H
#define RUNJOB_SERVER_REALTIME_CONNECTION_H

#include "server/realtime/EventHandler.h"
#include "server/fwd.h"

#include <hlcs/include/bgsched/realtime/Client.h>
#include <hlcs/include/bgsched/types.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {
namespace realtime {

/*!
 * \brief Establishes a connection to the real-time server and handles events.
 *
 * The EventHandler will initiate block reconnection to get a sequence ID, after
 * which the connection to the real-time server will be created.
 *
 * In the event the real-time server is not available (not configured, or not
 * running for some reason) this class will fall back to using a Polling
 * implementation for getting block status events.
 *
 * \see EventHandler
 * \see Polling
 * \see block::Reconnect
 */
class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Connection> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Connection();

    /*!
     * \brief
     */
    bool status();

private:
    Connection(
            const boost::shared_ptr<Server>& server
            );

    void setFilter();

    void connect();

    void read();
    
    void write();

    void handleRead(
            const boost::system::error_code& error
            );

    void poll();
    
private:
    const boost::weak_ptr<Server> _server;
    bgsched::realtime::Client _client;
    unsigned _connectTimeout;
    boost::asio::deadline_timer _timer;
    boost::scoped_ptr<boost::asio::posix::stream_descriptor> _descriptor;
    bgsched::SequenceId _sequence;
    EventHandler _handler;
    bool _connected;
};

} // realtime
} // server
} // runjob

#endif
