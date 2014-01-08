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
 * \page runjob_mux_log_level
 *
 * BG/Q displays and change runjob_mux log levels
 *
 * \section SYNOPSIS
 *
 * runjob_mux_log_level [OPTIONS] [logger=level [... logger=level]]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Display and change the logging levels of the runjob_mux. With no logger arguments,
 * the logger names and their levels of the runjob_mux are displayed. To set logger
 * levels, specify their name and a level as positional arguments. The valid logging levels
 * are the same values that are supported by the verbose argument.
 *
 * \section OPTIONS
 *
 * COMMON_ARGUMENTS_GO_HERE
 *
 * \section exit EXIT STATUS
 *
 * 0 on success, 1 otherwise.
 *
 * \section examples EXAMPLES
 *
 * runjob_mux_log_level
 *
 * to display all loggers and their levels.
 *
 * runjob_mux_log_level ibm.utility=TRACE
 *
 * updates the ibm.utility logger to TRACE level.
 *
 * runjob_mux_log_level ibm.runjob=WARN ibm.utility.performance=DEBUG
 *
 * updates the ibm.runjob logger to WARN level, and the ibm.utility.performance logger to DEBUG level.
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


#include "mux/commands/log_level/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/foreach.hpp>

#include <iostream>

namespace runjob {
namespace mux {
namespace commands {
namespace log_level {

LOG_DECLARE_FILE( runjob::mux::commands::log );

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::MuxCommandService, runjob::mux::commands::log, runjob::commands::Message::Tag::LogLevel, argc, argv ),
    _options(),
    _strings()
{
    _options.add_options()
        ("logging_setting", boost::program_options::value(&_strings) )
        ;

    // create positional arguments since our options are hidden
    _positionalArgs.add( "logging_setting", -1 );

    // add generic options
    Options::add(
            runjob::mux::commands::PropertiesSection,
            _options
            );
    
    const runjob::commands::request::LogLevel::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::LogLevel>( this->getRequest() )
            );

    request->setArgs( _strings );
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::LogLevel::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::LogLevel>( msg )
            );
    
    if ( response->getLoggers().empty() ) {
        std::cout << "logging levels updated" << std::endl;
    } else {
        BOOST_FOREACH( const runjob::commands::response::LogLevel::Logger& i, response->getLoggers() ) {
            std::cout <<
                std::setw(40) << std::left << 
                i._name << std::right <<
                i._level << 
                std::endl;
        }
    }
}

} // log_level
} // commands
} // mux
} // runjob
