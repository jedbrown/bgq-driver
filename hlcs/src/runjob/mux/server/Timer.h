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
#ifndef RUNJOB_MUX_SERVER_TIMER_H
#define RUNJOB_MUX_SERVER_TIMER_H

#include <utility/include/portConfiguration/Connector.h>

#include "mux/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

namespace runjob {
namespace mux {
namespace server {

/*!
 * \brief Retry connecting to an I/O node with a specific interval and ceiling.
 */
class Timer : public boost::enable_shared_from_this<Timer>
{
public:
    /*!
     * \brief Socket type.
     */
    typedef bgq::utility::portConfig::SocketPtr SocketPtr;

    /*!
     * \brief Callback for start method.
     */
    typedef boost::function<
        void(
                SocketPtr socket,
                boost::system::error_code
            )
        > Callback;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Timer> Ptr;

public:
    /*!
     * \brief Default number of seconds to wait between connection attempts.
     */
    static const unsigned DefaultInterval;

    /*!
     * \brief ctor.
     */
    Timer(
            const Options& options,             //!< [in]
            boost::asio::strand& strand         //!< [in]
            );

    /*!
     * \brief Start connecting.
     */
    void start(
            const Callback& callback    //!< [in]
            );

    /*!
     * \brief Stop connecting.
     */
    void stop(
            boost::system::error_code& error    //!< [in]
            );

    /*!
     * \brief
     */
    unsigned getInterval() const;

private:
    /*!
     * \brief
     */
    void timerHandler(
            const boost::system::error_code& error, //!< [in]
            const Callback& callback                //!< [in]
            );

    /*!
     * \brief
     */
    void connectHandler(
            const bgq::utility::Connector::ConnectResult& res, //!< [in]
            const Callback& callback                           //!< [in]
            );

private:
    boost::asio::strand _strand;                            //!<
    boost::asio::deadline_timer _timer;                     //!<
    const Options& _options;                                //!<
    bgq::utility::Connector _connector;                     //!<
};

} // server
} // mux
} // runjob

#endif
