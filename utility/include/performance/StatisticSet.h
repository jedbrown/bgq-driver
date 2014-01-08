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
#ifndef BGQ_UTILITY_PERFORMANCE_STATISTICSET_H_
#define BGQ_UTILITY_PERFORMANCE_STATISTICSET_H_
/*!
 * \file utility/include/performance/StatisticSet.h
 * \brief \link bgq::utility::performance::StatisticSet StatisticSet\endlink definition and implementation.
 */

#include <utility/include/performance/LogOutput.h>
#include <utility/include/performance/Timer.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Generic container for managing, storing, and displaying performance statistics.
 *
 * \section overview Overview
 *
 * This class features two \htmlonly <a
 * href=http://en.wikipedia.org/wiki/Policy-based_design>policies</a>\endhtmlonly
 *
 * - a Storage Policy for storing DataPoint objects.
 * - an OutputPolicy for displaying results.
 *
 * \section usage Usage
 *
 * Use the Timer nested type to add DataPoint objects to this set.
 *
 * \include test/performance/sample.cc
 *
 * \section timers Creating Timers
 *
 * You can also create Timer objects directly from the container. This has the added benefit
 * of not requiring locking a process wide mutex to store the DataPoint in the StatisticSet
 * container during the Timer object's destructor. It also exploits the 
 * <a href=http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.18>named parameter idiom</a>
 * to allow setting members without requiring them to be positional.
 *
 * \include test/performance/sample_timer.cc
 *
 * \section threads Thread Safety
 *
 * The Storage Policy has its own Threading policy, which defaults to \link bgq::utility::performance::Mutex
 * Mutex\endlink in the two provided \link bgq::utility::performance::PersistentStatisticList list\endlink and
 * \link bgq::utility::performance::PersistentStatisticBuffer buffer\endlink containers. You can overide this
 * behavior by creating a new threading policy if you manage concurrency outside of the Storage Policy.
 *
 * \see persistent statistics \link bgq::utility::performance::PersistentStatisticList list\endlink and \link
 * bgq::utility::performance::PersistentStatisticBuffer buffer\endlink
 * \see LogOutput 
 * \see GlobalStatistics 
 * \see Mode
 * \see Mutex
 */
template <
    typename SP, // Storage Policy
    typename OP = LogOutput // Output Policy
    >
class StatisticSet : public SP, public OP, public boost::enable_shared_from_this< StatisticSet<SP,OP> >
{
public:
    /*!
     * \brief Storage Policy type.
     */
    typedef SP StoragePolicy;

    /*!
     * \brief Output Policy type.
     */
    typedef OP OutputPolicy;

    /*!
     * \brief Timer type.
     */
    typedef bgq::utility::performance::Timer< StatisticSet<SP,OP> > Timer;

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr< StatisticSet<SP,OP> > Ptr;

public:
    /*!
     * \brief ctor.
     */
    explicit StatisticSet( 
            const std::string& type //!< [in]
            ) :
        _name( type )
    {
        // nothing to do 
    }

    /*!
     * \brief Create a Timer.
     
     * \throws boost::bad_lexical_cast if type or id cannot be converted to a std::string
     */
    typename Timer::Ptr create()
    {
        return typename Timer::Ptr( new Timer(this->shared_from_this()) );
    }

    /*!
     * \brief Get the name for this statistic.
     */
    const std::string& getName() const { return _name; }

    /*!
     * \brief Output the stored statistics using the OutputPolicy.
     */
    void output() 
    {
        OP::output(
                _name,
                *this
                );
    }

private:
    std::string _name;  //!< The type of statistics stored in this set.
};

} // performance
} // utility
} // bgq


#endif
