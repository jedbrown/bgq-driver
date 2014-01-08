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
 * \page cios_log_level
 *
 * Change Compute and I/O Services (CIOS) logging levels.
 *
 * \section SYNOPSIS
 *
 * cios_log_level [OPTIONS] [ibm.cios=level ibm.cios.jobctld=level ibm.cios.stdiod=level]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER.
 *
 * \section OPTIONS
 *
 * \subsection block --block
 *
 * The compute or I/O block ID to change. For I/O blocks, each I/O node will have its
 * logging levels updated. For compute blocks, each I/O node linked to the block will
 * have its levels updated. This argument is mutually exclusive with --location.
 *
 * \subsection location --location
 *
 * Update the logging levels on a specific I/O node. This argument is mutually exclusive
 * with --block.
 *
 * COMMON_ARGUMENTS_GO_HERE
 *
 * \section exit EXIT STATUS
 *
 * 0 on success, 1 otherwise.
 *
 * \section examples EXAMPLES
 *
 * cios_log_level --block R00-ID ibm.cios=T ibm.cios.jobctld=T ibm.cios.stdiod=TRACE
 *
 * Updates the logging levels for every I/O node in block R00-ID.
 *
 * cios_log_level --location R00-IC-J00 ibm.cios=WARN
 *
 * Updates just the ibm.cios logger for both the jobctld and stdiod for I/O node R00-IC-J00.
 *
 * cios_log_level --location R00-IC-J07 ibm.cios.jobctld=DEBUG
 *
 * Updates just the ibm.cios.jobctld logger for the jobctl daemon on R00-IC-J07.
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
 * - \link runjob_server runjob_server \endlink
 */

#include "server/commands/cios_log_level/Options.h"

#include "common/commands/ChangeCiosConfig.h"
#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/foreach.hpp>

#include <log4cxx/log4cxx.h>

#include <iostream>

namespace bgq {
namespace util {

std::istream&
operator>>(
        std::istream& stream,
        Location& location
        )
{
    std::string value;
    stream >> value;

    try {
        location.set( value );
        if ( location.getType() != Location::ComputeCardOnIoBoard ) {
            stream.setstate( std::ios::failbit );
        }
    } catch ( const std::exception& e ) {
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // util
} // bgq

namespace runjob {
namespace server {
namespace commands {
namespace cios_log_level {

std::string
getLevelString(
        const log4cxx::LevelPtr level
        )
{
    const int i = level->toInt();
    switch ( i ) {
        case log4cxx::Level::OFF_INT:      return "O";
        case log4cxx::Level::FATAL_INT:    return "F";
        case log4cxx::Level::ERROR_INT:    return "E";
        case log4cxx::Level::WARN_INT:     return "W";
        case log4cxx::Level::INFO_INT:     return "I";
        case log4cxx::Level::DEBUG_INT:    return "D";
        case log4cxx::Level::TRACE_INT:    return "T";
        case log4cxx::Level::ALL_INT:      return "A";
        default:                           return "";
    }
}

LOG_DECLARE_FILE( runjob::server::commands::log );

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::ChangeCiosConfig, argc, argv ),
    _options(),
    _block(),
    _location(),
    _strings()
{
    namespace po = boost::program_options;
    _options.add_options()
        ( "block", po::value(&_block), "block ID" )
        ( "location", po::value(&_location), "I/O node location" )
        ;

    po::options_description hidden;
    hidden.add_options()
        ("logging_setting", boost::program_options::value(&_strings) )
        ;

    po::options_description both;
    both.add( _options );
    both.add( hidden );

    _positionalArgs.add( "logging_setting", -1 );

    // add generic options
    Options::add(
            runjob::server::commands::PropertiesSection,
            both
            );
}

void
Options::doValidate() const
{
    const runjob::commands::request::ChangeCiosConfig::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::ChangeCiosConfig>( this->getRequest() )
            );

    if ( _vm["block"].empty() && _vm["location"].empty() ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing option 'block' or 'location'") );
    } else if ( !_vm["block"].empty() && !_vm["location"].empty() ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("both 'block' and 'location' specified") );
    }
    request->_block = _block;
    request->_location = _location;

    // validate logging level parameters
    const std::string default_logger( "ibm.cios" );
    bgq::utility::LoggingProgramOptions lpo( default_logger );
    lpo.notifier( _strings );

    BOOST_FOREACH( const std::string& i, _strings ) {
        const std::string::size_type split_pos( i.find( '=' ) );
        if ( split_pos != std::string::npos ) {
            LOG_TRACE_MSG( i );
            const std::string logger_name = i.substr( 0, split_pos );
            const log4cxx::LevelPtr level(
                    bgq::utility::LoggingProgramOptions::parseVerboseArgument(
                        i.substr(split_pos + 1)
                        )
                    );
            BOOST_ASSERT( level );
            if ( logger_name == "ibm.cios" ) {
                request->_common = getLevelString( level );
            } else if ( logger_name == "ibm.cios.jobctld" ) {
                request->_jobctl = getLevelString( level );
            } else if ( logger_name == "ibm.cios.stdiod" ) {
                request->_stdio = getLevelString( level );
            } else {
                BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value(
                            logger_name + " is not a valid cios logger"
                            )
                        );
            }
        } else {
            // assume level for default logger
            LOG_TRACE_MSG( "default logger: " << default_logger );
        }
    }

    if ( 
            request->_common.empty() &&
            request->_jobctl.empty() &&
            request->_stdio.empty()
       )
    {
        BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value(
                    "missing logger name and level"
                    )
                );
    }

    LOG_TRACE_MSG( "common: " << request->_common );
    LOG_TRACE_MSG( "jobctl: " << request->_jobctl );
    LOG_TRACE_MSG( "stdio:  " << request->_stdio );
}

std::string
Options::usage() const
{
    std::string result = runjob::commands::Options::usage();
    result.append( " [ibm.cios=level ibm.cios.jobctld=level ibm.cios.stdiod=level]" );

    return result;
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr&
        ) const
{
    std::cout << "logging levels updated" << std::endl;
}

void
Options::doHelp(
        std::ostream& os
        ) const
{
    os << _options << std::endl;

    os << "See the man page for logging level syntax." << std::endl;
}

} // cios_log_level
} // commands
} // server
} // runjob
