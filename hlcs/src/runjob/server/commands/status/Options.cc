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
 * \page runjob_server_status
 * displays BG/Q runjob_server status
 *
 * \section SYNOPSIS
 *
 * runjob_server_status [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Displays runjob_server status information.
 *
 * \section OPTIONS
 *
 * \subsection io --io-details
 *
 * Display detailed information about connections to the CIOS daemons on the I/O nodes.
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
 * - \link runjob runjob \endlink
 * - \link runjob_server runjob_server \endlink
 */

#include "server/commands/status/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"
#include "common/Uci.h"

#include <ramdisk/include/services/MessageHeader.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace status {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::ServerStatus, argc, argv ),
    _options( "Options" ),
    _ioDetails( false )
{

    _options.add_options()
        ("io-details", boost::program_options::value(&_ioDetails)->implicit_value(true), "Display detailed I/O link status")
        ;

    // id is positional
    // add generic args
    Options::add(
            runjob::server::commands::PropertiesSection,
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
    const runjob::commands::response::ServerStatus::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::ServerStatus>(msg)
            );
    if ( !response ) {
        LOG_FATAL_MSG( "received unknown response type: " << runjob::commands::Message::toString(msg->getTag()) );
        return;
    }

    std::cout << "BG/Q runjob server" << std::endl;
    std::cout << "driver: " << response->getDriver() << std::endl;
    std::cout << "revision: " << response->getRevision() << std::endl;
    std::cout << "configured from: " << response->getProperties() << std::endl;
    std::cout << "load: " << response->getLoad() << " microseconds" << std::endl;
    if ( response->_simulation ) {
        std::cout << "simulation enabled" << std::endl;
    }
    std::cout << (response->_realtime ? "realtime" : "polling") << " block notifications" << std::endl;
    std::cout << std::endl;

    this->displayConnectionPool( response );

    this->displayCiosProtocol( response );

    this->displayPerformanceCounters( response );

    this->displayConnections( response );

    this->displayJobs( response );

    this->displayBlocks( response );

    this->displayIoLinks( response );
}

void
Options::displayConnectionPool(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    std::cout << "CIOS protocol" << std::endl;
    std::cout << static_cast<unsigned>(response->_jobctlProtocol) << " job control" << std::endl;
    std::cout << static_cast<unsigned>(response->_stdioProtocol) << " standard I/O" << std::endl;
    std::cout << std::endl;
}

void
Options::displayCiosProtocol(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    std::cout << response->_connectionPoolAvailable << " database connections available" << std::endl;
    std::cout << response->_connectionPoolUsed << " database connections used" << std::endl;
    std::cout << response->_connectionPoolSize << " configured pool size" << std::endl;
    std::cout << response->_connectionPoolMax << " maximum database connections" << std::endl;
    std::cout << std::endl;
}

void
Options::displayPerformanceCounters(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    std::cout << "Performance Counters" << std::endl;
    std::cout << response->_jobCounters << " jobs" << std::endl;
    std::cout << response->_miscCounters << " miscellaneous" << std::endl;
    std::cout << std::endl;
}

void
Options::displayConnections(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    const runjob::commands::response::Status::Connections& connections = response->getConnections();

    const std::size_t mux_total = std::count_if(
            connections.begin(),
            connections.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &runjob::commands::response::Status::Connection::_type,
                    _1
                    ),
                "mux"
                )
            );

    const std::size_t command_total = std::count_if(
            connections.begin(),
            connections.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &runjob::commands::response::Status::Connection::_type,
                    _1
                    ),
                "command"
                )
            );

    std::cout << mux_total << " mux connection" << (mux_total == 1 ? "" : "s") << std::endl;
    std::cout << command_total << " command connection" << (command_total == 1 ? "" : "s") << std::endl;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver( io_service );

    BOOST_FOREACH( const runjob::commands::response::Status::Connections::value_type& connection, connections ) {
        // resolve address
        const boost::asio::ip::tcp::endpoint ep(
                boost::asio::ip::address::from_string( connection._address ),
                connection._port
                );
        const boost::asio::ip::tcp::resolver::iterator i(
                resolver.resolve(
                    ep
                    )
                );

        LOG_DEBUG_MSG( 
                ( i != boost::asio::ip::tcp::resolver::iterator() ? i->host_name() : connection._address ) <<
                "\t" << connection._port <<
                "\t" <<  connection._type
                )
    }
}

void
Options::displayJobs(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    const runjob::commands::response::ServerStatus::Jobs& jobs = response->_jobs;
    std::cout << std::endl;
    std::cout << jobs.size() << " job" << ( jobs.size() == 1 ? "" : "s" ) << std::endl;

    BOOST_FOREACH( const runjob::commands::response::ServerStatus::Jobs::value_type& job, jobs ) {
        LOG_DEBUG_MSG( job )
    }
}

void
Options::displayBlocks(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    const runjob::commands::response::ServerStatus::Blocks& blocks = response->_blocks;
    std::cout << std::endl;
    std::cout << blocks.size() << " block" << ( blocks.size() == 1 ? "" : "s" ) << std::endl;

    BOOST_FOREACH( const runjob::commands::response::ServerStatus::Blocks::value_type& block, blocks ) {
        LOG_DEBUG_MSG( block._id );
    }
}

void
Options::displayIoLinks(
        const runjob::commands::response::ServerStatus::Ptr& response
        ) const
{
    const runjob::commands::response::ServerStatus::IoLinks& links = response->_ioLinks;
    std::cout << std::endl;
    std::cout << links.size() << " I/O link" << ( links.size() == 1 ? "" : "s" ) << std::endl;
    if ( links.empty() ) return;

    const std::size_t jobctl_total = std::count_if(
            links.begin(),
            links.end(),
            boost::bind(
                std::equal_to<uint8_t>(),
                boost::bind(
                    &runjob::commands::response::ServerStatus::IoLink::_service,
                    _1
                    ),
                bgcios::JobctlService
                )
            );

    const std::size_t jobctl_up = std::count_if(
            links.begin(),
            links.end(),
            boost::bind(
                &runjob::commands::response::ServerStatus::IoLink::_status,
                _1
                ) &&
            boost::bind(
                std::equal_to<uint8_t>(),
                boost::bind(
                    &runjob::commands::response::ServerStatus::IoLink::_service,
                    _1
                    ),
                bgcios::JobctlService
                )
            );
    const std::size_t jobctl_down = jobctl_total - jobctl_up;
    std::cout <<
        jobctl_up << " jobctl link" << ( jobctl_up == 1 ? "" : "s" ) << " up " <<
        jobctl_down << " link" << ( jobctl_down == 1 ? "" : "s" ) << " down" << 
        std::endl;

    const std::size_t stdio_up = std::count_if(
            links.begin(),
            links.end(),
            boost::bind(
                &runjob::commands::response::ServerStatus::IoLink::_status,
                _1
                ) &&
            boost::bind(
                std::equal_to<uint8_t>(),
                boost::bind(
                    &runjob::commands::response::ServerStatus::IoLink::_service,
                    _1
                    ),
                bgcios::StdioService
                )
            );
    const std::size_t stdio_down = links.size() - jobctl_total - stdio_up;
    std::cout <<
        stdio_up << " stdio link" << ( stdio_up == 1 ? "" : "s" ) << " up " <<
        stdio_down << " link" << ( stdio_down == 1 ? "" : "s" ) << " down" <<
        std::endl;

    if ( !_ioDetails._value ) return;

    // calculate maximum size of each value when converted to a string
    size_t location = strlen("Location");
    size_t endpoint = strlen("Endpoint");
    size_t service = strlen("Service");
    size_t status = strlen("Status");
    BOOST_FOREACH( const runjob::commands::response::ServerStatus::IoLinks::value_type& link, links ) {
        const Uci uci( link._location );
        const std::string uci_string( boost::lexical_cast<std::string>(uci) );
        location = uci_string.size() > location ? uci_string.size() : location;

        const boost::asio::ip::tcp::endpoint ep(
                boost::asio::ip::address::from_string( link._address ),
                link._port
                );
        const std::string endpointString = boost::lexical_cast<std::string>( ep );
        endpoint = endpointString.size() > endpoint ? endpointString.size() : endpoint;

        switch ( link._service ) {
            case bgcios::JobctlService:
                service = strlen("jobctl") > service ? strlen("jobctl") : service;
                break;
            case bgcios::StdioService:
                service = strlen("stdio") > service ? strlen("stdio") : service;
                break;
            default:
                service = strlen("unknown") > service ? strlen("unknown") : service;
                break;
        }
    }

    // create format string using sizes previously calculated
    std::ostringstream formatting;
    formatting << "%-" << location + 1 << "s ";
    formatting << "%-" << endpoint + 1 << "s ";
    formatting << "%-" << service + 1 << "s ";
    formatting << "%-" << status + 1 << "s";
    formatting << "\n";

    std::cout << boost::format( formatting.str() ) % "Location" % "Endpoint" % "Service" % "Status";
    BOOST_FOREACH( const runjob::commands::response::ServerStatus::IoLinks::value_type& link, links ) {
        const boost::asio::ip::tcp::endpoint ep(
                boost::asio::ip::address::from_string( link._address ),
                link._port
                );
        const Uci uci( link._location );
        boost::format formatter( formatting.str() );
        formatter %
            boost::lexical_cast<std::string>(uci) %
            boost::lexical_cast<std::string>(ep);
        switch( link._service ) {
            case bgcios::JobctlService:
                formatter % "jobctl";
                break;
            case bgcios::StdioService:
                formatter % "stdio";
                break;
            default:
                formatter % "unknown";
        }

        formatter % (link._status ? "up" : "down");

        std::cout << formatter;
    }
}

} // status
} // commands
} // server
} // runjob
