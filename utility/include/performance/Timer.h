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
#ifndef BGQ_UTILITY_PERFORMANCE_TIMER_H
#define BGQ_UTILITY_PERFORMANCE_TIMER_H
/*!
 * \file utility/include/performance/Timer.h
 * \brief \link bgq::utility::performance::Timer Timer\endlink definition and implementation.
 */

#include <string>

#include <utility/include/performance/DataPoint.h>
#include <utility/include/performance/Mode.h>
#include <utility/include/performance/GlobalStatistics.h>

#include <utility/include/Log.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>

#include <exception>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief RAII wrapper around the DataPoint objects.
 *
 * \see bgq::utility::StatisticSet for usage information.
 */
template <typename T>
class Timer : boost::noncopyable, public boost::enable_shared_from_this< Timer<T> >
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr< Timer<T> > Ptr;

public:
    /*!
     * \brief ctor.
     * 
     * Create a timer for storage in a specific container.
     *
     * \throws std::invalid_argument if container == NULL
     */
    Timer(
            typename T::Ptr container   //!< [in]
         ) :
        _container( container ),
        _function( ),
        _sub_function( ),
        _id( ),
        _other( ),
        _qualifier( ),
        _start( boost::posix_time::microsec_clock::local_time() ),
        _stop( boost::posix_time::not_a_date_time ),
        _mode( Mode::Value::None ),
        _dismiss( false )
    {
        if ( !_container ) {
            throw std::invalid_argument( "container" );
        }
    }

    /*!
     * \brief ctor.
     *
     * \throws std::invalid_argument if container is not found.
     */
    Timer(
            const std::string& container,                       //!< [in]
            const std::string& function,                        //!< [in]
            const std::string& id,                              //!< [in]
            const Mode::Value::Type mode = Mode::Value::None,   //!< [in]
            const std::string& other = std::string(),           //!< [in]
            const std::string& qualifier = std::string(),       //!< [in]
            const std::string& sub_function = std::string()     //!< [in]
         ) :
        _container( GlobalStatistics::instance().get<typename T::Ptr>( container )),
        _function( function ),
        _sub_function( sub_function ),
        _id( id ),
        _other( other ),
        _qualifier( qualifier ),
        _start( boost::posix_time::microsec_clock::local_time() ),
        _stop( boost::posix_time::not_a_date_time ),
        _mode( mode ),
        _dismiss( false )
    {
        // sanity check for container
        BOOST_ASSERT( _container );
    }

    /*!
     * \brief Stop the timer.
     */
    void stop()
    {
        // record current time
        _stop = boost::posix_time::microsec_clock::local_time();
    }

    /*!
     * \brief Set the ID.
     * 
     * \throws boost::bad_lexical_cast if id is not convertible to a string.
     */
    template <typename U>
    Ptr id(
            const U& id    //!< [in]
            )
    {
        this->setId( id );
        return this->shared_from_this();
    }

    /*!
     * \copydoc id
     */
    template <typename U>
    void setId(
            const U& id //!< [in]
            )
    {
        _id = boost::lexical_cast<std::string>( id );
    }

    /*!
     * \brief Set the mode.
     */
    Ptr mode(
            Mode::Value::Type mode  //!< [in]
            )
    {
        this->setMode( mode );
        return this->shared_from_this();
    }

    /*!
     * \copydoc mode
     */
    void setMode(
            Mode::Value::Type mode    //!< [in]
            )
    {
        _mode = mode;
    }

    /*!
     * \brief Set the function.
     * 
     * \throws boost::bad_lexical_cast if function is not convertible to a string.
     */
    template <typename U>
    Ptr function(
            const U& function    //!< [in]
            )
    {
        this->setFunction( function );
        return this->shared_from_this();
    }

    /*!
     * \copydoc function.
     */
    template <typename U>
    void setFunction(
            const U& function   //!< [in]
            )
    {
        _function = boost::lexical_cast<std::string>( function );
    }

    /*!
     * \brief Set other data.
     *     
     * \throws boost::bad_lexical_cast if data is not convertible to a string.
     */
    template <typename U>
    Ptr otherData(
            const U& data    //!< [in]
            )
    {
        this->setOtherData( data );
        return this->shared_from_this();
    }

    /*!
     * \copydoc otherData
     */
    template <typename U>
    void setOtherData(
            const U& data   //!< [in]
            )
    {
        _other = boost::lexical_cast<std::string>( data );
    }

    /*!
     * \brief Set sub function.
     * 
     * \throws boost::bad_lexical_cast if sub_function is not convertible to a string.
     */
    template <typename U>
    Ptr subFunction(
            const U& sub_function    //!< [in]
            )
    {
        this->setSubFunction( sub_function );
        return this->shared_from_this();
    }

    /*!
     * \copydoc subFunction
     */
    template <typename U>
    void setSubFunction(
            const U& sub_function   //!< [in]
            )
    {
        _sub_function = boost::lexical_cast<std::string>( sub_function );
    }

    /*!
     * \brief Set qualifier.
     *     
     * \throws boost::bad_lexical_cast if data is not convertible to a string.
     */
    template <typename U>
    Ptr qualifier(
            const U& qualifier    //!< [in]
            )
    {
        this->setQualifier( qualifier );
        return this->shared_from_this();
    }

    /*!
     * \copydoc otherData
     */
    template <typename U>
    void setQualifier(
            const U& qualifier   //!< [in]
            )
    {
        _qualifier = boost::lexical_cast<std::string>( qualifier );
    }

    /*!
     * \brief Get the ID.
     */
    const std::string& getId() const { return _id; }

    /*!
     * \brief Dismiss this Timer.
     */
    void dismiss(
            bool d = true   //!< [in]
            )
    {
        _dismiss = d;
    }

    /*!
     * \brief dtor.
     */
    ~Timer()
    {
        // get stop time, stop timer if it has not been stopped
        if ( _stop.is_not_a_date_time() ) {
            this->stop();
        }

        // can't use LOG_DECLARE_FILE here since we're in a header
        const log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.performance" );

        // check if this timer has been dismissed
        if ( _dismiss ) {
            LOG_TRACE_MSG( "dismissing  Timer(" << _function << ", " << _id << ", " << _other << ", " << Mode::toString(_mode) << ")" );
            return;
        }

        // get global mode
        const Mode::Value::Type global_mode = Mode::instance().getMode();

        try {
            // ensure mode of this duration is enabled
            if ( global_mode >= _mode ) {
                // get elapsed time
                boost::posix_time::time_duration elapsed( _stop - _start );

                // create data point
                DataPoint duration( _function, _id, elapsed );

                duration.setStart( _start );
                duration.setOtherData( _other );
                duration.setSubFunction( _sub_function );
                duration.setQualifier( _qualifier );
                duration.setMode( _mode );
                LOG_TRACE_MSG( 
                        _function << "," <<
                        ( _sub_function.empty() ? "" : _sub_function + "," ) << 
                        ( _other.empty() ? "" : _other + "," ) << 
                        ( _qualifier.empty() ? "" : _qualifier + "," ) << 
                        Mode::toString( _mode ) << " " <<
                        _id << ": " <<
                        elapsed.total_microseconds() << "us"
                        );

                _container->add( duration );
            } else {
                LOG_TRACE_MSG(
                        "mode " << 
                        Mode::toString( global_mode )  << " " <<
                        "lower than requested mode of " << Mode::toString( _mode )
                        );
            }
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( "caught exception: " << e.what() );
        } catch ( ... ) {
            // eat any exceptions, we can't throw from a dtor
            LOG_WARN_MSG( "caught unknown exception" );
        }
    }

private:
     const typename T::Ptr _container;
     std::string _function;
     std::string _sub_function;
     std::string _id;
     std::string _other;
     std::string _qualifier;
     const boost::posix_time::ptime _start;
     boost::posix_time::ptime _stop;
     Mode::Value::Type _mode;
     bool _dismiss;
};

} // performance
} // utility
} // bgq

#endif
