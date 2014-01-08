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
#ifndef RUNJOB_SERVER_PERFORMANCE_COUNTERS_H
#define RUNJOB_SERVER_PERFORMANCE_COUNTERS_H

#include "server/performance/Interval.h"

#include "server/fwd.h"

#include <utility/include/performance/DatabaseOutput.h>
#include <utility/include/performance/GlobalStatistics.h>
#include <utility/include/performance/List.h>
#include <utility/include/performance/StatisticSet.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace server {
namespace performance {

/*!
 * \brief Interface to measure performance counters for job submission.
 */
class Counters : public boost::enable_shared_from_this<Counters>
{
public:
    /*!
     * \brief Container type for job counters.
     */
    typedef bgq::utility::performance::StatisticSet<
        bgq::utility::performance::List<
            bgq::utility::performance::DataPoint
            >,
             bgq::utility::performance::DatabaseOutput
        > JobContainer;

    /*!
     * \brief Container type for miscellaneous counters.
     */
    typedef bgq::utility::performance::StatisticSet<
        bgq::utility::performance::List<
            bgq::utility::performance::DataPoint
            >
        > MiscContainer;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Counters> Ptr;

public:
    /*!
     * \brief dtor.
     */
    ~Counters();

    /*!
     * \brief Factory.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server
            );

    /*!
     * \brief Get the job container.
     */
    JobContainer::Ptr getJobs() const { return _jobs; }

    /*!
     * \brief Get the container for miscellaneous counters.
     */
    MiscContainer::Ptr getMisc() const { return _misc; }

private:
    Counters(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

    void timerHandler(
            const boost::system::error_code& error  //!< [in]
            );

private:
    const Options& _options;                                    //!<
    boost::asio::deadline_timer _timer;                         //!<
    JobContainer::Ptr _jobs;                                    //!<
    MiscContainer::Ptr _misc;                                   //!<
    Interval _interval;                                         //!<
};

} // performance
} // server
} // runjob

#endif
