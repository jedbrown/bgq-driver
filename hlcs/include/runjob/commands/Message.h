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
#ifndef RUNJOB_COMMANDS_MESSAGE_H
#define RUNJOB_COMMANDS_MESSAGE_H
/*!
 * \file runjob/commands/Message.h
 * \brief runjob::commands::Message definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/Header.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include <boost/shared_ptr.hpp>

#include <cassert>
#include <sstream>
#include <string>

namespace runjob {
namespace commands {

/*!
 * \brief Protocol version number.
 */
static const unsigned ProtocolVersion = 3;

/*!
 * \brief Abstract message class for all messages to derive from.
 * \ingroup command_protocol
 */
class Message
{
public:
    /*!
     * \brief Supported message types.
     */
    struct Header {
        /*!
         * \brief types.
         */
        enum Type {
            Invalid,
            Request,
            Response,
            NumTypes
        };
    };

    /*!
     * \brief Supported message tags.
     */
    struct Tag {
        /*!
         * \brief types.
         */
        enum Type {
            Invalid,
            EndTool,
            JobAcl,
            JobStatus,
            KillJob,
            LocateRank,
            LogLevel,
            MuxStatus,
            RefreshConfig,
            ServerStatus,
            StartTool,
            ToolStatus,
            ChangeCiosConfig,
            DumpProctable,
            NumTypes
        };
    };

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Message> Ptr;

public:
    /*!
     * \brief Convert a Header::Type into a string.
     */
    static const char* toString(
            Header::Type t //!< [in]
            )
    {
        static const char* strings[] = {
            "Request",
            "Response"
        };

        if (t > Header::Invalid  && t < Header::NumTypes) {
            return strings[static_cast<uint32_t>(t) - 1];
        } else {
            return "INVALID";
        }
    }

    /*!
     * \brief Convert a Tag::Type into a string.
     */
    static const char* toString(
            Tag::Type t //!< [in]
            )
    {
        static const char* strings[] = {
            "EndTool",
            "JobAcl",
            "JobStatus",
            "KillJob",
            "LocateRank",
            "LogLevel",
            "MuxStatus",
            "RefreshConfig",
            "ServerStatus",
            "StartTool",
            "ToolStatus",
            "ChangeCiosConfig",
            "DumpProctable"
        };

        if (t > Tag::Invalid && t < Tag::NumTypes) {
            return strings[static_cast<uint32_t>(t) - 1];
        } else {
            return "INVALID";
        }
    }

public:
    /*!
     * \brief ctor.
     */
    explicit Message(
            Header::Type h = Header::Invalid,       //!< [in]
            Tag::Type t = Tag::Invalid              //!< [in]
            ) :
        _type(h),
        _tag(t)
    {

    }

    /*!
     * \brief dtor
     */
    virtual ~Message() { }

    /*!
     * \brief Serialize message into a string.
     */
    void serialize(
            std::ostream& os      //!< [in]
            )
    {
        return this->doSerialize( os );
    }

    Header::Type getType() const { return _type; }  //!< Get Type.
    Tag::Type getTag() const { return _tag; }   //!< Get Tag.

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & _type;
        ar & _tag;
    }

    virtual void doSerialize(
            std::ostream& os
            ) = 0;

private:
    Header::Type _type;     //!<
    Tag::Type _tag;         //!<
};

} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::Message, runjob::commands::ProtocolVersion )

#endif
