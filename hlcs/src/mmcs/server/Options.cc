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

#include "Options.h"

#include "common/Properties.h"

#include <control/include/mcServer/defaults.h>

#include <utility/include/Log.h>
#include <utility/include/version.h>

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <map>
#include <sstream>

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    _argc( argc ),
    _argv( argv ),
    _vm(),
    _options( "Options" ),
    _propertiesOptions(),
    _loggingOptions( "ibm.mmcs" ),
    _serverConfig( new bgq::utility::ServerPortConfiguration( "32031" ) ),
    _bringupOptions()
{
    namespace po = boost::program_options;

    // parse --verbose and --properties early so we can initialize logging before
    // parsing the additional arguments
    po::options_description options;
    _loggingOptions.addTo( options );
    _propertiesOptions.addTo( options );
    try {
        po::command_line_parser cmd_line( _argc, _argv );
        cmd_line.allow_unregistered();
        cmd_line.options( options );
        po::variables_map vm;
        po::store( cmd_line.run(), vm );
        po::notify(vm);
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << ": check that an option was not specified twice." << std::endl;
        throw boost::program_options::invalid_option_value( "verbose" );
    }
    const bgq::utility::Properties::ConstPtr properties( this->openProperties() );

    // Create hidden options
    po::options_description hidden;

    // Add port configuration options.
    _serverConfig->addTo( _options, hidden );

    // Add properties options
    _propertiesOptions.addTo( _options );

    // Create options
    _options.add_options()
        ("help,h", po::bool_switch(), "This help text")
        ("version,v", po::bool_switch(), "Display version information")
        ("iolog", po::value<std::string>()->notifier(boost::bind(&Properties::setProperty, MMCS_LOGDIR, _1)), "Directory for I/O node logs")
        ("mcserverip", po::value<std::string>()->notifier(boost::bind(&Properties::setProperty, MC_SERVER_IP, _1)), "mc_server IP address")
        ("mcserverport", po::value<std::string>()->notifier(boost::bind(&Properties::setProperty, MC_SERVER_PORT, _1)), "mc_server port number")
        ("bringup-option", po::value(&_bringupOptions), "Hardware bringup options, may be specified more than once")
        ("boot-timeout-and-free", po::value<std::string>()->notifier(boost::bind(&Properties::setProperty, WAIT_BOOT_FREE_TIME, _1)), "Minutes to wait for a boot to complete")
        ;

    // Add logging options
    _loggingOptions.addTo( _options );

    options.add( _options );
    options.add( hidden );

    // Parse
    po::command_line_parser cmd_line( _argc, _argv );
    cmd_line.options( options );
    try {
        po::store( cmd_line.run(), _vm );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    // Notify variables_map that we are done processing options
    po::notify( _vm );

    // Check for help
    if ( _vm["help"].as<bool>() ) {
        std::cout << _options << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Check for version
    if ( _vm["version"].as<bool>() ) {
        std::string basename = boost::filesystem::basename( boost::filesystem::path(_argv[0]) );
        std::cout << "BG/Q " << basename << " " << bgq::utility::DriverName;
        std::cout << " (revision " << bgq::utility::Revision << ")" << std::endl;
        std::cout << "built on " << __DATE__ << " at " << __TIME__ << std::endl;
        exit(EXIT_SUCCESS);
    }

    try {
        // Read the properties file for execution options
        LOG_INFO_MSG( "Configured from " << properties->getFilename() );
        Properties::init( properties->getFilename() );

        // Validate all of our options are sane
        this->validate();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Log each arg
    std::ostringstream args;
    std::copy(_argv, _argv + _argc, std::ostream_iterator<char*>(args, " "));
    LOG_INFO_MSG("Startup parameters: " << args.str());

    // Set version information
    std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
    std::ostringstream version;
    version << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    Properties::setProperty(MMCS_VERSION, version.str());
    Properties::setProperty(MMCS_PROCESS, basename);
    LOG_INFO_MSG( Properties::getProperty(MMCS_VERSION) );
}

bgq::utility::Properties::ConstPtr
Options::openProperties()
{
    bgq::utility::Properties::Ptr properties;
    try {
        if ( !_propertiesOptions.getFilename().empty() ) {
            properties = bgq::utility::Properties::create( _propertiesOptions.getFilename() );
        } else {
            properties = bgq::utility::Properties::create();
        }

        // Setup logging
        bgq::utility::initializeLogging( *properties, _loggingOptions, "mmcs" );

        _serverConfig->setProperties( properties, "mmcs" );
        _serverConfig->notifyComplete();
    } catch ( const std::runtime_error& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    return properties;
}

void
Options::validate()
{
    std::string parameter( "shutdown-timeout" );
    if ( _vm.count(parameter) ) {
        try {
            const int value = boost::lexical_cast<int>( _vm[parameter].as<std::string>() );
            if ( value <= 0 ) {
                BOOST_THROW_EXCEPTION( std::logic_error(parameter + " must be positive") );
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            BOOST_THROW_EXCEPTION( std::logic_error("Bad " + parameter + " value: " + _vm[parameter].as<std::string>()) );
        }
    }

    parameter = "iolog";
    if ( _vm.count(parameter) ) {
        if (access(_vm[parameter].as<std::string>().c_str(), F_OK | R_OK | W_OK | X_OK) == -1) {
            const int error = errno;
            BOOST_THROW_EXCEPTION(
                    std::logic_error(
                        "Cannot write to I/O log directory: " +
                        _vm[parameter].as<std::string>() + " (" +
                        boost::lexical_cast<std::string>(strerror(error)) + ")"
                        )
                    );
        }
    }

    parameter = "mcserverport";
    if ( _vm.count(parameter) ) {
        try {
            const int16_t value = boost::lexical_cast<int16_t>( _vm[parameter].as<std::string>() );
            if ( value <= 0 ) {
                BOOST_THROW_EXCEPTION( std::logic_error(parameter + " must be positive") );
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            BOOST_THROW_EXCEPTION( std::logic_error("Bad " + parameter + " value: " + _vm[parameter].as<std::string>()) );
        }
    }

    parameter = "boot-timeout-and-free";
    if ( _vm.count(parameter) ) {
        try {
            const int value = boost::lexical_cast<int>( _vm[parameter].as<std::string>() );
            if ( value <= 0 ) {
                BOOST_THROW_EXCEPTION( std::logic_error(parameter + " must be positive") );
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            BOOST_THROW_EXCEPTION( std::logic_error("Bad " + parameter + " value: " + _vm[parameter].as<std::string>()) );
        }
    }
}

} } // namespace mmcs::server
