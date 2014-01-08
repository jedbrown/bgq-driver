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
#ifndef RUNJOB_MUX_PERFORMANCE_COUNTERS_H
#define RUNJOB_MUX_PERFORMANCE_COUNTERS_H

#include "mux/performance/Interval.h"
#include "mux/performance/Output.h"

#include "mux/server/fwd.h"

#include "mux/fwd.h"

#include <utility/include/performance/GlobalStatistics.h>
#include <utility/include/performance/List.h>
#include <utility/include/performance/StatisticSet.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace mux {
namespace performance {

/*!
 * \brief Interface to measure performance counters for job submission.
 */
class Counters : public boost::enable_shared_from_this<Counters>
{
public:
    /*!
     * \brief Container type.
     */
    typedef bgq::utility::performance::StatisticSet<
        bgq::utility::performance::List<
            bgq::utility::performance::DataPoint
            >,
        Output
    > Container;

public:
    /*!
     * \brief ctor.
     */
    Counters(
            boost::asio::io_service& io_service,                //!< [in]
            const Options& options,                             //!< [in]
            const boost::shared_ptr<server::Connection>& server //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Counters();

    /*!
     * \brief Start collecting counters.
     */
    void start();

    /*!
     * \brief Get the container.
     */
    const Container::Ptr& getContainer() const { return _container; }

private:
    /*!
     * \brief
     */
    void timerHandler(
        const boost::system::error_code& error  //!< [in]
        );

private:
    const Options& _options;                                    //!<
    boost::asio::deadline_timer _timer;                         //!<
    Container::Ptr _container;                                  //!<
    boost::shared_ptr<server::Connection> _server;              //!<
    Interval _interval;                                         //!<
};

} // performance
} // mux
} // runjob

#endif
