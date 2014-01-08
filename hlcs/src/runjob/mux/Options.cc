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
 * \page runjob_mux
 *
 * BG/Q job submission multiplexer daemon
 *
 * \section SYNOPSIS
 * runjob_mux [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * Multiplexes runjob clients into one connection to the runjob_server.
 * 
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section OPTIONS
 *
 * \subsection host --host
 *
 * runjob_server host and port to connect to.  Enclose ipv6 addresses in square brackets. For
 * link-local addresses, append the interface to the address after a % character.  Specify the
 * port after the interface using a colon.
 *
 * The following values are examples values:
 *
 * \if MAN
 * \verbinclude host_syntax.man
 * \else
 * \htmlinclude host_syntax.html
 * \endif
 *
 * \subsection command_listen_port --command-listen-port
 * address and port to bind to for command connections.  Enclose ipv6 addresses in square brackets. For
 * link-local addresses append the interface to the address after a % character.  Optionally, specify the
 * port after the address using a colon.
 *
 * The following values are examples values:
 *
 * \if MAN
 * \verbinclude host_syntax.man
 * \else
 * \htmlinclude host_syntax.html
 * \endif
 *
 * \subsection socket --socket
 *
 * Name of the runjob_mux listen socket, maximum length is UNIX_PATH_MAX characters.
 * This is stored in the abstract namespace rather than on the filesystem.
 *
 * \subsection help -h,--help
 *
 * display help text.
 *
 * \subsection version --version
 *
 * display version information
 *
 * \subsection properties --properties
 *
 * location of bg.properties file
 *
 * \subsection verbose --verbose
 * 
 * \if MAN
 * \verbinclude verbose.man
 * \else
 * \htmlinclude verbose.html
 * \endif
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
 * - \link runjob runjob \endlink
 * - \link runjob_server runjob_server \endlink
 */

#include "mux/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <utility/include/version.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <sstream>

namespace po = boost::program_options;

namespace runjob {
namespace mux {

LOG_DECLARE_FILE( runjob::mux::log );

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    AbstractOptions(argc, argv, runjob::mux::log ),
    _options( "Options" ),
    _clientPort( defaults::ServerMuxService, bgq::utility::ClientPortConfiguration::ConnectionType::Administrative ),
    _commandPort( defaults::MuxCommandService, "command", "command connections",  bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand ),
    _socket(),
    _threads( 0 )
{
    // create args
    _options.add_options()
        ("socket", po::value(&_socket)->default_value(runjob::defaults::MuxLocalSocket), "listen socket for runjob connections")
        ;

    // add port configuration
    boost::program_options::options_description hidden;
    _clientPort.addTo( _options );
    _commandPort.addTo( _options, hidden );

    // add generic args
    this->add(_options);

    // parse
    this->parse(_options);

    // notify
    this->notify();

    // set properties file in port configuration
    _clientPort.setProperties( this->getProperties(), runjob::mux::PropertiesSection );
    _clientPort.notifyComplete();
    _commandPort.setProperties( this->getProperties(), runjob::mux::PropertiesSection );
    _commandPort.notifyComplete();

    // log version information
    std::ostringstream version;
    this->version( version );
    LOG_INFO_MSG( version.str() );

    // log host name
    LOG_INFO_MSG( this->hostname() );
    LOG_INFO_MSG( this->credentials() );

    // combine properties values with options
    this->combineProperties();

    // calculate thread pool size
    if ( !_threads ) {
        _threads = this->calculateWorkerThreads();
    }
}

Options::~Options()
{
    LOG_TRACE_MSG( "terminating" );
}

void
Options::help(
        std::ostream& os
        ) const
{
    os << _options << std::endl;
}

void
Options::combineProperties()
{
    // get local socket
    if ( !_vm["socket"].defaulted() ) {
        // given specific arg, use it
    } else {
        // look in properties
        try {
            _socket = this->getProperties()->getValue(PropertiesSection, "local_socket");
            LOG_INFO_MSG( "set local socket to " << _socket << " from properties file ");
        } catch (const std::invalid_argument& e) {
            // this isn't fatal, we'll use the default
            LOG_WARN_MSG( "missing local_socket key from " << PropertiesSection << " section in properties file" );
        }
    }

    // look in properties for thread pool size
    try {
        const std::string key = "thread_pool_size";
        const std::string value = this->getProperties()->getValue( PropertiesSection, key );
        if ( value == "auto" ) {
            // default means to calculate it
        } else {
            _threads = boost::lexical_cast<int>( value );
            if ( _threads <= 0 ) {
                LOG_WARN_MSG( "invalid " << key << " value '" << value );
                _threads = 0;
            } else {
                LOG_INFO_MSG( "set thread pool size to " << _threads << " from properties file" );
            }
        }
    } catch (const std::invalid_argument& e) {
        // this isn't fatal, we'll use the default
        LOG_INFO_MSG("missing thread_pool_size key from " << PropertiesSection << " section in properties file");
    } catch (const boost::bad_lexical_cast& e) {
        // garbage data
        LOG_WARN_MSG(e.what());
    }
}

} // mux
} // runjob
