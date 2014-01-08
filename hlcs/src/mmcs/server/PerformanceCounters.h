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

#ifndef MMCS_SERVER_PERFORMANCE_COUNTERS_H
#define MMCS_SERVER_PERFORMANCE_COUNTERS_H

#include "types.h"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <utility/include/performance/DatabaseOutput.h>
#include <utility/include/performance/DataPoint.h>
#include <utility/include/performance/List.h>
#include <utility/include/performance/StatisticSet.h>

#include <string>

namespace mmcs {
namespace server {

/*!
 * \brief Container for storing block boot performance counters.
 *
 * Each object is expected to be associated with a single block. Since there
 * are some operations involved with each boot that happen before or after the boot
 * cookie is created, the counters are retained in the container until the output
 * method is invoked with the boot cookie.
 */
class PerformanceCounters : private boost::noncopyable
{
public:
    /*!
     * \brief Container type.
     *
     * Storage is a std::list, output policy goes to the database.
     */
    typedef bgq::utility::performance::StatisticSet<
        bgq::utility::performance::List<
            bgq::utility::performance::DataPoint
            >,
            bgq::utility::performance::DatabaseOutput
        > Container;

    typedef Container::Timer Timer;

public:
    /*!
     * \brief ctor.
     */
    explicit PerformanceCounters(
            const BlockPtr& block   //!< [in] block
            );

    /*!
     * \brief Create a timer.
     */
    Timer::Ptr create();

    /*!
     * \brief Output all counters.
     */
    void output(
            unsigned cookie //!< [in] boot cookie
            );

private:
    void getBlockSize();

private:
    const BlockPtr                     _block;
    const boost::shared_ptr<Container> _container;
    std::string                        _otherData;
};

} } // namespace mmcs::server

#endif
