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
#ifndef RUNJOB_COMMANDS_RESPONSE_H
#define RUNJOB_COMMANDS_RESPONSE_H
/*!
 * \file runjob/commands/Response.h
 * \brief runjob::commands::Response definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/error.h>
#include <hlcs/include/runjob/commands/Message.h>

#include <string>

#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace runjob {
namespace commands {

/*!
 * \brief Abstract Response type used for responses to Request messages.
 * \ingroup command_protocol
 */
class Response : public Message
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Response> Ptr;

public:
    /*!
     * \brief ctor
     */
    Response(
            Message::Tag::Type t        //!< [in]
            ) :
        Message(Message::Header::Response, t),
        _message(),
        _error( error::success )
    {

    }
   
    /*!
     * \brief copydoc runjob::commands::Message::serialize
     */
    void serialize(
            std::ostringstream& os
            )
    {
        boost::archive::text_oarchive ar(os);
        ar & *this;
    }

    /*!
     * \brief Set descriptive message.
     */
    void setMessage(
            const std::string& message  //!< [in]
            ) 
    {
        _message = message;
    }

    /*!
     * \brief set error.
     */
    void setError(
            error::rc e //!< [in]
            )
    {
        _error = e;
    }

    // getters
    const std::string& getMessage() const { return _message; }  //!< Get error message.
    error::rc getError() const { return _error; }    //!< Get error code.

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _message;
        ar & _error;
    }

private:
    error::rc _error;           //!< error code
    std::string _message;       //!< descriptive error message
};

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::Response, runjob::commands::ProtocolVersion )

#endif


