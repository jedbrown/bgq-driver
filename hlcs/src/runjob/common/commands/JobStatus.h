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
#ifndef RUNJOB_COMMANDS_JOB_STATUS_H
#define RUNJOB_COMMANDS_JOB_STATUS_H
/*!
 * \file runjob/commands/JobStatus.h
 * \brief runjob::commands::request::JobStatus and runjob::commands::response::JobStatus definition and implementation.
 * \ingroup command_protocol
 */

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <hwi/include/common/uci.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>

namespace runjob {
namespace commands {
namespace request {

/*!
 * \brief Request message for job_status command.
 * \ingroup command_protocol
 */
class JobStatus : public Request
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<JobStatus> Ptr;

    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

public:
    /*!
     * \brief ctor.
     */
    JobStatus() :
        Request( Message::Tag::JobStatus ),
        _job( 0 ),
        _pid( 0 ),
        _hostname( )
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
        ar & boost::serialization::base_object<Request>(*this);
        ar & _job;
        ar & _pid;
        ar & _hostname;
    }

public:
    BGQDB::job::Id _job;
    pid_t _pid;
    std::string _hostname;
};

} // request

namespace response {

/*!
 * \brief Response message for job_status command.
 * \ingroup command_protocol
 */
class JobStatus : public Response
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 6;

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<JobStatus> Ptr;

    /*!
     * \brief Representation of an I/O node connection.
     */
    class IoConnection
    {
    public:
        IoConnection() :
            _location( 0 ),
            _computes( 0 ),
            _drained( false ),
            _killed( false ),
            _ended( false ),
            _error( false ),
            _exited( false ),
            _loaded( false ),
            _outputStarted( false ),
            _running( false ),
            _hardwareFailure( false ),
            _signalInFlight( false )
        {

        }

        BG_UniversalComponentIdentifier _location;  //!< node location
        size_t _computes;       //!< number of compute nodes
        bool _drained;          //!<
        bool _killed;           //!<
        bool _ended;            //!<
        bool _error;            //!<
        bool _exited;           //!<
        bool _loaded;           //!<
        bool _outputStarted;    //!<
        bool _running;          //!<
        bool _hardwareFailure;  //!<
        bool _signalInFlight;   //!<

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _location;
            ar & _computes;
            ar & _drained;
            ar & _killed;
            ar & _ended;
            ar & _error;
            ar & _exited;
            ar & _loaded;
            ar & _outputStarted;
            ar & _running;
            ar & _hardwareFailure;
            ar & _signalInFlight;;
        }
    };

    /*!
     * \brief Container of IoConnection objects.
     */
    typedef std::vector<IoConnection> Connections;

public:
    /*!
     * brief ctor
     */
    JobStatus() :
        Response( Message::Tag::JobStatus ),
        _connections(),
        _killTimeout(),
        _mux()
    {

    }

    Connections _connections;   //!< connection container
    boost::posix_time::time_duration _killTimeout;   //!< kill timeout
    std::string _mux;

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
        ar & _connections;
        ar & _killTimeout;
        ar & _mux;
    }
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::JobStatus, runjob::commands::request::JobStatus::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::JobStatus, runjob::commands::response::JobStatus::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::JobStatus::IoConnection, runjob::commands::response::JobStatus::ProtocolVersion )

#endif
