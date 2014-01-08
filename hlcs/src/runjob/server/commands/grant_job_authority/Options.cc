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
 * \page grant_job_authority
 *
 * Set job access control.
 *
 * \section SYNOPSIS
 *
 * grant_job_authority id user action [OPTIONS]
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
 * \subsection user --user
 *
 * The user ID to grant authority. This parameter may be specified as the second positional argument.
 *
 * \subsection action --action
 *
 * The action to revoke. Must be either read or execute. This parameter may be specified as the
 * third positional argument.
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
 * - \link revoke_job_authority \endlink
 * - \link list_job_authority \endlink
 */

#include "server/commands/grant_job_authority/Options.h"

#include "common/commands/JobAcl.h"
#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <hlcs/include/security/Types.h>

#include <boost/algorithm/string.hpp>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace grant_job_authority {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::JobAcl, argc, argv ),
    _options( "Options" ),
    _user(),
    _action( hlcs::security::Action::Invalid )
{
    namespace po = boost::program_options;
    _options.add_options()
        ("id", po::value(&_job), "job ID")
        ("user", po::value(&_user), "user or group name.")
        ("action", po::value(&_action), "read or execute.")
        ;

    _positionalArgs
        .add( "id", 1 )
        .add( "user", 1 )
        .add( "action", 1 )
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
        "Grants authority to perform <action> to <user> on job <id>.\n"
        "Actions may be 'read' or 'execute'. This does NOT affect special\n"
        "authorities created in the properties file.\n"
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

    if ( _user.getValue().empty() ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'user'") );
    }
    
    switch ( _action ) {
        case hlcs::security::Action::Read:
        case hlcs::security::Action::Execute:
            break;
        case hlcs::security::Action::Invalid:
            BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'action'") );
        default:
            BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value("action") );
    }

    const runjob::commands::request::JobAcl::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::JobAcl>( this->getRequest() )
            );

    request->_id = _job;
    request->_mode = runjob::commands::request::JobAcl::Grant;
    request->_user = _user;
    request->_action = _action;
}

} // grant_job_authority
} // commands
} // server
} // runjob
