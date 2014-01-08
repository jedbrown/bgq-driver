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
#ifndef RUNJOB_SERVER_CIOS_AUTHENTICATE_H
#define RUNJOB_SERVER_CIOS_AUTHENTICATE_H

#include "server/block/fwd.h"

#include "server/cios/ConnectionInterval.h"
#include "server/cios/Message.h"

#include "server/fwd.h"

#include "common/Uci.h"

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {
namespace cios {

/*!
 * \brief
 */
class Authenticate : public boost::enable_shared_from_this<Authenticate>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Authenticate> Ptr;

public:
    /*!
     * \brief
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server,    //!< [in]
            const uint8_t service,                      //!< [in]
            const Uci& location                         //!< [in]
            );

    /*!
     * \brief
     */
    void start(
            const ConnectionInterval::SocketPtr& socket,    //!< [in]
            const block::SecurityKey& securityKey,          //!< [in]
            const ConnectionInterval::Callback& callback    //!< [in]
            );

    /*!
     * \brief Stop authenticating.
     */
    void stop();

    /*!
     * \brief dtor.
     */
    ~Authenticate();

private:
    Authenticate(
            const boost::shared_ptr<Server>& server,
            const uint8_t service,
            const Uci& location
            );

    void writeHandler(
            const boost::system::error_code& error
            );

    void readHeaderHandler(
            const boost::system::error_code& error
            );
   
    void readDataHandler(
            const boost::system::error_code& error
            );

    void timerHandler(
            const boost::system::error_code& error
            );

    void stopImpl();

    int getTimeout() const;

private:
    const boost::weak_ptr<Server> _server;
    const Options& _options;
    boost::asio::strand _strand;
    boost::asio::deadline_timer _timer;
    const uint8_t _service;
    const Uci _location;
    ConnectionInterval::SocketPtr _socket;
    ConnectionInterval::Callback _callback;
    Message::Ptr _request;
    bgcios::MessageHeader _header;
    const boost::shared_ptr<cios::Message> _ack;
    bool _timedOut;
};

} // cios
} // server
} // runjob

#endif

