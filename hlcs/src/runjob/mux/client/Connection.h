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
#ifndef RUNJOB_MUX_CLIENT_CONNECTION_H
#define RUNJOB_MUX_CLIENT_CONNECTION_H

#include "common/message/Header.h"

#include "mux/client/fwd.h"
#include "mux/client/Id.h"
#include "mux/client/MessageQueue.h"

#include "common/commands/MuxStatus.h"

#include "common/Message.h"

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <deque>

namespace runjob {

class Message;

namespace mux {
namespace client {

/*!
 * \brief Connection to a runjob client.
 */ 
class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    /*!
     * \brief Callback type for handling a message.
     */
    typedef boost::function<
            void (
                    const boost::shared_ptr<runjob::Message>&,
                    const boost::system::error_code&
                 )
            > MessageHandler;

    /*!
     * \brief Callback type for status requests.
     */
    typedef boost::function<void (const runjob::commands::response::MuxStatus::Ptr&)> StatusCallback;

    /*!
     * \brief Socket type.
     */
    typedef boost::asio::local::stream_protocol::socket Socket;

public:
    /*!
     * \brief ctor.
     */
    Connection(
            const boost::shared_ptr<Multiplexer>& mux,  //!< [in]
            const Id& id                                //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Connection();

    /*!
     * \brief Start handling requests from this client.
     */
    void start(
            const MessageHandler& handler  //!< [in]
            );

    /*!
     * \brief Stop handling requests from this client.
     */
    void stop();
    
    /*!
     * \brief Get socket object.
     */
    Socket& getSocket() { return _socket; }

    /*!
     * \brief Start an asynchronous write of a message.
     */
    void write(
            const boost::shared_ptr<runjob::Message>& message,      //!< [in]
            const Status& status                                    //!< [in]
            );

    /*!
     * \brief Add our status to the response.
     */
    void status(
            const runjob::commands::response::MuxStatus::Client& client,    //!< [in]
            runjob::commands::response::MuxStatus::Ptr response,            //!< [in]
            const StatusCallback& callback                                  //!< [in]
            );

private:
    void readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const MessageHandler& handler
            );
    
    void readBodyHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred,
            const MessageHandler& handler
            );
    
    void writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

    void writeImpl(
            const boost::shared_ptr<runjob::Message>& message,
            const Status& status
            );

    void statusImpl(
            runjob::commands::response::MuxStatus::Client& client,
            const runjob::commands::response::MuxStatus::Ptr& response,
            const StatusCallback& callback
            );

    void write();

private:
    Socket _socket;                             //!<
    boost::asio::strand _strand;                //!<
    const Id _id;                               //!<
    message::Header _incomingHeader;            //!<
    boost::asio::streambuf _incomingMessage;    //!<
    MessageQueue _outbox;                       //!<
    bool _terminating;                          //!<
};

} // client
} // mux
} // runjob

#endif
