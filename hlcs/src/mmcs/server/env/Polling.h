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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_ENV_POLLING_H
#define MMCS_ENV_POLLING_H

#include "types.h"

#include <utility/include/performance/DatabaseOutput.h>
#include <utility/include/performance/GlobalStatistics.h>
#include <utility/include/performance/List.h>
#include <utility/include/performance/StatisticSet.h>
#include <utility/include/portConfiguration/Connector.h>
#include <utility/include/portConfiguration/types.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace mmcs {
namespace server {
namespace env {

class McServerConnection;

/*!
 * \brief Base class for environmental data collection.
 *
 * This class presents an asyncrhonous interface for operations needed
 * by other environmental types:
 *
 * - connecting to mc_server, see Polling::poll()
 * - closing a target set, see Polling::closeTarget()
 * - creating performance counters, see Polling::time()
 * - waiting for the configured interval to begin polling, see Polling::wait()
 */
class Polling : public boost::enable_shared_from_this<Polling>
{
public:
    typedef bgq::utility::performance::StatisticSet<
        bgq::utility::performance::List<bgq::utility::performance::DataPoint>,
        bgq::utility::performance::DatabaseOutput
        > PerformanceCounters;

    typedef boost::shared_ptr<Polling> Ptr;

    typedef PerformanceCounters::Timer Timer;

    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief ctor.
     */
    Polling(
            boost::asio::io_service& io_service,    //!< [in]
            unsigned interval                       //!< [in]
          );

    /*!
     * \brief dtor.
     */
    virtual ~Polling();

    /*!
     * \brief
     */
    void wait();

protected:
    /*!
     * \brief
     */
    void poll(
            const boost::system::error_code& error
            );

    /*!
     * \brief
     */
    virtual void impl(
            const boost::shared_ptr<McServerConnection>& mc_server //!< [in]
            ) = 0;

    /*!
     * \brief
     */
    virtual void done() { }

    /*!
     * \brief
     */
    Timer::Ptr time();

    /*!
     * \brief
     */
    void closeTarget(
            const boost::shared_ptr<McServerConnection>& mc_server, //!< [in]
            const std::string& name,                                //!< [in]
            const int handle,                                       //!< [in]
            const Callback& callback                                //!< [in]
            );

private:
    unsigned getPollingProperty();

    virtual std::string getDescription() const = 0;

    void connectHandler(
            const bgq::utility::Connector::Error::Type error,
            const std::string& message,
            const boost::shared_ptr<McServerConnection>& mc_server
            );

    void closeTargetHandler(
            std::istream& response,
            const std::string& name,
            const int handle,
            const Callback& callback
            );

protected:
    boost::posix_time::ptime _start;
    unsigned _seconds;
    bool _enabled;
    PerformanceCounters::Ptr _counters;
    boost::asio::io_service& _io_service;
    boost::asio::deadline_timer _timer;
};

} } } // namespace mmcs::server::env

#endif
