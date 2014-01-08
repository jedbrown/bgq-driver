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
 * \page tool_status
 *
 * Display tool status for a BG/Q job
 *
 * \section SYNOPSIS
 *
 * tool_status id [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Display status for tools associated with a job.
 *
 * \section OPTIONS
 *
 * \subsection pid --pid.
 *
 * runjob process ID. Either this parameter
 * or --id must be given.
 *
 * \note start_tool must be invoked from the same host that runjob was when using this parameter.
 * \subsection id --id
 *
 * Job ID. This parameter may be specified as the first positional argument.
 *
 * \subsection tool --tool
 *
 * tool ID. Omitting this parameter returns status for all tools associated with this job.
 *
 * \subsection subset --subset
 *
 * Display the subset specification for a tool.
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
 * - \link end_tool \endlink
 */

#include "server/commands/tool_status/Options.h"

#include "common/commands/ToolStatus.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace tool_status {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::ToolStatus, argc, argv ),
    _pid( 0 ),
    _hostname(),
    _options( "Options" ),
    _tool()
{
    namespace po = boost::program_options;
    _options.add_options()
        ("pid", po::value(&_pid), "runjob process ID")
        ("id", po::value(&_job), "job ID")
        ("tool", po::value(&_tool), "tool ID")
        ("subset", po::value(&_subset)->implicit_value(true)->default_value(false), "display subset information")
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
        "query the status of tool daemons.\n"
        "\n"
        "Requires Read authority.";
}

void
Options::doValidate() const
{
    if ( _job == 0 && _pid == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'id'") );
    }

    const runjob::commands::request::ToolStatus::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::ToolStatus>( this->getRequest() )
            );

    request->_job = _job;
    request->_tool = _tool;
    request->_pid = _pid;
    request->_hostname = _hostname;
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::ToolStatus::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::ToolStatus>(msg)
            );
    if ( !response ) {
        LOG_FATAL_MSG( "received unknown response type: " << runjob::commands::Message::toString(msg->getTag()) );
        return;
    }

    // get tool container
    const runjob::commands::response::ToolStatus::Tools& tools = response->_tools;
    std::cout << tools.size() << " tool" << (tools.size() == 1  ? "" : "s") << std::endl;
    std::cout << std::endl;

    if ( tools.empty() ) {
        return;
    }

    // calculate maximum size of each value when converted to a string
    size_t id = strlen("Id");
    size_t path = strlen("Path");
    size_t status = strlen("Status");
    size_t message = strlen("Message");
    size_t subset = strlen("Subset");
    size_t time = strlen("Start Time");
    BOOST_FOREACH( const auto& tool, tools ) {
        const std::string idString = boost::lexical_cast<std::string>( tool._id );
        id = idString.size() > id ? idString.size() : id;
        path = tool._path.size() > path ? tool._path.size() : path;
        status = tool._status.size() > status ? tool._status.size() : status;
        message = tool._error.size() > message ? tool._error.size() : message;
        subset = tool._subset.size() > subset ? tool._subset.size() : subset;

        const std::string timeString = boost::lexical_cast<std::string>( tool._timestamp );
        time = timeString.size() > time ? timeString.size() : time;
    }

    // create format string using sizes previously calculated
    std::ostringstream formatting;
    formatting << "%-" << id + 1 << "s ";
    formatting << "%-" << path + 1 << "s ";
    formatting << "%-" << status + 1 << "s ";
    formatting << "%-" << message + 1 << "s ";
    if ( _subset._value ) {
        formatting << "%-" << subset + 1 << "s ";
    }
    formatting << "%-" << time + 1 << "s\n";

    if ( _subset._value ) {
        std::cout << boost::format(formatting.str()) %
            "Id" %
            "Path" %
            "Status" %
            "Message" %
            "Subset" %
            "Start Time"
            ;
    } else {
        std::cout << boost::format(formatting.str()) %
            "Id" %
            "Path" %
            "Status" %
            "Message" %
            "Start Time"
            ;
    }

    BOOST_FOREACH( const auto& tool, tools ) {
        if ( _subset._value ) {
            std::cout << boost::format(formatting.str()) %
                tool._id %
                tool._path %
                tool._status %
                tool._error %
                tool._subset %
                tool._timestamp
                ;
        } else {
            std::cout << boost::format(formatting.str()) %
                tool._id %
                tool._path %
                tool._status %
                tool._error %
                tool._timestamp
                ;
        }
    }
}

} // tool_status
} // commands
} // server
} // runjob
