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
#ifndef BGQ_UTILITY_PERFORMANCE_MUTEX_H
#define BGQ_UTILITY_PERFORMANCE_MUTEX_H
/*!
 * \file utility/include/performance/Mutex.h
 * \brief \link bgq::utility::performance::Mutex Mutex\endlink definition and implementation.
 */

#include <boost/thread/mutex.hpp>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Implements the ThreadingPolicy for storage policy classes within a StatisticSet.
 */
class Mutex
{
public:
    class ScopedLock;
    friend class ScopedLock;

    /*!
     * \brief Scoped lock type.
     */
    class ScopedLock
    {
    public:
        /*!
         * \brief
         */
        explicit ScopedLock(
                const Mutex* host   //!< [in]
                ) :
            _host( host )
        {
            _host->_mutex.lock();
        }

        /*!
         * \brief dtor.
         */
        ~ScopedLock()
        {
            _host->_mutex.unlock();
        }

    private:
        const Mutex* _host;
    };
   
public:
    /*!
     * \brief ctor.
     */
    Mutex() :
        _mutex()
    {

    }

protected:
    mutable boost::mutex _mutex;
};

} // performance
} // utility
} // bgq

#endif
