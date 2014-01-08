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
 * \page locate_rank
 * 
 * Locate a compute node location string given a job ID and MPI rank.
 *
 * \section SYNOPSIS
 *
 * locate_rank id rank [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * Locate a node using the job and the MPI rank. The job may be a currently active job, or
 * a job that has already terminated. This command accounts for the --mapping, --ranks-per-node,
 * --np, --corner, and --shape parameters to runjob to output the location string of the compute node
 * with the requested rank.
 *
 * \note For jobs using a mapping file (see runjob) this file must be readable by the runjob_server (typically
 * the bgqadmin uid) to correctly calculate location to rank mappings.
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section OPTIONS
 *
 * \subsection id --id.
 *
 * job ID. This parameter may be specified as the first positional argument.
 *
 * \subsection rank --rank
 *
 * MPI rank. This parameter may be specified as the second positional argument.
 *
 * \section examples EXAMPLES
 *
\verbatim
user@host ~> locate_rank 254 0
rank 0 from job 254 on block R00-M0-N00 used compute node: R00-M0-N00-J17

user@host ~> locate_rank 275 127
rank 127 from job 275 on block MY_BLOCK used compute node: R01-M1-N12-J11

user@host ~>
\endverbatim
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
 */

#include "server/commands/locate_rank/Options.h"

#include "common/commands/LocateRank.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace locate_rank {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::LocateRank, argc, argv ),
    _options( "Options" ),
    _rank()
{
    namespace po = boost::program_options;
    _options.add_options()
        ("id", po::value(&_job), "job ID")
        ("rank", po::value(&_rank), "rank")
        ;

    // id and rank are positional
    _positionalArgs.add( "id", 1 );
    _positionalArgs.add( "rank", 1 );

    // add generic args
    Options::add(
            runjob::server::commands::PropertiesSection,
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

const char*
Options::description() const
{
    return 
        "Locate a node using the job and MPI rank.\n"
        "\n"
        "Active jobs require Read authority.\n"
        "History jobs require ownership of the job."
        ;
}

void
Options::doValidate() const
{
    if ( _job == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'id'") );
    }

    if ( _vm["rank"].empty() ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'rank'") );
    }

    const runjob::commands::request::LocateRank::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::LocateRank>( this->getRequest() )
            );

    request->_job = _job;
    request->_rank = _rank;
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::LocateRank::Ptr response(
            boost::dynamic_pointer_cast<runjob::commands::response::LocateRank>(msg)
            );
    if ( !response ) {
        LOG_FATAL_MSG( "received unknown response type: " << runjob::commands::Message::toString(msg->getTag()) );
        return;
    }

    if ( !response->getMessage().empty() ) {
        std::cout << "Warning: " << response->getMessage() << std::endl;
    }

    std::cout << 
        "rank " << _rank << " " <<
        "from job " << _job << " " <<
        "on block " << response->_block << " " <<
        "used compute node: " << response->_location <<
        std::endl <<
        std::endl
        ;
}

} // locate_rank
} // commands
} // server
} // runjob
