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
#ifndef RUNJOB_SERVER_CIOS_CONNECTION_INTERVAL_H
#define RUNJOB_SERVER_CIOS_CONNECTION_INTERVAL_H

#include "common/Uci.h"

#include "server/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

namespace runjob {
namespace server {
namespace cios {

/*!
 * \brief Retry connecting to an I/O node with a specific interval and ceiling.
 */
class ConnectionInterval : public boost::enable_shared_from_this<ConnectionInterval>
{
public:
    /*!
     * \brief Socket pointer type.
     */
    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

    /*!
     * \brief Callback for start method.
     */
    typedef boost::function<
        void(
                const SocketPtr&
            )
        > Callback;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<ConnectionInterval> Ptr;

public:
    /*!
     * \brief Default number of seconds to wait between connection attempts.
     */
    static const unsigned DefaultIntervalCeiling;

    /*!
     * \brief Default number of connection attempts to make.
     */
    static const unsigned DefaultIntervalIncrement;

    /*!
     * \brief ctor.
     */
    ConnectionInterval(
            const Options& options,                 //!< [in]
            boost::asio::io_service& io_service,    //!< [in]
            const Uci& location,                    //!< [in]
            const std::string& block                //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~ConnectionInterval();

    /*!
     * \brief Start connecting.
     */
    void start(
            const boost::asio::ip::tcp::endpoint& ep,   //!< [in]
            const Callback& callback                    //!< [in]
            );

    /*!
     * \brief Stop connecting.
     */
    void stop();

    /*!
     * \brief
     */
    unsigned getMax() const { return _intervalCeiling; }

    /*!
     * \brief
     */
    unsigned getIncrement() const { return _intervalIncrement; }

private:
    void timerHandler(
            const boost::system::error_code& error, //!< [in]
            const Callback& callback                //!< [in]
            );

    void connectHandler(
            const boost::system::error_code& error, //!< [in]
            const Callback& callback                //!< [in]
            );

    void stopImpl();

    void wait(
            const Callback& callback                //!< [in]
            );

    bool available();

private:
    boost::asio::strand _strand;                            //!<
    boost::asio::deadline_timer _timer;                     //!<
    boost::asio::ip::tcp::endpoint _endpoint;               //!<
    SocketPtr _socket;                                      //!<
    const Uci _location;                                    //!<
    time_t _interval;                                       //!<
    int _intervalCeiling;                                   //!<
    int _intervalIncrement;                                 //!<
    const std::string _block;                               //!<
};

} // cios
} // server
} // runjob

#endif
