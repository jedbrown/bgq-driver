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
#ifndef RUNJOB_COMMON_MESSAGE_PERF_COUNTERS_H
#define RUNJOB_COMMON_MESSAGE_PERF_COUNTERS_H

#include "common/Message.h"

#include <utility/include/performance/DataPoint.h>

#include <boost/archive/binary_oarchive.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

#include <new> // for placement new

namespace runjob {
namespace message {

/*!
 * \brief Message used to send performance counters from a runjob_mux to the runjob_server.
 */
class PerfCounters: public Message
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<PerfCounters> Ptr;

    /*!
     * \brief const pointer type.
     */
    typedef boost::shared_ptr<const PerfCounters> ConstPtr;

    /*!
     * \brief Container type.
     */
    typedef std::vector<bgq::utility::performance::DataPoint> Container;

public:
    /*!
     * \brief ctor
     */
    PerfCounters() :
        Message(),
        _data()
    {
        this->setType(Message::PerfCounters);
    }

    /*!
     * \brief
     */
    void serialize(std::ostream& os) {
        boost::archive::binary_oarchive ar(os);
        ar & *this;
    }

    /*!
     * \brief Add a DataPoint.
     */
    void addDataPoint(
            const bgq::utility::performance::DataPoint& data    //!< [in]
            )
    {
        _data.push_back( data );
    }

    /*!
     * \brief Get the container of DataPoint objects.
     */
    const Container& getData() const { return _data; }


private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _data;
    }

private:
    Container _data;
};

} // message
} // runjob

namespace boost {
namespace serialization {

template <class Archive>
void serialize(
        Archive& /* ar */,
        bgq::utility::performance::DataPoint& /* data */,
        const unsigned int /* version */
        )
{

}

template <class Archive>
void 
save_construct_data(
        Archive & ar, 
        const bgq::utility::performance::DataPoint* data, 
        const unsigned int /* version */
        )
{
    // save data required to construct instance
    ar & data->getFunction();
    ar & data->getSubFunction();
    ar & data->getId();
    ar & data->getOtherData();
    ar & data->getStart();
    ar & data->getDuration();
    const int mode = data->getMode();
    ar & mode;
}

template <class Archive>
void
load_construct_data(
        Archive & ar, 
        bgq::utility::performance::DataPoint* data,
        const unsigned int /* version */
        )
{
    // retrieve data from archive required to construct new instance
    std::string function;
    ar & function;
    std::string sub_function;
    ar & sub_function;
    std::string id;
    ar & id;
    std::string other;
    ar & other;
    boost::posix_time::ptime start;
    ar & start;
    boost::posix_time::time_duration duration;
    ar & duration;
    bgq::utility::performance::Mode::Value::Type mode;
    ar & mode;
    
    // invoke placement new for DataPoint object
    // see http://www.parashift.com/c++-faq-lite/dtors.html#faq-11.10
    (void)::new(data) bgq::utility::performance::DataPoint(function, id, duration);
    data->setOtherData( other );
    data->setSubFunction( sub_function );
    data->setStart( start );
    data->setMode( mode );
}

} // serialization
} // boost

#endif
