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
 * \page dump_proctable
 *
 * Displays job rank to I/O node information.
 *
 * \section SYNOPSIS
 *
 * dump_proctable id [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section OPTIONS
 *
 * \subsection id --id
 *
 * Job id to query.
 *
 * \subsection rank --rank
 *
 * Query for specific ranks, this option may be specified multiple times. If not specified, all
 * ranks are displayed.
 *
 * \subsection show-location --show-location
 * 
 * In addition to I/O node IP addresses, show their location. Defaults to false.
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
 * - \link runjob_server runjob_server \endlink
 * - \link start_tool \endlink
 */


#include "server/commands/dump_proctable/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace dump_proctable {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::DumpProctable, argc, argv ),
    _options("Options"),
    _ranks(),
    _showLocation( false )
{
    namespace po = boost::program_options;
    _options.add_options()
        ("id", po::value(&_job), "job ID")
        ("rank", po::value(&_ranks), "rank to query")
        ("show-location", po::value(&_showLocation)->implicit_value(true)->default_value(false), "show I/O node location")
        ;

    // id is positional
    _positionalArgs.add( "id", 1 );

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
        "Get rank to I/O node mapping information for job <id>.\n"
        "\n"
        "Requires Read authority.";
}

void
Options::doValidate() const
{
    if ( _job == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'id'") );
    }

    const runjob::commands::request::DumpProctable::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::DumpProctable>( this->getRequest() )
            );

    request->_job = _job;
    BOOST_FOREACH( Rank::Type rank, _ranks ) {
        request->_ranks.push_back( rank );
    }
}

void
Options::doHandle(
        const runjob::commands::Response::Ptr& msg
        ) const
{
    const runjob::commands::response::DumpProctable::Ptr response(
            boost::static_pointer_cast<runjob::commands::response::DumpProctable>( msg )
            );
    std::cout << "block          : " << response->_block << std::endl;
    if ( !response->_corner.empty() && !response->_shape.empty() ) {
        std::cout << "shape          : " << response->_shape << std::endl;
        std::cout << "corner         : " << response->_corner << std::endl;
    }
    std::cout << "np             : " << response->_np << std::endl;
    std::cout << "ranks per node : " << response->_ranksPerNode << std::endl;
    std::cout << "mapping        : " << response->_mapping << std::endl;

    std::cout << std::endl;

    // build formatting string
    // maximum rank is 7 digits long
    // maximum IP address is 39 characters: 128 bit hex + 7 colon separators
    std::ostringstream formatting;
    if ( !_showLocation._value ) {
        formatting << "%-7s  %-39s 0x%08x\n";
        std::cout << "Rank     I/O node IP address                     pid" << std::endl; 
    } else {
        formatting << "%-7s %-18s %-39s 0x%08x\n";
        std::cout << "Rank    I/O location       I/O node IP address                     pid" << std::endl; 
    }

    for (
            runjob::tool::Proctable::const_iterator i = response->_proctable.begin();
            i != response->_proctable.end();
            ++i
        ) 
    {
        const size_t rank = i->rank();
        const unsigned pid = i->pid();

        // filter out specific ranks if asked
        if ( !_ranks.empty() ) {
            if ( std::find( _ranks.begin(), _ranks.end(), rank ) == _ranks.end() ) {
                continue;
            }
        }

        const runjob::commands::response::DumpProctable::Io::const_iterator io = response->_io.find( i->io() );
        if ( io == response->_io.end() ) {
            LOG_WARN_MSG( "could not find I/O node " << std::hex << i->io() );
            continue;
        }
        const std::string& ip = io->second;

        if ( !_showLocation._value ) {
            std::cout <<
                boost::format( formatting.str() ) %
                rank %
                ip %
                pid
                ;
        } else {
            std::cout <<
                boost::format( formatting.str() ) %
                rank %
                i->io() %
                ip %
                pid
                ;
        }
    }
}

} // dump_proctable
} // commands
} // server
} // runjob
