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

#include <utility/include/Log.h>
#include <utility/include/version.h>

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <map>

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.console" );

namespace mmcs {
namespace console {

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
    _portConfig_ptr( new bgq::utility::ClientPortConfiguration( "32031" ) )
{
    std::string reconnect;

    // create options
    _options.add_options()
        ("help,h", boost::program_options::bool_switch(), "this help text")
        ("version,v", boost::program_options::bool_switch(), "display version information")
        ("auto-reconnect,r", boost::program_options::value(&reconnect), "automatic reconnect to mmcs_server, defaults to true")
        ("eof-exit,e", boost::program_options::bool_switch(), "terminate after reading a script" )
        ;

    // create hidden options
    boost::program_options::options_description hidden;
    hidden.add_options()
        ("no_shell,n", boost::program_options::bool_switch(), "disable shell commands")
        ("waitforserver,n", boost::program_options::bool_switch(), "")
        ;

    // add properties options
    _propertiesOptions.addTo( _options );

    // add logging options
    _loggingOptions.addTo( _options );

    // add port configuration options.
    _portConfig_ptr->addTo( _options );

    // combine regular and hidden args
    boost::program_options::options_description options;
    options.add( _options );
    options.add( hidden );

    // parse
    boost::program_options::command_line_parser cmd_line( _argc, _argv );
    cmd_line.options( options );
    cmd_line.positional( boost::program_options::positional_options_description() );
    boost::program_options::store( cmd_line.run(), _vm );

    // notify variables_map that we are done processing options
    boost::program_options::notify( _vm );

    // check for help
    if ( _vm["help"].as<bool>() ) {
        std::cout << _options << std::endl;
        exit(EXIT_SUCCESS);
    }

    // check for version
    if ( _vm["version"].as<bool>() ) {
        std::string basename = boost::filesystem::basename( boost::filesystem::path(_argv[0]) );
        std::cout << "BG/Q " << basename << " " << bgq::utility::DriverName;
        std::cout << " (revision " << bgq::utility::Revision << ")" << std::endl;
        exit(EXIT_SUCCESS);
    }

    this->openProperties();

    this->setupLoggingDefaults();

    Properties::setProperty(
            MMCS_PROCESS,
            boost::filesystem::basename(
                boost::filesystem::path( _argv[0])
                )
            );
    Properties::setProperty(
            MMCS_VERSION,
            bgq::utility::Revision
            );

    Properties::init(_propertiesOptions.getFilename(), Properties::console);

    // check for eofexit
    if ( _vm["eof-exit"].as<bool>() ) {
        Properties::setProperty(FILE_EXIT, "true");
    } else if ( !isatty(STDIN_FILENO) ) {
        // further backwards compatibility for scripting environments
        Properties::setProperty(FILE_EXIT, "true");
    }

    if (_vm["no_shell"].as<bool>()) {
        Properties::setProperty(NO_SHELL, "true");
    } else Properties::setProperty(NO_SHELL, "false");

    if (!reconnect.empty()) {
        if (reconnect == "true" || reconnect == "false")
            Properties::setProperty(AUTO_RECONNECT, reconnect.c_str());
        else {
            std::cerr << "Invalid argument '" << reconnect
                      << "' for auto-reconnect. Must be either 'true' or 'false'"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        Properties::setProperty(AUTO_RECONNECT, "true");
    }

    // log each arg
    std::ostringstream args;
    std::copy(_argv, _argv + _argc, std::ostream_iterator<char*>(args, " "));
    LOG_DEBUG_MSG("Startup parameters: " << args.str());
}

void
Options::openProperties()
{
    bgq::utility::Properties::Ptr properties;
    try {
        if ( !_propertiesOptions.getFilename().empty() ) {
            properties = bgq::utility::Properties::create( _propertiesOptions.getFilename() );
        } else {
            properties = bgq::utility::Properties::create();
        }

        // setup logging
        bgq::utility::initializeLogging( *properties, _loggingOptions );
    } catch ( const std::runtime_error& e ) {
        // eat this exception, we'll flag it later when opening Properties
    }


    if ( properties ) {
        _portConfig_ptr->setProperties( properties, "bg_console" );
        _portConfig_ptr->notifyComplete();
    }

}

void
Options::setupLoggingDefaults()
{
    // by default we want to set these loggers level to Fatal
    // unless they are overridden with the --verbose argument
    typedef std::map<std::string,log4cxx::LevelPtr> Loggers;
    Loggers loggers = boost::assign::map_list_of
            ( "ibm.mmcs_client", log4cxx::Level::getFatal() )
            ( "ibm.mmcs", log4cxx::Level::getFatal() )
            ( "ibm.mmcs.bg_console", log4cxx::Level::getError() )
            ( "ibm.utility", log4cxx::Level::getWarn() )
            ( "ibm.utility.cxxsockets", log4cxx::Level::getFatal() )
            ;

    bgq::utility::LoggingProgramOptions::Strings strings;
    if ( !_vm["verbose"].empty() ) {
        strings = _vm["verbose"].as<bgq::utility::LoggingProgramOptions::Strings>();
    }

    // parse --verbose arguments
    // they are in the form --verbose logger=level or --verbose level
    BOOST_FOREACH( const std::string& i, strings ) {
        const std::string::size_type split_pos( i.find( '=' ) );
        std::string logger_name;
        if ( split_pos != std::string::npos ) {
            logger_name = i.substr( 0, split_pos );
        } else {
            // assume level for default logger
            logger_name = "ibm.mmcs";
        }

        // remove any loggers specified in --verbose from the default logger list
        const Loggers::iterator verbose = loggers.find( logger_name );
        if ( verbose != loggers.end() ) {
            loggers.erase( verbose );
        }

    }

    // set logging levels
    for ( Loggers::const_iterator i = loggers.begin(); i != loggers.end(); ++i ) {
        const std::string& logger = i->first;
        const log4cxx::LevelPtr& level = i->second;
        if ( log4cxx::LoggerPtr log = log4cxx::Logger::getLogger( logger ) ) {
            log->setLevel( level );
        }
    }
}


} } // namespace mmcs::console
