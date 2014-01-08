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
 * \page runjob_mux_refresh_config
 * refreshes BG/Q runjob_mux configuration.
 *
 * \section SYNOPSIS
 *
 * runjob_mux_refresh_config [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Refreshes BG/Q runjob_mux configuration.
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

#include "mux/commands/refresh_config/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace commands {
namespace refresh_config {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::MuxCommandService, runjob::mux::commands::log, runjob::commands::Message::Tag::RefreshConfig, argc, argv ),
    _options( "Options" ),
    _file()
{
    // create program options
    _options.add_options()
        ("file", boost::program_options::value(&_file), "refresh config from this file")
        ;

    // file is positional
    _positionalArgs.add( "file", 1 );

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
Options::doValidate() const
{
    const runjob::commands::request::RefreshConfig::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::RefreshConfig>( this->getRequest() )
            );
    
    if ( !_file.empty() ) {
        LOG_DEBUG_MSG( "set filename " << _file );
        request->_filename = _file;
    }
}

} // refresh_config
} // commands
} // mux
} // runjob
