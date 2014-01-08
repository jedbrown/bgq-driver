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
 * \page list_job_authority
 *
 * Get BG/Q job access control
 *
 * \section SYNOPSIS
 *
 * list_job_authority id [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Lists job information.
 *
 * \section OPTIONS
 *
 * \subsection id --id
 *
 * Job ID. This parameter may be specified as the first positional argument.
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
 * - \link runjob_server \endlink
 * - \link grant_job_authority \endlink
 * - \link list_job_authority \endlink
 */

#include "server/commands/list_job_authority/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/algorithm/string.hpp>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace list_job_authority {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::JobAcl, argc, argv ),
    _options( "Options" )
{
    namespace po = boost::program_options;
    _options.add_options()
        ("id", po::value(&_job), "job ID")
        ;

    _positionalArgs
        .add( "id", 1 )
        ;

    // add generic args
    Options::add(
            runjob::server::commands::PropertiesSection,
            _options
            );
}

const char*
Options::description() const
{
    return
        "Lists user and group authority to perform various actions on job <id>.\n"
        "\n"
        "Requires administrative authority or ownership of the job."
        ;
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
    if ( _job == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'id'") );
    }

    // cast to our type so we can set members
    const runjob::commands::request::JobAcl::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::JobAcl>( this->getRequest() )
            );

    request->_id = _job;
    request->_mode = runjob::commands::request::JobAcl::List;
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::JobAcl::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::JobAcl>( msg )
            );
    if ( !response ) {
        LOG_FATAL_MSG( "received unknown response type: " << runjob::commands::Message::toString(msg->getTag()) );
        return;
    }

    std::cout << std::endl;
    std::cout << "owner: " << response->_owner << std::endl;
    std::cout << std::endl;

    if ( response->_authorities.empty() ) return;

    // calculate maximum size of each value when converted to a string
    size_t user = strlen("User or Group");
    size_t action = strlen("Action");
    size_t source = strlen("Source");
    BOOST_FOREACH( const runjob::commands::response::JobAcl::Authority& authority, response->_authorities ) {
        user = authority._user.size() > user ? authority._user.size() : user;
        const std::string actionString = boost::lexical_cast<std::string>( authority._action );
        action = actionString.size() > action  ? actionString.size() : action;
        const std::string sourceString = boost::lexical_cast<std::string>( authority._source );
        source = sourceString.size() > source  ? sourceString.size() : source;
    }
    // create format string using sizes previously calculated
    std::ostringstream formatting;
    formatting << "%-" << user + 1 << "s ";
    formatting << "%-" << action + 1 << "s ";
    formatting << "%-" << source + 1 << "s\n";
    std::cout << boost::format(formatting.str()) % "User or Group" % "Action" % "Source";
    
    BOOST_FOREACH( const runjob::commands::response::JobAcl::Authority& authority, response->_authorities ) {
        std::cout << boost::format(formatting.str()) %
            authority._user %
            boost::lexical_cast<std::string>(authority._action) %
            boost::lexical_cast<std::string>(authority._source)
            ;
    }
    std::cout << std::endl;
}

} // list_job_authority
} // commands
} // server
} // runjob
