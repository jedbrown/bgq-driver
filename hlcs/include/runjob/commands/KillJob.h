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
#ifndef RUNJOB_COMMANDS_KILL_JOB_H
#define RUNJOB_COMMANDS_KILL_JOB_H
/*!
 * \file runjob/commands/KillJob.h
 * \brief runjob::commands::request::KillJob and runjob::commands::response::KillJob definition and implementation.
 * \ingroup command_protocol
 */

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <utility/include/LoggingProgramOptions.h>

#include <boost/serialization/vector.hpp>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for kill_job command.
 * \ingroup protocol
 */
class KillJob : public Request
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<KillJob> Ptr;

public:
    /*!
     * \brief ctor.
     */
    KillJob() :
        Request(Message::Tag::KillJob),
        _signal( 0 ),
        _job( 0 ),
        _timeout( 0 ),
        _controlActionRecordId( 0 )
    {

    }

    int _signal;
    BGQDB::job::Id _job;
    size_t _timeout;
    int _controlActionRecordId;

    /*!
     * \copydoc runjob::commands::Message::serialize
     */
    void doSerialize(
            std::ostream& os
            )
    {
        boost::archive::text_oarchive ar(os);
        ar & *this;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Request>(*this);
        ar & _signal;
        ar & _job;
        ar & _timeout;
        ar & _controlActionRecordId;
    }
};

} // request

namespace response {

/*!
 * \brief Response message for kill_job command.
 * \ingroup protocol
 */
class KillJob : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<KillJob> Ptr;

public:
    /*!
     * brief Ctor.
     */
    KillJob() :
        Response(Message::Tag::KillJob),
        _error(0)
    {

    }

    /*!
     * \copydoc runjob::commands::Message::serialize
     */
    void doSerialize(
            std::ostream& os
            )
    {
        boost::archive::text_oarchive ar(os);
        ar & *this;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Response>(*this);
        ar & _error;
    }

private:
    uint32_t _error;
};

} // response

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::KillJob, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::KillJob, runjob::commands::ProtocolVersion )

#endif
