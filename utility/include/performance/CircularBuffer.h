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
#ifndef BGQ_UTILITY_PERFORMANCE_CIRCULAR_BUFFER_H
#define BGQ_UTILITY_PERFORMANCE_CIRCULAR_BUFFER_H
/*!
 * \file utility/include/performance/CircularBuffer.h
 * \brief \link bgq::utility::performance::CircularBuffer CircularBuffer\endlink definition and implementation.
 */

#include <utility/include/performance/Mutex.h>

#include <utility/include/performance.h>

#include <utility/include/Log.h>

#include <boost/circular_buffer.hpp>

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
 * Objects are stored in a boost::circular_buffer. Objects are removed from the container using
 * CircularBuffer::get.
 *
 * The capacity of the buffer is obtained from the buffer_capacity key in the [performance] section of the Blue
 * Gene properties file. If no value is found, the static member CircularBuffer::DefaultBufferCapacity is used.
 */
template <
    typename T,
    typename ThreadingPolicy = Mutex
>
class CircularBuffer : public ThreadingPolicy
{
public:
    /*!
     * \brief Default buffer capacity.
     */
    static const size_t DefaultBufferCapacity;

public:
    /*!
     * \brief Container type.
     */
    typedef boost::circular_buffer<T> Container;

public:
    /*!
     * \brief ctor.
     */
    CircularBuffer() :
        _container()
    {
        // can't use LOG_DECLARE_FILE here since we're in a header
        log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.performance" );

        // get capacity of circular buffer from properties
        const bgq::utility::Properties::ConstPtr properties = getProperties();
        const std::string key = "buffer_capacity";
        const std::string section = "performance";
        int capacity = DefaultBufferCapacity;
        try {
            capacity = boost::lexical_cast<int>( 
                    properties->getValue( section, key )
                    );

            // sanity check
            if ( capacity <= 0 ) {
                LOG_WARN_MSG( "invalid " << key << " value: " << capacity );
                capacity = DefaultBufferCapacity;
                LOG_WARN_MSG( "using default capacity of " << capacity );
            } else {
                LOG_DEBUG_MSG( "using circular buffer capacity of " << capacity );
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_WARN_MSG( "garbage value from key " << key << " in section " << section << ": " << e.what() );
            LOG_WARN_MSG( "using default capacity of " << capacity );
        } catch ( const std::invalid_argument& e ) {
            LOG_WARN_MSG( e.what() );
            LOG_WARN_MSG( "using default capacity of " << capacity );
        }

        // create buffer
        _container = Container( capacity );
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
     * \brief Get buffer capacity.
     */
    const size_t getCapacity() const { 
        typename ThreadingPolicy::ScopedLock guard(this);
        return _container.capacity();
    }

    /*!
     * \brief Get count.
     */
    const size_t getCount() const { 
        typename ThreadingPolicy::ScopedLock guard(this);
        return _container.size();
    }

    /*!
     * \brief Get and clear the container.
     */
    void get(
            Container& container    //!< [out]
            )
    {
        // clear input
        container.clear();

        // lock mutex and swap container
        typename ThreadingPolicy::ScopedLock guard(this);
        container.swap( _container );

        // set capacity to our old value since it was altered by swap
        _container.set_capacity( container.capacity() );
    }

private:
    Container _container;
};

// storage for default buffer capacity
template <
    typename T,
    typename ThreadingPolicy
>
const size_t CircularBuffer<T,ThreadingPolicy>::DefaultBufferCapacity = 2048;

} // performance
} // utility
} // bgq

#endif
