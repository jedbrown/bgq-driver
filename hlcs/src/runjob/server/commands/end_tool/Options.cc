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
 * \page end_tool
 *
 * End a BG/Q tool daemon
 *
 * \section SYNOPSIS
 *
 * end_tool id [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * End a tool daemon on each I/O node in use by the job.
 *
 * \section OPTIONS
 *
 * \subsection pid --pid.
 *
 * runjob process ID. Either this parameter
 * or --id must be given.
 *
 * \subsection id --id
 *
 * Job ID. If given this value overrides the pid. This parameter may be specified as the first 
 * positional argument. 
 *
 * \subsection tool --tool
 *
 * tool ID.
 *
 * \subsection signal --signal
 *
 * The signal to send to the tool, if not specified the default is SIGTERM.
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
 * - \link start_tool \endlink
 * - \link tool_status \endlink
 */

#include "server/commands/end_tool/Options.h"

#include "common/commands/EndTool.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/foreach.hpp>

#include <csignal>
#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace end_tool {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::EndTool, argc, argv ),
    _pid( 0 ),
    _hostname( ),
    _options( "Options" ),
    _tool( 0 ),
    _signal( 0 )
{
    namespace po = boost::program_options;
    _options.add_options()
        ("pid", po::value(&_pid), "runjob process ID")
        ("id", po::value(&_job), "job ID")
        ("tool", po::value(&_tool), "tool ID to end")
        ("signal", po::value(&_signal)->default_value(SIGTERM), "signal to deliver")
        ;

    // --runjob-hostname is hidden
    boost::program_options::options_description hidden;
    hidden.add_options()
        ("runjob-hostname", po::value(&_hostname), "hostname where runjob was started")
        ;
    

    // id is positional
    _positionalArgs.add( "id", 1 );

    po::options_description both;
    both.add( _options );
    both.add( hidden );

    // add generic args
    Options::add(
            runjob::server::commands::PropertiesSection,
            both
            );
}

void
Options::doHelp(
        std::ostream& os
        ) const
{
    os << _options << std::endl;
}

const char*
Options::description() const
{
    return
        "end a tool daemon.\n"
        "\n"
        "Requires Execute authority."
        ;
}

void
Options::doValidate() const
{
    if ( _job == 0 && _pid == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'id'") );
    }
    if ( _tool == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'tool'") );
    }

    const runjob::commands::request::EndTool::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::EndTool>( this->getRequest() )
            );

    request->_job = _job;
    request->_tool = _tool;
    request->_signal = _signal;
    request->_pid = _pid;
    request->_hostname = _hostname;
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::EndTool::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::EndTool>(msg)
            );
    if ( !response ) {
        LOG_FATAL_MSG( "received unknown response type: " << runjob::commands::Message::toString(msg->getTag()) );
        return;
    }

    std::cout << "tool " << _tool << " ending" << std::endl;
}

} // end_tool
} // commands
} // server
} // runjob
