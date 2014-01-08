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
#ifndef RUNJOB_MUX_SERVER_CONNECTION_H_
#define RUNJOB_MUX_SERVER_CONNECTION_H_

#include "common/message/Header.h"

#include "mux/client/fwd.h"

#include "mux/server/Timer.h"

#include "common/commands/MuxStatus.h"

#include "common/error.h"

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <deque>
#include <vector>

namespace runjob {

class Message;

namespace mux {
namespace server {

/*!
 * \brief A connection to the runjob_server.
 */
class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Connection> Ptr;
    
    /*!
     * \brief Callback for the write method.
     */
    typedef boost::function<
        void (
                const boost::system::error_code&
             )
        > WriteCallback;

    /*!
     * \brief Callback for the write method.
     */
    typedef boost::function<
        void (
                const runjob::commands::response::MuxStatus::Ptr&
             )
        > StatusCallback;

public:
    /*!
     * \brief ctor.
     *
     * Create a new server connection and start the timer to connect
     */
    Connection(
            boost::asio::io_service& service,                   //!< [in]
            const Options& options,                             //!< [in]
            const boost::shared_ptr<client::Container>& clients //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Connection();

    /*!
     * \brief start connecting to the server.
     */
    void start();

    /*!
     * \brief Start an asynchronous write to the runjob_server.
     */
    void write(
            const boost::shared_ptr<runjob::Message>& msg,     //!< [in]
            const WriteCallback& callback = WriteCallback()    //!< [in]
            );

    /*!
     * \brief Get status information.
     */
    void status(
            const runjob::commands::response::MuxStatus::Ptr& response, //!< [in]
            const StatusCallback& callback                              //!< [in]
            );

private:
    typedef boost::shared_ptr<runjob::Message> MessagePtr;
    typedef std::pair<MessagePtr,WriteCallback> MessagePair;
    typedef std::deque<MessagePair> MessageQueue;

private:
    void connectHandler(
            const Timer::SocketPtr& socket,
            const boost::system::error_code& error
            );

    void writeImpl(
            const boost::shared_ptr<runjob::Message>& msg,
            const WriteCallback& callback
            );

    void write();

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

    void findHandler(
            const boost::shared_ptr<runjob::Message>& msg,
            const boost::shared_ptr<client::Runjob>& client
            );

    void statusImpl(
            const runjob::commands::response::MuxStatus::Ptr& response,
            const StatusCallback& callback
            );

    void handleEof();

    void handshakeHandler(
            error_code::rc error,
            const Timer::SocketPtr& socket
            );

    void keepAlive(
            const Timer::SocketPtr& socket
            );
private:
    boost::asio::strand _strand;                                //!< protects _socket and _outbox
    Timer::SocketPtr _socket;                                   //!<
    boost::shared_ptr<Timer> _timer;                            //!<
    message::Header _incomingHeader;                            //!<
    boost::asio::streambuf _incomingMessage;                    //!<
    message::Header _outgoingHeader;                            //!<
    boost::asio::streambuf _outgoingMessage;                    //!<
    MessageQueue _outbox;                                       //!<
    const Options& _options;                                    //!<
    boost::shared_ptr<client::Container> _clients;              //!<
};

} // server
} // mux
} // runjob

#endif
