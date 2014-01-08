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
#include "client/Debugger.h"

#include "common/tool/Subset.h"

#include "common/message/StartTool.h"

#include "common/JobInfo.h"
#include "common/logging.h"

#include "client/MuxConnection.h"

#include <boost/numeric/conversion/cast.hpp>

#include <boost/asio/error.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/filesystem/path.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <unistd.h>

LOG_DECLARE_FILE( runjob::client::log );

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
int MPIR_i_am_starter = 1;              //!< let TotalView know runjob is not part of the MPI job
char MPIR_executable_path[256];         //!< path to tool daemon
char MPIR_server_arguments[1024];       //!< Arguments to the tool daemon
char MPIR_subset_attach[4096];          //!< subset rank, see CDTI document for syntax

void
MPIR_Breakpoint()
{
    LOG_TRACE_MSG( "MPIR_Breakpoint" );
}

} // extern "C"

namespace {
    std::string proctable_executable;
    runjob::message::Proctable::Io proctable_io;
}

namespace runjob {
namespace client {

Debugger::Ptr
Debugger::create(
        boost::asio::io_service& io_service
        )
{
    const Ptr result(
            new Debugger( io_service )
            );

    return result;
}

Debugger::Debugger(
        boost::asio::io_service& io_service
        ) :
    _mux( ),
    _timer( io_service ),
    _tool(),
    _being_debugged( 0 ),
    _stopped( false )
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
Debugger::attached() 
{
    _being_debugged = MPIR_being_debugged;
    return _being_debugged;
}

const tool::Daemon&
Debugger::tool()
{
    // remember what we currently have if setting anything throws
    tool::Daemon temp( _tool );

    try {
        _tool.setExecutable( MPIR_executable_path );
        this->parseArguments();
        const tool::Subset subset( MPIR_subset_attach );
        _tool.setSubset( subset );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );

        // reset contents to what we had
        _tool = temp;
    }

    return _tool;
}

void
Debugger::parseArguments()
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
    _tool.setArguments( args );
}

void
Debugger::fillProctable(
        const runjob::message::Proctable::ConstPtr& proctable,
        const JobInfo& info
        )
{
    // executable_name in proctable needs to be fully qualified
    proctable_executable = info.getExe();
    if ( !boost::filesystem::path(proctable_executable).is_complete() ) {
        std::string cwd( info.getCwd() );
        if ( cwd.at(cwd.size() - 1) != '/' ) cwd.push_back( '/' );
        proctable_executable.insert( 0, cwd );
    }

    // I/O node host names need to persist after MPIR_Breakpoint returns
    proctable_io = proctable->_io;

    // delete old proctable and allocate storage for new one
    delete [] MPIR_proctable;
    MPIR_proctable_size = boost::numeric_cast<int>( proctable->_proctable.size() );
    MPIR_proctable = new MPIR_PROCDESC[ MPIR_proctable_size ];
    LOG_TRACE_MSG( "allocated storage for " << MPIR_proctable_size << " entries" );

    BOOST_FOREACH( const auto& i, proctable->_proctable ) {
        const runjob::message::Proctable::Io::const_iterator io_node = proctable_io.find( i.io() );
        if ( io_node == proctable_io.end() ) {
            LOG_WARN_MSG( "could not find I/O node " << i.io() << " for rank " << i.rank() );
            continue;
        } else {
            MPIR_proctable[ i.rank() ].host_name = io_node->second.c_str(); 
            LOG_TRACE_MSG( "rank " << i.rank() << " uses I/O node " << i.io() << " at " << io_node->second );
        }

        MPIR_proctable[ i.rank() ].executable_name = proctable_executable.c_str();
        MPIR_proctable[ i.rank() ].pid = i.pid();
    }

    MPIR_debug_state = static_cast<int>( State::Spawned );
    MPIR_Breakpoint();
}

void
Debugger::handle(
        const runjob::message::Result::ConstPtr& result
        )
{
    LOG_WARN_MSG(
            "tool '" << MPIR_executable_path << "' failed to start: " << 
            error_code::toString( result->getError() )
            );
    LOG_WARN_MSG( result->getMessage() );

    MPIR_debug_state = static_cast<int>( State::Aborting );
    MPIR_Breakpoint();
}

void
Debugger::start(
        const boost::weak_ptr<MuxConnection>& mux
        )
{
    _mux = mux;
    this->wait();
}

void
Debugger::wait()
{
    _timer.expires_from_now( boost::posix_time::seconds(1) );

    _timer.async_wait(
            boost::bind(
                &Debugger::callback,
                this,
                boost::asio::placeholders::error
                )
            );
}

void
Debugger::stop()
{
    // canceling a deadline_timer requires using a _stopped flag since the handler can already be
    // queued for invoation prior to invoking cancel() see
    // http://www.boost.org/doc/libs/1_42_0/doc/html/boost_asio/reference/basic_deadline_timer/cancel/overload1.html
    boost::system::error_code error;
    _timer.cancel( error );
    _stopped = true;
    if ( error ) {
        LOG_TRACE_MSG( "cancel: " << boost::system::system_error(error).what() );
    }
}

void
Debugger::callback(
        const boost::system::error_code& error
        )
{
    LOG_TRACE_MSG( "callback" );

    if ( error == boost::asio::error::operation_aborted || _stopped) {
        // asked to terminate
        return;
    }

    if ( error ) {
        LOG_WARN_MSG( "could not wait: " << boost::system::system_error(error).what() );
        return;
    }

    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    if ( !_being_debugged && MPIR_being_debugged ) {
        LOG_TRACE_MSG( "debugger has attached" );
        _being_debugged = MPIR_being_debugged;

        const message::StartTool::Ptr start( new message::StartTool() );
        start->_description = this->tool();
        mux->write( start );
    } else if ( _being_debugged && !MPIR_being_debugged ) {
        LOG_TRACE_MSG( "debugger has detached" );
        _being_debugged = MPIR_being_debugged;
    }

    this->wait();
}

} // client
} // runjob

