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
#include "mux/commands/start_tool/Debugger.h"

#include "common/logging.h"

#include <boost/numeric/conversion/cast.hpp>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::mux::commands::log );

/*!
 * \brief Variables for interacting with debuggers (ex: TotalView)
 *
 * These variables are from the mpi debug standard, for more information
 * see:
 *
 *  - http://www-unix.mcs.anl.gov/mpi/mpi-debug/ 
 *  - the BG/Q code development and tools interface (CDTI) document.
 */
extern "C" {

MPIR_PROCDESC* MPIR_proctable = NULL;   //!< storage for proctable
int MPIR_proctable_size = 0;            //!< storage for proctable size
volatile int MPIR_debug_state = 0;      //!< storage for debug state
volatile int MPIR_being_debugged = 0;   //!< storage for more debug state info 
int MPIR_i_am_starter = 1;              //!< let TotalView know start_tool is not part of the MPI job
char MPIR_executable_path[256];         //!< path to tool daemon
char MPIR_server_arguments[1024];       //!< Arguments to the tool daemon
char MPIR_subset_attach[4096];          //!< subset rank, see CDTI document for syntax

void
MPIR_Breakpoint()
{
    LOG_TRACE_MSG( "MPIR_Breakpoint" );
}

} // extern "C"

namespace runjob {
namespace mux {
namespace commands {
namespace start_tool {

Debugger::Debugger()
{

}

Debugger::~Debugger()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    // notify debugger that we are done
    MPIR_debug_state = static_cast<int>( State::Aborting );
    MPIR_Breakpoint();
}

bool
Debugger::attached() const
{
    return MPIR_being_debugged;
}

tool::Daemon
Debugger::tool() const
{
    tool::Daemon result;

    try {
        result.setExecutable( MPIR_executable_path );
        this->parseArguments( result );
        const tool::Subset subset( MPIR_subset_attach );
        result.setSubset( subset );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return result;
}

void
Debugger::parseArguments(
        tool::Daemon& tool
        ) const
{
    if ( MPIR_server_arguments[0] == '\0' ) {
        // no arguments given
        return;
    }

    tool::Daemon::Arguments args;
    std::string arg;
    for ( unsigned i = 0; i < sizeof(MPIR_server_arguments); ++i ) {
        char c = MPIR_server_arguments[i];
        if ( c == '\0' ) {
            // if this is the second null, we are done
            if ( i && MPIR_server_arguments[i-1] == '\0' ) {
                break;
            }

            args.push_back( arg );
            LOG_TRACE_MSG( "added arg: '" << arg << "'" );
            arg.clear();
        } else {
            arg.push_back( c );
        }
    }
    LOG_TRACE_MSG( args.size() << " arguments" );
    tool.setArguments( args );
}

void
Debugger::fillProctable(
        const std::string& exe,
        const message::Proctable::Ptr& response
        ) const
{
    const message::Proctable::Io& io = response->_io;

    // delete old proctable and allocate storage for new one
    delete [] MPIR_proctable;
    MPIR_proctable_size = boost::numeric_cast<int>( response->_proctable.size() );
    MPIR_proctable = new MPIR_PROCDESC[ MPIR_proctable_size ];
    LOG_TRACE_MSG( "allocated storage for " << MPIR_proctable_size << " entries" );

    typedef std::map<Uci,unsigned> PidMap;
    PidMap pidMap;

    BOOST_FOREACH( const auto& i, response->_proctable ) {
        const message::Proctable::Io::const_iterator io_node = io.find( i.io() );
        if ( io_node == io.end() ) {
            LOG_WARN_MSG( "could not find I/O node " << i.io() << " for rank " << i.rank() );
            continue;
        } else {
            MPIR_proctable[ i.rank() ].host_name = io_node->second.c_str(); 
            LOG_TRACE_MSG( "rank " << i.rank() << " uses I/O node " << i.io() << " at " << io_node->second );
        }

        MPIR_proctable[ i.rank() ].executable_name = exe.c_str();

        // add pid
        PidMap::iterator pid = pidMap.find( i.io() );
        if ( pid == pidMap.end() ) {
            pid = pidMap.insert(
                    std::make_pair( i.io(), 0 )
                    ).first;
        } else {
            pid->second += 1;
        }
        MPIR_proctable[ i.rank() ].pid = pid->second;
    }

    MPIR_debug_state = static_cast<int>( State::Spawned );
    MPIR_Breakpoint();
}

} // start_tool
} // commands
} // mux
} // runjob
