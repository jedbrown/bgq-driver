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
#ifndef RUNJOB_COMMANDS_STATUS_H
#define RUNJOB_COMMANDS_STATUS_H
/*!
 * \file runjob/commands/Status.h
 * \brief runjob::commands::request::Status and runjob::commands::response::Status definition and implementation.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <boost/serialization/vector.hpp>

#include <vector>

namespace runjob {
namespace commands {

namespace request {

/*!
 * \brief Request message for status command.
 * \ingroup command_protocol
 */
class Status : public Request
{
public:
    /*!
     * \brief ctor.
     */
    explicit Status(
            Message::Tag::Type tag  //!< [in]
            ) :
        Request( tag )
    {

    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Request>(*this);
    }
};

} // request

namespace response {

/*!
 * \brief Response message for status command.
 * \ingroup command_protocol
 */
class Status : public Response
{
public:
    /*!
     * \brief Represents a command connection.
     */
    struct Connection
    {
        std::string _address;  //!< address
        uint16_t _port; //!< port
        std::string _type;      //!< connection type
   
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _address;
            ar & _port;
            ar & _type;
        }
    };

    /*!
     * \brief Connection container.
     */
    typedef std::vector<Connection> Connections;

public:
    /*!
     * brief ctor.
     */
    explicit Status(
            Message::Tag::Type tag  //!< [in]
            ) :
        Response( tag ),
        _connections(),
        _driver(),
        _properties(),
        _revision( 0 ),
        _load( 0 )
    {

    }

    /*!
     * \brief dtor.
     */
    virtual ~Status() { }

    /*!
     * \brief Add a connection to the container.
     */
    void addConnection(
            const Connection& connection    //!< [in]
            )
    {
        _connections.push_back( connection );
    }

    void setDriver( const std::string& driver ) { _driver = driver; }   //!< Set driver name.
    void setProperties( const std::string& properties ) { _properties = properties; }   //!< Set properties path.
    void setRevision( uint32_t revision ) { _revision = revision; } //!< Set revision number.
    void setLoad( long load ) { _load = load; } //!< Set load.

    /*!
     * \brief Get the list of connections.
     */
    const Connections& getConnections() const { return _connections; }

    const std::string& getDriver() const { return _driver; }    //!< Get driver name.
    const std::string& getProperties() const { return _properties; }    //!< Get properties file.
    uint32_t getRevision() const { return _revision; }  //!< Get revision number.
    long getLoad() const { return _load; } //!< Get load.

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* version */)
    {
        ar & boost::serialization::base_object<Response>(*this);
        ar & _connections;
        ar & _driver;
        ar & _properties;
        ar & _revision;
        ar & _load;
    }

private:
    Connections _connections;   //!< container of connections.
    std::string _driver;        //!< BG/Q driver name.
    std::string _properties;    //!< path to properties file.
    uint32_t _revision;         //!< svn build revision.
    long _load;                 //!< load average of server
};

} // response
} // commands
} // runjob

BOOST_CLASS_VERSION( runjob::commands::request::Status, runjob::commands::ProtocolVersion )
BOOST_CLASS_VERSION( runjob::commands::response::Status, runjob::commands::ProtocolVersion )

#endif
