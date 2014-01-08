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
#ifndef RUNJOB_COMMON_MESSAGE_KILL_JOB_H
#define RUNJOB_COMMON_MESSAGE_KILL_JOB_H

#include "common/Message.h"

#include <boost/archive/binary_oarchive.hpp>

#include <csignal>

namespace runjob {
namespace message {

/*!
 * \brief Request to kill a job.
 */
class KillJob : public Message
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<KillJob> Ptr;

public:
    /*!
     * \brief ctor.
     */
    KillJob() :
        _clientDisconnected( false ),
        _signal( SIGKILL )
    {
        this->setType( Message::KillJob );
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

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _clientDisconnected;
        ar & _signal;
    }

public:
    bool _clientDisconnected;
    int _signal;
};

} // message
} // runjob

#endif
