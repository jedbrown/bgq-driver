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
#include "common/AbstractOptions.h"

#include "common/logging.h"

#include <hlcs/include/runjob/commands/Message.h>

#include <utility/include/performance.h>
#include <utility/include/UserId.h>
#include <utility/include/version.h>

#include <boost/asio/ip/host_name.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <fstream>
#include <iostream>

#include <sys/resource.h> // getrlimit

#include <unistd.h> // sysconf

namespace po = boost::program_options;

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

AbstractOptions::AbstractOptions(
        int argc,
        char** argv,
        const std::string& log_name
        ) :
    _vm(),
    _argc( argc ),
    _argv( argv ),
    _positionalArgs(),
    _properties(),
    _propertiesOptions(),
    _loggingOptions( std::string( "ibm." + log_name) )
{
    // parse --verbose and --properties early so we can initialize logging before
    // parsing the additional arguments
    boost::program_options::options_description options;
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
    this->openProperties();
    const std::string logging_section = "runjob";
    bgq::utility::initializeLogging( *_properties, _loggingOptions, logging_section );

    LOG_INFO_MSG( "using properties file " << _properties->getFilename() );

    std::ostringstream args;
    std::copy(_argv, _argv + _argc, std::ostream_iterator<char*>(args, " "));
    LOG_DEBUG_MSG( "startup parameters: " << args.str() );
}

AbstractOptions::~AbstractOptions()
{
    bgq::utility::performance::init( bgq::utility::Properties::ConstPtr() );
}

void
AbstractOptions::parse(
        boost::program_options::options_description& options,
        ExtraParser* extra
        )
{
    try {
        // create command line parser
        po::command_line_parser cmd_line( _argc, _argv );

        // assign options
        cmd_line.options( options );
        
        // assign positional args
        cmd_line.positional( _positionalArgs );

        // assign extra parser
        if ( extra ) {
            cmd_line.extra_parser( *extra );
        }

        // set our style
        cmd_line.style(
                po::command_line_style::allow_long | 
                po::command_line_style::long_allow_next |
                po::command_line_style::long_allow_adjacent |
                po::command_line_style::allow_short |
                po::command_line_style::short_allow_next |
                po::command_line_style::short_allow_adjacent |
                po::command_line_style::allow_dash_for_short
                );

        // run and store the results
        po::store( cmd_line.run(), _vm );
    } catch ( const boost::program_options::multiple_occurrences& e ) {
        std::cerr << e.what() << ": check that an option was not specified twice." << std::endl;
        throw;
    } catch ( const boost::program_options::too_many_positional_options_error& e ) {
        std::cerr << e.what() << ": check that an option name was not misspelled." << std::endl;
        throw;
    } catch ( const boost::program_options::error& e ) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

void
AbstractOptions::notify()
{
    try {
        // notify variables_map that we are done processing options
        boost::program_options::notify( _vm );
    } catch ( const boost::program_options::error& e ) {
        std::cerr << e.what() << std::endl;
        throw;
    } catch ( const std::invalid_argument& e ) {
        std::cerr << e.what() << std::endl;
        throw boost::program_options::invalid_option_value( "verbose" );
    }

    // initialize performance counter API
    bgq::utility::performance::init( _properties );

    this->logLimits();
}

void
AbstractOptions::openProperties()
{
    using namespace bgq::utility;

    try {
        if ( !_propertiesOptions.getFilename().empty() ) {
            _properties = bgq::utility::Properties::create( _propertiesOptions.getFilename() );
        } else {
            _properties = bgq::utility::Properties::create();
        }
    } catch ( const std::runtime_error& e ) {
        std::cerr << e.what() << std::endl;
        // translate this to a boost::po error since the contents have already been logged
        throw boost::program_options::error( e.what() );
    }
}

void
AbstractOptions::version(
        std::ostream& os
        ) const
{
    std::string basename = boost::filesystem::basename( boost::filesystem::path(_argv[0]) );
    os << "BG/Q " << basename << " " << bgq::utility::DriverName;
    os << " (revision " << bgq::utility::Revision << ")";
    os << " " << __DATE__ << " " << __TIME__;
    os << " pid " << getpid();
}

void
AbstractOptions::add(
        boost::program_options::options_description& options
        )
{
    options.add_options()
        ("help,h", boost::program_options::bool_switch(), "this help text")
        ("version,v", boost::program_options::bool_switch(), "display version information")
        ;

    // add properties options
    _propertiesOptions.addTo( options );

    // add logging options
    _loggingOptions.addTo( options );
}

unsigned
AbstractOptions::calculateWorkerThreads()
{
    const unsigned workers = boost::thread::hardware_concurrency();
    LOG_DEBUG_MSG( "calculated " << workers << " worker threads" );
    return workers;
}

std::string
AbstractOptions::hostname()
{
    boost::system::error_code error;
    const std::string host = boost::asio::ip::host_name( error );
    if ( error ) {
        LOG_WARN_MSG( 
                "could not get current host name: " << 
                boost::system::system_error( error ).what()
                );
    }

    return host;
}

std::string
AbstractOptions::credentials()
{
    try {
        bgq::utility::UserId uid;
        std::ostringstream os;
        os << "user=" << uid.getUser() << " uid=" << uid.getUid() << " groups:";
        BOOST_FOREACH( const auto& i, uid.getGroups() ) {
            os << " " << i.second << " (" << i.first << ")";
        }
        return os.str();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return std::string();
}


void
AbstractOptions::logLimits()
{
    LOG_INFO_MSG( "max open file descriptors: " << sysconf( _SC_OPEN_MAX) );

    struct rlimit core;
    memset( &core, 0, sizeof(core) );
    if ( getrlimit(RLIMIT_CORE, &core) == -1 ) {
        LOG_WARN_MSG( "could not get core file size rlimit" << strerror(errno) );
    } else {
        LOG_INFO_MSG( "core file limit: " << core.rlim_cur );
    }
}

} // runjob
