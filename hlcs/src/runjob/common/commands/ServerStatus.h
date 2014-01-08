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
#ifndef RUNJOB_COMMANDS_SERVER_STATUS_H
#define RUNJOB_COMMANDS_SERVER_STATUS_H
/*!
 * \file runjob/commands/ServerStatus.h
 * \brief runjob::commands::request::ServerStatus and runjob::commands::response::ServerStatus definition and implementation.
 * \ingroup command_protocol
 */

#include "common/commands/Status.h"

#include <db/include/api/job/types.h>

#include <hwi/include/common/uci.h>

#include <boost/serialization/vector.hpp>

#include <vector>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for status command.
 * \ingroup command_protocol
 */
class ServerStatus : public Status
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<ServerStatus> Ptr;

public:
    /*!
     * \brief ctor.
     */
    ServerStatus() :
        Status( Message::Tag::ServerStatus )
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
        ar & boost::serialization::base_object<Status>(*this);
    }
};

} // request

namespace response {

/*!
 * \brief Response message for status command.
 * \ingroup command_protocol
 */
class ServerStatus : public Status
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 5;

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<ServerStatus> Ptr;

    /*!
     * \brief Block status.
     */
    class Block
    {
    public:
        std::string _id;    //!< [in]
    
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _id;
        }
    };

    /*!
     * \brief Block container.
     */
    typedef std::vector<Block> Blocks;

    /*!
     * \brief
     */
    typedef std::vector<BGQDB::job::Id> Jobs;

    /*!
     * \brief Represents a command connection.
     */
    struct IoLink : public Status::Connection
    {
        bool _status;
        BG_UniversalComponentIdentifier _location;
        uint8_t _service;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & boost::serialization::base_object<Status::Connection>(*this);
            ar & _status;
            ar & _location;
            ar & _service;
        }
    };

    /*!
     * \brief I/O link container.
     */
    typedef std::vector<IoLink> IoLinks;

public:
    Jobs _jobs;
    Blocks _blocks;
    IoLinks _ioLinks;
    bool _simulation;
    size_t _jobCounters;
    size_t _miscCounters;
    uint8_t _jobctlProtocol;
    uint8_t _stdioProtocol;
    bool _realtime;
    unsigned _connectionPoolSize;
    unsigned _connectionPoolAvailable;
    unsigned _connectionPoolUsed;
    unsigned _connectionPoolMax;

public:
    /*!
     * brief Ctor.
     */
    ServerStatus() :
        Status( Message::Tag::ServerStatus ),
        _jobs(),
        _blocks(),
        _ioLinks(),
        _simulation( false ),
        _jobCounters( 0 ),
        _miscCounters( 0 ),
        _jobctlProtocol( 0 ),
        _stdioProtocol( 0 ),
        _realtime( false ),
        _connectionPoolSize( 0 ),
        _connectionPoolAvailable( 0 ),
        _connectionPoolUsed( 0 ),
        _connectionPoolMax( 0 )
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
        ar & boost::serialization::base_object<runjob::commands::response::Status>(*this);
        ar & _jobs;
        ar & _blocks;
        ar & _ioLinks;
        ar & _simulation;
        ar & _jobCounters;
        ar & _miscCounters;
        ar & _jobctlProtocol;
        ar & _stdioProtocol;
        ar & _realtime;
        ar & _connectionPoolSize;
        ar & _connectionPoolAvailable;
        ar & _connectionPoolUsed;
        ar & _connectionPoolMax;
    }
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::ServerStatus, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::ServerStatus, runjob::commands::response::ServerStatus::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::ServerStatus::Block, runjob::commands::ProtocolVersion )

#endif
