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
#ifndef RUNJOB_MUX_SERVER_HANDSHAKE_H
#define RUNJOB_MUX_SERVER_HANDSHAKE_H

#include "common/message/Header.h"

#include "common/error.h"

#include "mux/server/Timer.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

namespace runjob {
namespace mux {
namespace server {

/*!
 * \brief
 */
class Handshake : public boost::enable_shared_from_this<Handshake> {
public:
    /*!
     * \brief Callback type.
     */
    typedef boost::function<
        void(
                runjob::error_code::rc
            )
        > Callback;

    /*!
     * \brief Factory.
     */
    static void create(
            const Timer::SocketPtr& socket, //!< [in]
            const Callback& callback        //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Handshake();

private:
    Handshake(
            const Timer::SocketPtr& socket,
            const Callback& callback
            );

    void start();

    void writeHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
        );

    void readHeaderHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
        );

    void readBodyHandler(
            const boost::system::error_code& error,
            const size_t bytesTransferred
        );

    void timerHandler(
            const boost::system::error_code& error
            );

private:
    error_code::rc _status;
    const Timer::SocketPtr _socket; 
    boost::asio::strand _strand;            //!< protects _status
    boost::asio::deadline_timer _timer;
    message::Header _myVersion;
    message::Header _theirVersion;
    boost::asio::streambuf _incomingMessage;
    const Callback _callback;
};

} // server
} // mux
} // runjob

#endif
