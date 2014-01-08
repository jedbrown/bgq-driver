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
#ifndef BGQ_UTILITY_PERFORMANCE_GLOBALSTATISTICS_H_
#define BGQ_UTILITY_PERFORMANCE_GLOBALSTATISTICS_H_
/*!
 * \file utility/include/performance/GlobalStatistics.h
 * \brief \link bgq::utility::performance::GlobalStatistics GlobalStatistics\endlink definition and implementation.
 */

#include <utility/include/Singleton.h>

#include <boost/thread/mutex.hpp>

#include <boost/any.hpp>
#include <boost/foreach.hpp>

#include <list>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief A global object for management of StatisticSet objects.
 *
 * Access global statistics through GlobalStatistics::instance().
 *
 * \note accessing the singleton instance locks a mutex, it is suggested to cache each instance once like so
 *
\verbatim
GlobalStatistics& instance = GlobalStatistics::instance();
instance.doSomething();
...
instance.doSomething();
\endverbatim

rather than something like

\verbatim
GlobalStatistics::instance().doSomething();
...
GlobalStatistics::instance().doSomething();
\endverbatim
 *
 * Thread Safety: the container is protected by a mutex.
 */
class GlobalStatistics : public bgq::utility::Singleton<GlobalStatistics>
{
public:
    /*!
     * \brief ctor.
     */
    GlobalStatistics() :
        _container(),
        _mutex()
    {

    }

    /*!
     * \brief Add a new StatisticSet.
     *
     * \return true if new set added, false if there was a set of the same name 
     * already.
     */
    template <typename T>
    bool add( 
            const T& statistic              //!< [in]
            )
    {
        boost::mutex::scoped_lock lock(_mutex);
        if ( _container.find( statistic->getName() ) != _container.end() ) {
            return false; 
        }

        _container[ statistic->getName() ] = statistic;
        return true;
    }

    /*!
     * \brief Get a specific StatisticSet.
     *
     * \throws std::invalid_argument if name is not found
     * \throws boost::bad_any_cast
     */
    template <typename T>
    T get( 
            const std::string& name //!< [in]
            )
    {
        T result;
        this->get(name, result);
        return result;
    }

    /*!
     * \brief Get a specific StatisticSet.
     *
     * \throws std::invalid_argument if name is not found
     * \throws boost::bad_any_cast
     */
    template <typename T>
    void get( 
            const std::string& name,    //!< [in]
            T& result                   //!< [out]
            )
    {
        boost::mutex::scoped_lock lock(_mutex);

        Container::const_iterator i = _container.find( name );
        if ( i != _container.end() ) {
            result = boost::any_cast<T>( i->second );
        } else {
            std::ostringstream os;
            os << name << " not found";
            throw std::invalid_argument( os.str() );
        }
    }

    /*!
     * \brief Get all StatisticSets.
     *
     * \throws boost::bad_any_cast
     */
    template <typename T>
    void get( 
            std::list<T>& result    //!< [out]
            )
    {
        // clear result container
        result.clear();

        boost::mutex::scoped_lock lock(_mutex);

        // iterate through container
        BOOST_FOREACH( const Container::value_type& i, _container ) {
            result.push_back(
                    boost::any_cast<T>( i.second )
                    );
        }
    }

private:
    typedef std::map<std::string, boost::any> Container;

private:
    Container _container;
    boost::mutex _mutex;
};

} // performance
} // utility
} // bgq

#endif
