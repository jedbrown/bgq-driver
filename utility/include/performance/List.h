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
#ifndef BGQ_UTILITY_PERFORMANCE_LIST_H
#define BGQ_UTILITY_PERFORMANCE_LIST_H
/*!
 * \file utility/include/performance/List.h
 * \brief \link bgq::utility::performance::List List\endlink definition and implementation.
 */

#include <utility/include/performance/Mutex.h>

#include <utility/include/Log.h>

#include <list>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Implements the \link bgq::utility::performance::StatisticSet StatisticSet\endlink storage policy.
 *
 * This class features a single policy:
 *
 * - a Threading Policy for protecting access to the container.
 *
 * Objects are stored in a std::list. Objects are removed from the container using
 * List::get.
 */
template <
    typename T,
    typename ThreadingPolicy = Mutex
>
class List : public ThreadingPolicy
{
public:
    /*!
     * \brief Container type.
     */
    typedef std::list<T> Container;

public:
    /*!
     * \brief ctor.
     */
    List() :
        _container()
    {
        // nothing to do
    }

    /*!
     * \brief Add a data point.
     */
    void add(
            const T& data   //!< [in]
            )
    {
        typename ThreadingPolicy::ScopedLock guard(this);
        _container.push_back( data );
    }

    /*!
     * \brief Get count.
     */
    const size_t getCount() const { 
        typename ThreadingPolicy::ScopedLock guard(this);
        return _container.size();
    }

    /*!
     * \brief Get the container and clear it.
     */
    void get(
            Container& container    //!< [out]
            )
    {
        // clear input
        container.clear();

        // lock mutex and swap
        typename ThreadingPolicy::ScopedLock guard(this);
        container.swap( _container );
    }

private:
    Container _container;
};

} // performance
} // utility
} // bgq

#endif
