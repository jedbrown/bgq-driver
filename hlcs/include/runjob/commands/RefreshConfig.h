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
#ifndef RUNJOB_SERVER_COMMANDS_REFRESH_CONFIG_H
#define RUNJOB_SERVER_COMMANDS_REFRESH_CONFIG_H
/*!
 * \file runjob/commands/RefreshConfig.h
 * \brief runjob::commands::request::RefreshConfig and runjob::commands::response::RefreshConfig definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <boost/serialization/list.hpp>

#include <list>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for status command.
 * \ingroup command_protocol
 */
class RefreshConfig : public Request
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<RefreshConfig> Ptr;

public:
    /*!
     * \brief ctor.
     */
    RefreshConfig() :
        Request(Message::Tag::RefreshConfig),
        _filename()
    {

    }

    std::string _filename;

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
        ar & _filename;
    }

private:
};

} // request

namespace response {

/*!
 * \brief Response message for status command.
 * \ingroup command_protocol
 */
class RefreshConfig : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<RefreshConfig> Ptr;

public:
    /*!
     * brief Ctor.
     */
    RefreshConfig() :
        Response(Message::Tag::RefreshConfig)
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

BOOST_CLASS_VERSION( runjob::commands::request::RefreshConfig, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::RefreshConfig, runjob::commands::ProtocolVersion )

#endif
