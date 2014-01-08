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
#ifndef BGQ_UTILITY_PERFORMANCE_PERSISTENT_STATISTICS_H
#define BGQ_UTILITY_PERFORMANCE_PERSISTENT_STATISTICS_H
/*!
 * \file utility/include/performance/PersistentStatistics.h
 * \brief types and macros for persistent storage of statistics.
 */

#include <utility/include/performance/CircularBuffer.h>
#include <utility/include/performance/DataPoint.h>
#include <utility/include/performance/List.h>
#include <utility/include/performance/StatisticSet.h>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Persistent storage of DataPoint objects until they are explicitly removed.
 */
typedef StatisticSet< List<DataPoint> > PersistentStatisticList;

/*!
 * \brief Persistent storage of DataPoint objects in a circular buffer until they are explicitly removed
 * or overwritten.
 */
typedef StatisticSet< CircularBuffer<DataPoint> > PersistentStatisticBuffer;

/*!
 * \brief Create a PersistentStatisticList and add it to the GlobalStatistics object.
 */
#define PERFORMANCE_LIST_INIT(type) \
    bgq::utility::performance::GlobalStatistics::instance().add(\
            bgq::utility::performance::PersistentStatisticList::Ptr(\
                new bgq::utility::performance::PersistentStatisticList(type) \
                )\
            )


/*!
 * \brief Create a PersistentStatisticBuffer and add it to the GlobalStatistics object.
 */
#define PERFORMANCE_BUFFER_INIT(type) \
    bgq::utility::performance::GlobalStatistics::instance().add(\
            bgq::utility::performance::PersistentStatisticBuffer::Ptr(\
                new bgq::utility::performance::PersistentStatisticBuffer(type) \
                )\
            )
} // performance
} // utility
} // bgq

#endif
