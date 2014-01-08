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
/*!
 * \page runjob_mux_status
 *
 * BG/Q displays runjob_mux status
 *
 * \section SYNOPSIS
 *
 * runjob_mux_status [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Displays runjob_mux status information.
 *
 * \section OPTIONS
 *
 * COMMON_ARGUMENTS_GO_HERE
 *
 * \section exit EXIT STATUS
 *
 * 0 on success, 1 otherwise.
 *
 * \section AUTHOR
 *
 * IBM
 *
 * \section copyright COPYRIGHT
 *
 * © Copyright IBM Corp. 2010, 2011
 *
 * \section also SEE ALSO
 *
 * - \link runjob_mux runjob_mux \endlink
 */

#include "mux/commands/status/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

LOG_DECLARE_FILE( runjob::mux::commands::log );

namespace runjob {
namespace mux {
namespace commands {
namespace status {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::MuxCommandService, runjob::mux::commands::log, runjob::commands::Message::Tag::MuxStatus, argc, argv ),
    _options( "Options" ),
    _io_service(),
    _resolver( _io_service )
{
    // add generic args
    Options::add(
            runjob::mux::commands::PropertiesSection,
            _options
            );
}

void
Options::doHelp(
        std::ostream& os
        ) const
{
    os << _options << std::endl;
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::MuxStatus::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::MuxStatus>(msg)
            );
    if ( !response ) {
        LOG_FATAL_MSG( "received unknown response type: " << runjob::commands::Message::toString(msg->getTag()) );
        return;
    }

    std::cout << "BG/Q runjob multiplexer" << std::endl;
    std::cout << "driver: " << response->getDriver() << std::endl;
    std::cout << "revision: " << response->getRevision() << std::endl;
    std::cout << "configured from: " << response->getProperties() << std::endl;
    std::cout << "load: " << response->getLoad() << " microseconds" << std::endl;
    std::cout << std::endl;

    std::cout << "runjob server: ";
    try {
        // resolve address
        const boost::asio::ip::tcp::resolver::iterator i(
                _resolver.resolve(
                    response->getServer()
                    )
                );
        std::cout << i->host_name();
    } catch ( const boost::system::system_error& e ) {
        std::cout << "not connected";
    }
    std::cout << std::endl << std::endl;

    std::cout << "scheduler plugin: ";
    if ( !response->getPlugin().empty() ) {
        std::cout << "loaded from: " << response->getPlugin();
    } else {
        std::cout << "not loaded";
    }
    std::cout << std::endl;
    std::cout << "bgsched version: " << response->_bgschedMajor << "." << response->_bgschedMinor << "." << response->_bgschedMod << std::endl;
    std::cout << std::endl;

    this->displayConnections( response );
    std::cout << std::endl;
    this->displayClients( response );
}

void
Options::displayConnections(
        const runjob::commands::response::MuxStatus::Ptr& response
        ) const
{
    runjob::commands::response::Status::Connections connections = response->getConnections();
    if ( connections.empty() ) {
        std::cout << "no connections" << std::endl;
        return;
    }
    std::cout << connections.size() << " connection" << (connections.size() == 1 ? "" : "s") << std::endl;

    // calculate maximum size of each value when converted to a string
    size_t host = strlen("Host");
    size_t type = strlen("Type");
    BOOST_FOREACH( runjob::commands::response::Status::Connections::value_type& connection, connections ) {
        // resolve address
        const boost::asio::ip::tcp::endpoint ep(
                boost::asio::ip::address::from_string( connection._address ),
                connection._port
                );
        const boost::asio::ip::tcp::resolver::iterator i(
                _resolver.resolve(
                    ep
                    )
                );
        if ( i != boost::asio::ip::tcp::resolver::iterator() ) {
            connection._address = i->host_name();
        }

        host = connection._address.size() > host ? connection._address.size() : host;
        type = connection._type.size() > type ? connection._type.size() : type;
    }
    
    // create format string using sizes previously calculated
    std::ostringstream formatting;
    formatting << "%-" << host + 1 << "s ";
    formatting << "%-5s ";
    formatting << "%-" << type + 1 << "s\n";
    std::cout << boost::format( formatting.str() ) % "Host" % "Port" % "Type";
    BOOST_FOREACH( const runjob::commands::response::Status::Connections::value_type& connection, connections ) {
        std::cout << boost::format( formatting.str() ) % connection._address % connection._port % connection._type;
    }
}

void
Options::displayClients(
        const runjob::commands::response::MuxStatus::Ptr& response
        ) const
{
    const runjob::commands::response::MuxStatus::Clients& clients = response->getClients();
    if ( clients.empty() ) {
        std::cout << "no clients connected" << std::endl;
        return;
    }
    std::cout << clients.size() << " client" << (clients.size() == 1 ? "" : "s") << std::endl;

    // calculate maximum size of each value when converted to a string
    size_t pid = strlen("pid");
    size_t id = strlen("Client");
    size_t user = strlen("Username");
    size_t job = strlen("Job");
    size_t size = strlen("Queue Size");
    size_t max = strlen("Max Size");
    size_t dropped = strlen("Dropped Messages");
    BOOST_FOREACH( const runjob::commands::response::MuxStatus::Clients::value_type& client, clients ) {
        const std::string pidString = boost::lexical_cast<std::string>( client._pid );
        pid = pidString.size() > pid ? pidString.size() : pid;
        const std::string idString = boost::lexical_cast<std::string>( client._id );
        id = idString.size() > id ? idString.size() : id;
        user = client._user.size() > user ? client._user.size() : user;
        const std::string jobString = boost::lexical_cast<std::string>( client._job );
        job = jobString.size() > job ? jobString.size() : job;
        const std::string sizeString = boost::lexical_cast<std::string>( client._queueSize );
        size = sizeString.size() > size ? sizeString.size() : size;
        const std::string maxString = boost::lexical_cast<std::string>( client._queueMaximumSize );
        max = maxString.size() > max ? maxString.size() : max;
        const std::string droppedString = boost::lexical_cast<std::string>( client._queueDropped );
        dropped = droppedString.size() > dropped ? droppedString.size() : dropped;
    }
    
    // create format string using sizes previously calculated
    std::ostringstream formatting;
    formatting << "%" << pid + 1 << "s ";
    formatting << "%" << id + 1 << "s ";
    formatting << "%-" << user + 1 << "s ";
    formatting << "%" << job + 1 << "s ";
    formatting << "%" << size + 1 << "s ";
    formatting << "%" << max + 1 << "s ";
    formatting << "%" << dropped + 1 << "s";
    LOG_DEBUG_MSG( boost::format( formatting.str() ) % "pid" % "Client" % "Username" % "Job" % "Queue Size" % "Max Size" % "Dropped Messages" );
    BOOST_FOREACH( const runjob::commands::response::MuxStatus::Clients::value_type& client, clients ) {
        LOG_DEBUG_MSG(
                boost::format(
                formatting.str()
                )
            % client._pid
            % client._id
            % client._user
            % client._job
            % client._queueSize
            % client._queueMaximumSize
            % client._queueDropped
            );
    }
}

} // status
} // commands
} // mux
} // runjob
