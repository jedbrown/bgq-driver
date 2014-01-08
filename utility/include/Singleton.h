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
/*!
 * \file utility/include/Singleton.h
 * \brief \link bgq::utility::Singleton Singleton\endlink definition and implementation.
 */

#ifndef BGQ_UTILITY_SINGLETON_H_
#define BGQ_UTILITY_SINGLETON_H_

#include <exception>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace bgq {
namespace utility {

/*!
 * \brief Implementation of the Singleton's CreationPolicy.
 *
 * \tparam T
 */
template <typename T>
class CreateUsingNew
{
public:
    static T* create() { return new T; }
    static void destroy(T* t) { delete t; }
protected:
    ~CreateUsingNew() { }
};

/*!
 * \brief Implementation of the Singleton's CreationPolicy.
 *
 * \tparam T
 */
template <typename T>
class CreateStatic
{
private:
    static T* _instance;

public:
    /*!
     * \brief Create the singleton instance.
     */
    static T* create() { return _instance; }

    /*!
     * \brief set the singleton instance.
     * \throws std::invalid_argument if _instance is already set.
     */
    static void setInstance(
            T* instance //!< [in]
            )
    {
        if ( _instance ) {
            throw std::invalid_argument( "instance" );
        }
        
        // set instance
        _instance = instance;
    }

    /*!
     * \brief Destroy the instance.
     */
    static void destroy(
            T* t    //!< [in]
            )
    {
        delete t;
        _instance = NULL;
    }

protected:
    ~CreateStatic() { }
};

// storage for CreateStation creation policy instance
template <typename T>
T* CreateStatic<T>::_instance;

/*!
 * \brief Singleton design pattern.
 *
 * \tparam T
 * \tparam CreationPolicy
 *
 * \warning this class locks a mutex during each call to instance. If you are
 * concerned about performance, it is suggested that you cache the reference
 * locally in each thread.
 *
 * \note you must link against the boost_thread library when using this class.
\verbatim
LDFLAGS += -L $(BGQ_INSTALL_DIR)/work/extlib/lib -lboost_thread
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/work/extlib/lib
\endverbatim
 *
 * Sample usage
 *
 * \include test/singleton/sample.cc
 */
template <
typename T,
         template <class> class CreationPolicy = CreateUsingNew
         >
         class Singleton : private boost::noncopyable, public CreationPolicy<T>
{
public:
    /*!
     * \brief Get the singleton instance.
     */
    static T& instance();

    /*!
     * \brief Get the singleton instance.
     */
    static T& Instance();

    /*!
     * \brief Reset the singleton instance.
     *
     * \internal This method is used for unit tests, do not use it in production.
     */
    static void reset();
        
protected:
    // members

    static T* _instance;            //!< The singleton instance.
    static boost::mutex _mutex;     //!< Mutex to access the singleton.
};

// storage for instance
template <
    typename T,
    template <class> class CreationPolicy
>
T* Singleton<T, CreationPolicy>::_instance = NULL;

// storage for mutex
template <
    typename T,
    template <class> class CreationPolicy
>
boost::mutex Singleton<T, CreationPolicy>::_mutex;

template <
    typename T,
    template <class> class CreationPolicy
>
inline T& Singleton<T, CreationPolicy>::instance()
{
    // lock mutex
    boost::lock_guard<boost::mutex> guard(_mutex);
    if (!_instance) {
        _instance = CreationPolicy<T>::create();
    }

    return *_instance;
}

template <
    typename T,
    template <class> class CreationPolicy
>
inline T& Singleton<T, CreationPolicy>::Instance()
{
    return Singleton<T, CreationPolicy>::instance();
}

template <
    typename T,
    template <class> class CreationPolicy
>
inline void Singleton<T, CreationPolicy>::reset()
{
    // lock mutex
    boost::lock_guard<boost::mutex> guard(_mutex);
    CreationPolicy<T>::destroy( _instance );
    _instance = NULL;
}

} // utility
} // bgq

#endif
