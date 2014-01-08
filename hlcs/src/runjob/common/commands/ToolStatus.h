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
#ifndef RUNJOB_COMMANDS_TOOL_STATUS_H
#define RUNJOB_COMMANDS_TOOL_STATUS_H
/*!
 * \file runjob/commands/ToolStatus.h
 * \brief runjob::commands::request::ToolStatus and runjob::commands::response::ToolStatus definition and implementation.
 * \ingroup command_protocol
 */

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <string>

namespace runjob {
namespace commands {
namespace request {

/*!
 * \brief Request message for start_tool command.
 * \ingroup command_protocol
 */
class ToolStatus : public Request
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<ToolStatus> Ptr;

public:
    /*!
     * \brief ctor.
     */
    ToolStatus() :
        Request( Message::Tag::ToolStatus ),
        _tool( 0 ),
        _job( 0 ),
        _pid( 0 ),
        _hostname()
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
        ar & _tool;
        ar & _job;
        ar & _pid;
        ar & _hostname;
    }

public:
    unsigned _tool;
    BGQDB::job::Id _job;
    pid_t _pid;
    std::string _hostname;
};

} // request

namespace response {

/*!
 * \brief Response message for end_tool command.
 * \ingroup command_protocol
 */
class ToolStatus : public Response
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<ToolStatus> Ptr;

    /*!
     * \brief
     */
    class Tool
    {
    public:
        /*!
         * \brief I/O node status
         */
        typedef std::map<std::string, char> Io;

    public:
        /*!
         * \brief ctor.
         */
        Tool() :
            _id( 0 ),
            _path(),
            _status(),
            _timestamp(),
            _error(),
            _io(),
            _subset()
        {

        }

    public:
        unsigned _id;
        std::string _path;
        std::string _status;
        boost::posix_time::ptime _timestamp;
        std::string _error;
        Io _io;
        std::string _subset;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _id;
            ar & _path;
            ar & _status;
            ar & _timestamp;
            ar & _error;
            ar & _io;
            ar & _subset;
        }
    };

    /*!
     * \brief
     */
    typedef std::vector<Tool> Tools;

public:
    /*!
     * brief ctor
     */
    ToolStatus() :
        Response( Message::Tag::ToolStatus ),
        _tools()
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

public:
    Tools _tools;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Response>(*this);
        ar & _tools;
    }
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::ToolStatus, runjob::commands::request::ToolStatus::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::ToolStatus, runjob::commands::response::ToolStatus::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::ToolStatus::Tool, runjob::commands::response::ToolStatus::ProtocolVersion )

#endif
