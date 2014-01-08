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
#ifndef RUNJOB_COMMANDS_CHANGE_CIOS_CONFIG_H
#define RUNJOB_COMMANDS_CHANGE_CIOS_CONFIG_H
/*!
 * \file runjob/commands/ChangeCiosConfig.h
 * \brief runjob::commands::request::ChangeCiosConfig and runjob::commands::response::ChangeCiosConfig definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for cios_log_level command.
 * \ingroup command_protocol
 */
class ChangeCiosConfig : public Request
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<ChangeCiosConfig> Ptr;

public:
    /*!
     * \brief ctor.
     */
    ChangeCiosConfig() :
        Request(Message::Tag::ChangeCiosConfig),
        _block(),
        _location(),
        _common(),
        _jobctl(),
        _stdio()
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
        ar & _block;
        ar & _location;
        ar & _common;
        ar & _jobctl;
        ar & _stdio;
    }

public:
    std::string _block;
    std::string _location;
    std::string _common;
    std::string _jobctl;
    std::string _stdio;
};

} // request

namespace response {

/*!
 * \brief Response message for cios_log_level command.
 * \ingroup command_protocol
 */
class ChangeCiosConfig : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<ChangeCiosConfig> Ptr;

public:
    /*!
     * brief Ctor.
     */
    ChangeCiosConfig() :
        Response(Message::Tag::ChangeCiosConfig)
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

BOOST_CLASS_VERSION( runjob::commands::request::ChangeCiosConfig, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::ChangeCiosConfig, runjob::commands::ProtocolVersion )

#endif
