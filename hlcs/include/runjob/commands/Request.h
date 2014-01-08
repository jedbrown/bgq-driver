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
#ifndef RUNJOB_COMMANDS_REQUEST_H
#define RUNJOB_COMMANDS_REQUEST_H
/*!
 * \file runjob/commands/Request.h
 * \brief runjob::commands::Request definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/Message.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace runjob {
namespace commands {

/*!
 * \brief Abstract Request message used for describing command requests.
 * \ingroup command_protocol
 */
class Request : public Message
{
public:
    /*!
     * \brief ctor.
     */
    explicit Request(
            Message::Tag::Type t     //!< [in]
            ) :
        Message(Message::Header::Request, t)
    {

    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Message>(*this);
    }
};

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::Request, runjob::commands::ProtocolVersion )

#endif
