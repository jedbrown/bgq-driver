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
#ifndef RUNJOB_COMMON_MESSAGE_INSERT_JOB_H
#define RUNJOB_COMMON_MESSAGE_INSERT_JOB_H

#include "common/Message.h"
#include "common/JobInfo.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>

namespace runjob {
namespace message {

/*!
 * \brief Request to add a new job.
 */
class InsertJob : public Message
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<InsertJob> Ptr;

public:
    /*!
     * \brief ctor
     */
    InsertJob() :
        Message(),
        _jobInfo(),
        _duration()
    {
        this->setType( Message::InsertJob );
    }

    /*!
     * \brief
     */
    void serialize(
            std::ostream& os    //!< [in]
            )
    {
        boost::archive::binary_oarchive ar(os);
        ar & *this;
    }

    // setters
    void setJobInfo(const JobInfo& jobInfo) { _jobInfo = jobInfo; } //!< Set JobInfo.

    /*!
     * \brief Set duration DataPoint.
     */
    void setDuration(
            const boost::posix_time::time_duration& duration    //!< [in]
            )
    {
        _duration = duration;
    }

    JobInfo& getJobInfo() { return _jobInfo; }  //!< Get JobInfo.
    const boost::posix_time::time_duration& getDuration() const { return _duration; }   //!< \brief Get the duration DataPoint.

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _jobInfo;
        ar & _duration;
    }

private:
    JobInfo _jobInfo;
    boost::posix_time::time_duration _duration;
};

} // message
} // runjob

#endif
