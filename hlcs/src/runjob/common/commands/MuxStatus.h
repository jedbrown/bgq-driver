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
#ifndef RUNJOB_COMMANDS_MUX_STATUS_H
#define RUNJOB_COMMANDS_MUX_STATUS_H
/*!
 * \file runjob/commands/MuxStatus.h
 * \brief runjob::commands::request::MuxStatus and runjob::commands::response::MuxStatus definition and implementation.
 * \ingroup command_protocol
 */

#include <db/include/api/job/types.h>

#include "common/commands/Status.h"

#include <boost/asio.hpp>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for status command.
 * \ingroup command_protocol
 */
class MuxStatus : public Status
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<MuxStatus> Ptr;

public:
    /*!
     * \brief ctor.
     */
    MuxStatus() :
        Status( Message::Tag::MuxStatus )
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
class MuxStatus : public Status
{
public:
    /*!
     * \brief Protocol version.
     */
    static const unsigned ProtocolVersion = 2;

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<MuxStatus> Ptr;

    /*!
     * \brief Represents a runjob client.
     */
    class Client
    {
    public:
        Client() :
            _pid( 0 ),
            _user(),
            _job( 0 ),
            _id( 0 ),
            _queueSize( 0 ),
            _queueMaximumSize( 0 ),
            _queueDropped( 0 )
        {

        }

        pid_t _pid;                 //!< pid of the runjob process.
        std::string _user;          //!< username of the runjob process.
        BGQDB::job::Id _job;        //!< job ID.
        uint64_t _id;               //!< client ID.
        size_t _queueSize;          //!< output queue current size.
        size_t _queueMaximumSize;   //!< output queue maximum size.
        size_t _queueDropped;       //!< output queue number of dropped messages.

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _pid;
            ar & _user;
            ar & _job;
            ar & _id;
            ar & _queueSize;
            ar & _queueMaximumSize;
            ar & _queueDropped;
        }
    };

    /*!
     * \brief Client container.
     */
    typedef std::vector<Client> Clients;

public:
    /*!
     * brief Ctor.
     */
    MuxStatus() :
        Status( Message::Tag::MuxStatus ),
        _clients(),
        _serverAddress(),
        _serverPort( 0 ),
        _plugin(),
        _bgschedMajor( 0 ),
        _bgschedMinor( 0 ),
        _bgschedMod( 0 )
    {

    }

    /*!
     * \brief Add a client to the container.
     */
    void addClient(
            const Client& client    //!< [in]
            )
    {
        _clients.push_back( client );
    }

    /*!
     * \brief Set server information.
     */
    void setServer( const boost::asio::ip::tcp::endpoint& server ) { 
        _serverAddress = server.address().to_string();
        _serverPort = server.port();
    }

    /*!
     * \brief Set plugin information.
     */
    void setPlugin( const std::string& plugin ) { _plugin = plugin; }

    /*!
     * \brief Get the list of clients.
     */
    const Clients& getClients() const { return _clients; }

    /*!
     * \brief Get server information.
     */
    boost::asio::ip::tcp::endpoint getServer() const { 
        const boost::asio::ip::tcp::endpoint result(
                boost::asio::ip::address::from_string( _serverAddress ),
                _serverPort
                );

        return result;
    }

    /*!
     * \brief Get plugin information.
     */
    const std::string& getPlugin() const { return _plugin; }

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
        ar & _clients;
        ar & _serverAddress;
        ar & _serverPort;
        ar & _plugin;
        ar & _bgschedMajor;
        ar & _bgschedMinor;
        ar & _bgschedMod;
    }

public:
    Clients _clients;
    std::string _serverAddress;
    uint16_t _serverPort;
    std::string _plugin;
    unsigned _bgschedMajor;
    unsigned _bgschedMinor;
    unsigned _bgschedMod;
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::MuxStatus, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::MuxStatus, runjob::commands::response::MuxStatus::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::MuxStatus::Client, runjob::commands::ProtocolVersion )

#endif
