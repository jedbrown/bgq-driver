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
#ifndef RUNJOB_COMMANDS_END_TOOL_H
#define RUNJOB_COMMANDS_END_TOOL_H
/*!
 * \file runjob/commands/EndTool.h
 * \brief runjob::commands::request::EndTool and runjob::commands::response::EndTool definition and implementation.
 * \ingroup command_protocol
 */

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <string>

namespace runjob {
namespace commands {
namespace request {

/*!
 * \brief Request message for start_tool command.
 * \ingroup command_protocol
 */
class EndTool : public Request
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<EndTool> Ptr;

public:
    /*!
     * \brief ctor.
     */
    EndTool() :
        Request( Message::Tag::EndTool ),
        _tool( 0 ),
        _job( 0 ),
        _signal( 0 ),
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
        ar & _tool;
        ar & _job;
        ar & _signal;
        ar & _pid;
        ar & _hostname;
    }

public:
    unsigned _tool;
    BGQDB::job::Id _job;
    int _signal;
    pid_t _pid;
    std::string _hostname;
};

} // request

namespace response {

/*!
 * \brief Response message for end_tool command.
 * \ingroup command_protocol
 */
class EndTool : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<EndTool> Ptr;

public:
    /*!
     * brief ctor
     */
    EndTool() :
        Response( Message::Tag::EndTool )
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
    }
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::EndTool, runjob::commands::request::EndTool::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::EndTool, runjob::commands::ProtocolVersion )

#endif
