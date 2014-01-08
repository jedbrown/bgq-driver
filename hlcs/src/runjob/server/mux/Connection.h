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
#ifndef RUNJOB_SERVER_MUX_CONNECTION_H_
#define RUNJOB_SERVER_MUX_CONNECTION_H_

#include "common/message/Header.h"

#include "server/performance/Counters.h"

#include "common/Message.h"

#include "server/Connection.h"
#include "server/Server.h"

#include <deque>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace message {

class AddJob;
class KillJob;
class StdIo;

} // message

namespace server {

class ConnectionContainer;

namespace mux {

/*!
 * \brief connection to runjob_mux.
 */
class Connection : public runjob::server::Connection, public boost::enable_shared_from_this<Connection>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Connection> Ptr;

    /*!
     * \brief weak pointer type.
     */
    typedef boost::weak_ptr<Connection> WeakPtr;
 
    /*!
     * \brief Callback for the write method.
     */
    typedef boost::function<void ()> WriteCallback;

public:
    /*!
     * \brief ctor.
     */
    Connection(
            const bgq::utility::portConfig::SocketPtr& socket,  //!< [in]
            const Server::Ptr& server                           //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Connection();

    /*!
     * \brief Start asynchronously writing a message to this mux.
     */
    void write(
            const runjob::Message::Ptr& msg,                //!< [in]
            const WriteCallback& callback = WriteCallback() //!< [in]
            );

    /*!
     * \brief Get client container.
     */
    const boost::shared_ptr<ClientContainer>& clients() { return _clients; }

private:
    runjob::server::Connection::Ptr getShared() { return this->shared_from_this(); }
    
    void writeImpl(
            const runjob::Message::Ptr& msg,
            const WriteCallback& callback
            );

    void write();

    void readHeader();

    void handle(
            const runjob::Message::Ptr& msg
            );

    void readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );
    
    void readBodyHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

    void writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

private:
    typedef std::pair<runjob::Message::Ptr,WriteCallback> MessagePair;
    typedef std::deque<MessagePair> MessageQueue;

private:
    boost::asio::strand _strand;                            //!<
    message::Header _incomingHeader;                        //!<
    message::Header _outgoingHeader;                        //!<
    MessageQueue _outbox;                                   //!<
    boost::shared_ptr<ClientContainer> _clients;            //!<
    bool _handshakeComplete;                                //!<
};

} // mux
} // server
} // runjob

#endif
