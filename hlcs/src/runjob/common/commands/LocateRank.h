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
#ifndef RUNJOB_COMMANDS_LOCATE_RANK_H
#define RUNJOB_COMMANDS_LOCATE_RANK_H
/*!
 * \file runjob/commands/LocateRank.h
 * \brief runjob::commands::request::LocateRank and runjob::commands::response::LocateRank definition and implementation.
 * \ingroup command_protocol
 */

#include "common/Uci.h"

#include <db/include/api/job/types.h>

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <string>

namespace runjob {
namespace commands {
namespace request {

/*!
 * \brief Request message for locate_rank command.
 * \ingroup command_protocol
 */
class LocateRank : public Request
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 1;

    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<LocateRank> Ptr;

public:
    /*!
     * \brief ctor.
     */
    LocateRank() :
        Request( Message::Tag::LocateRank ),
        _rank( 0 ),
        _job( 0 )
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
        ar & _rank;
        ar & _job;
    }

public:
    uint32_t _rank;
    BGQDB::job::Id _job;
};

} // request

namespace response {

/*!
 * \brief Response message for locate_rank command.
 * \ingroup command_protocol
 */
class LocateRank : public Response
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<LocateRank> Ptr;
    
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

public:
    /*!
     * brief ctor
     */
    LocateRank() :
        Response( Message::Tag::LocateRank ),
        _location(),
        _block()
    {

    }

    Uci _location;  //!< node location
    std::string _block; //!< block ID

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
        ar & _location;
        ar & _block;
    }
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::LocateRank, runjob::commands::request::LocateRank::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::LocateRank, runjob::commands::response::LocateRank::ProtocolVersion )

#endif
