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
 * \page runjob_server
 *
 * BG/Q job submission server daemon
 *
 * \section SYNOPSIS
 * runjob_server [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * Provides job launch, management, and monitoring services.
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section OPTIONS
 *
 * \subsection mux_listen_port --mux-listen-port
 * address and port to bind to for multiplexer connections.  Enclose ipv6 addresses in square brackets. For
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
 * \subsection sim --sim
 *
 * Enable or diable job simulation.
 *
 * \subsection help -h,--help
 * 
 * display help text.
 *
 * \subsection version -v,--version
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
 * - \link runjob_mux runjob_mux \endlink
 */

#include "server/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <ras/include/RasEventHandlerChain.h>

#include <cerrno>
#include <iostream>
#include <sstream>

#include <sys/resource.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

Options::Options(
        unsigned int argc, 
        char** argv
        ) :
    AbstractOptions( argc, argv, runjob::server::log ),
    _options( "Options" ),
    _commandPort( defaults::ServerCommandService, "command", "command connections",  bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeCommand ),
    _muxPort( defaults::ServerMuxService, "mux", "multiplexer connections", bgq::utility::ServerPortConfiguration::ConnectionType::AdministrativeOnly ),
    _sim( defaults::ServerJobSimulation ),
    _threads( 0 ),
    _reconnect( )
{
    // create args
    _options.add_options()
        ("sim", po::value(&_sim)->implicit_value(true)->default_value(defaults::ServerJobSimulation), "Enable or disable job simulation")
        ;

    // create hidden args
    po::options_description hidden;
    hidden.add_options()
        ("reconnect", po::value(&_reconnect)->implicit_value(Reconnect::Scope::Jobs)->default_value(Reconnect::Scope::Jobs))
        ;

    // add port configuration
    _muxPort.addTo(_options, hidden);
    _commandPort.addTo(_options, hidden);

    // add generic args
    this->add(_options);

    // combine regular and hidden options
    po::options_description options;
    options.add( _options );
    options.add( hidden );
    
    // parse and notify
    this->parse( options );

    // notify
    this->notify();

    // set properties file in port configurations
    _commandPort.setProperties( this->getProperties(), runjob::server::PropertiesSection );
    _muxPort.setProperties( this->getProperties(), runjob::server::PropertiesSection );
    _commandPort.notifyComplete();
    _muxPort.notifyComplete();

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
   
    // setup some simulation environment stuff
    if ( _sim._value ) {
        this->setupSimulationEnvironment();
    }

    // set properties for RAS handlers
    RasEventHandlerChain::setProperties( this->getProperties() );
}

Options::~Options()
{
    LOG_TRACE_MSG( "resetting RasEventHandlerChain" );
    RasEventHandlerChain::setProperties( bgq::utility::Properties::ConstPtr() );
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
    } catch ( const std::invalid_argument& e ) {
        // this isn't fatal, we'll use the default
        LOG_DEBUG_MSG( "missing thread_pool_size key from " << PropertiesSection << " section in properties file" );
    } catch ( const boost::bad_lexical_cast& e ) {
        // garbage data
        LOG_WARN_MSG(e.what());
    }

    if ( !_vm["sim"].defaulted() ) {
        // given specific arg, use it
        LOG_DEBUG_MSG( "using job simulation argument value: " << std::boolalpha << _sim );
    } else {
        const std::string key( "job_sim" );
        try {
            const std::string value = this->getProperties()->getValue( PropertiesSection, key );
            std::istringstream is( value );
            is >> std::boolalpha >> _sim;
            if ( !is ) {
                LOG_WARN_MSG( "invalid " << key << " value from properties file: " << value );
                _sim = defaults::ServerJobSimulation;
            } else {
                LOG_DEBUG_MSG( "using job simulation properties value: " << std::boolalpha << _sim );
            }
        } catch ( const std::invalid_argument& e ) {
            // this isn't fatal, we'll use the default
            LOG_DEBUG_MSG( "missing " << key << " key from " << PropertiesSection << " section in properties file" );
        }
    }
}

void
Options::setupSimulationEnvironment()
{
    // verify iosd exists
    std::string iosd_path = BGQ_INSTALL_DIR + std::string("/ramdisk/bin");
    std::string iosd_name = "start_job_simulation";
    try {
        iosd_path = this->getProperties()->getValue(PropertiesSection, "iosd_sim_path");
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "iosd_sim_path not found, using default: " << iosd_path );
    }
    try {
        iosd_name = this->getProperties()->getValue(PropertiesSection, "iosd_sim_name");
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "iosd_sim_name not found, using default: " << iosd_name );
    }

    try {
        boost::filesystem::path path( iosd_path );
        path /= iosd_name;
        if ( !boost::filesystem::exists(path) ) {
            LOG_FATAL_MSG( "could not find iosd at " << iosd_path << "/" << iosd_name );
            LOG_FATAL_MSG( "verify the iosd_sim_path and iosd_sim_name keys in the properties file" );
            BOOST_THROW_EXCEPTION( boost::program_options::error("iosd") );
        }
    } catch ( const std::invalid_argument& e ) {
        LOG_FATAL_MSG( "could not find " << iosd_path << " or " << iosd_name << " key in " << PropertiesSection << " section of properties file" );
        BOOST_THROW_EXCEPTION( boost::program_options::error("iosd") );;
    }

    // get current nproc ceiling
    struct rlimit limit;
    memset( &limit, 0, sizeof(limit) );
    int rc = getrlimit( RLIMIT_NPROC, &limit );
    if ( rc == -1 ) {
        LOG_WARN_MSG( "rlimit( RLIMIT_NPROC ) failed: " << strerror(errno) );
    } else {
        LOG_DEBUG_MSG( "current max number of user processes: " << limit.rlim_cur );

        // set new maximum
        try {
            std::string max = this->getProperties()->getValue( PropertiesSection, "max_user_processes" );
            const int value = boost::lexical_cast<int>( max );
            if ( value > 0 ) {
                LOG_INFO_MSG( "setting max user processes to " << max );
                limit.rlim_cur = value;
            } else {
                LOG_INFO_MSG( "leaving RLIMIT_NPROC at " << limit.rlim_cur );
            }
        } catch ( const std::invalid_argument& e ) {
            LOG_WARN_MSG( "missing max_user_processes key from " << PropertiesSection << " of properties file" );
            LOG_WARN_MSG( "using " << defaults::ServerMaxUserProcesses << " as default" );
            limit.rlim_cur = defaults::ServerMaxUserProcesses;
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_WARN_MSG( "garbage data from max_user_processes key in properties file: " << e.what() );
            LOG_WARN_MSG( "using " << defaults::ServerMaxUserProcesses << " as default" );
            limit.rlim_cur = defaults::ServerMaxUserProcesses;
        }
        rc = setrlimit( RLIMIT_NPROC, &limit );
        if ( rc == -1 ) {
            LOG_WARN_MSG( "could not set maximum number of user processes: " << strerror(errno) );
        }
    }

    // cannot reconnect jobs in simulation
    if ( _reconnect.scope() == Reconnect::Scope::Jobs ) {
        _reconnect = Reconnect( Reconnect::Scope::Blocks );
        LOG_WARN_MSG( "job reconnection not supported in simulation" );
        LOG_WARN_MSG( "defaulting to block reconnection" );
    }
}

} // server
} // runjob
