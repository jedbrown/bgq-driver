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
#ifndef RUNJOB_CLIENT_MUX_CONNECTION_H_
#define RUNJOB_CLIENT_MUX_CONNECTION_H_

#include "common/message/Header.h"

#include "common/Message.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include <deque>
#include <string>

namespace runjob {
namespace client {

class Job;

namespace options {

class Parser;

} // options

/*!
 * \brief connection to runjob multiplexer.
 */
class MuxConnection : public boost::enable_shared_from_this<MuxConnection>
{
public:
    /*!
     * \brief
     */
    typedef boost::function<void(const boost::system::error_code&)> Callback;

public:
    /*!
     * \brief ctor.
     */
    MuxConnection(
            boost::asio::io_service& io_service,                    //!< [in] asio service
            const boost::shared_ptr<Job>& job,                      //!< [in] job object
            const boost::shared_ptr<const options::Parser>& options //!< [in] program options
            );

    /*!
     * \brief dtor.
     */
    ~MuxConnection();

    /*!
     * \brief Start handling the mux connection.
     */
    void start(
            const Callback& callback
            );

    /*!
     * \brief send a message.
     */
    void write(
            const Message::Ptr& msg        //!< [in] message to send
            );

private:
    void __attribute__ ((visibility("hidden"))) writeImpl(
            const Message::Ptr& msg
            );

    void __attribute__ ((visibility("hidden"))) connectHandler(
            const Callback& callback,
            const boost::system::error_code& error
            );

    void __attribute__ ((visibility("hidden"))) readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );
    
    void __attribute__ ((visibility("hidden"))) readBodyHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );

    void __attribute__ ((visibility("hidden"))) writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
            );
private:
    boost::shared_ptr<const options::Parser> _options;      //!<
    boost::shared_ptr<Job> _job;                            //!<
    boost::asio::local::stream_protocol::socket _socket;    //!<
    boost::asio::io_service& _io_service;                   //!<
    message::Header _incomingHeader;                        //!<
    boost::asio::streambuf _incomingMessage;                //!<
    std::deque<Message::Ptr> _outbox;                       //!<
    message::Header _outgoingHeader;                        //!<
    boost::asio::streambuf _outgoingMessage;                //!<
};

} // client
} // runjob

#endif
