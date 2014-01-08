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
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 3;

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
        _pid( 0 ),
        _hostname( ),
        _timeout( 0 ),
        _controlActionRecordId( 0 ),
        _details( )
    {

    }

    int _signal;
    BGQDB::job::Id _job;
    pid_t _pid;
    std::string _hostname;
    size_t _timeout;
    int _controlActionRecordId;
    std::string _details;

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
        ar & _pid;
        ar & _hostname;
        ar & _timeout;
        ar & _controlActionRecordId;
        ar & _details;
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
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

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
        _job(0)
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
        ar & _job;
    }

public:
    BGQDB::job::Id _job;
};

} // response

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::KillJob, runjob::commands::request::KillJob::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::KillJob, runjob::commands::response::KillJob::ProtocolVersion )

#endif
