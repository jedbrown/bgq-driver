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
#ifndef RUNJOB_COMMON_MESSAGE_START_TOOL_H
#define RUNJOB_COMMON_MESSAGE_START_TOOL_H

#include "common/tool/Daemon.h"

#include "common/Message.h"

#include <utility/include/UserId.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace runjob {
namespace message {

/*!
 * \brief Request to start a tool.
 */
class StartTool : public Message
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<StartTool> Ptr;

public:
    /*!
     * \brief ctor.
     */
    StartTool() :
        Message(),
        _description(),
        _uid(),
        _pid( 0 )
    {
        this->setType( Message::StartTool );
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

public:
    tool::Daemon _description;           //!<
    bgq::utility::UserId::Ptr _uid;      //!< credentials of user
    pid_t _pid;                          //!< runjob pid

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _description;
        ar & _uid;
        ar & _pid;
    }
};

} // message
} // runjob

#endif
