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

#include "ChildProcesses.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>

#include <errno.h>
#include <signal.h>

#include <sys/wait.h>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace utility {


ChildProcesses::ChildProcesses(
        boost::asio::io_service& io_service
    ) :
        _io_service(io_service),
        _strand( _io_service )
{
    // Nothing to do.
}


ChildProcess::Ptr ChildProcesses::create(
        const std::string& name,
        const boost::filesystem::path& executable,
        const ChildProcess::Args& args
    )
{
    ChildProcess::Ptr ret( new ChildProcess( name, executable, args, *this, _io_service ) );
    return ret;
}


EasyChildProcess::Ptr ChildProcesses::createEasy(
        const std::string& name,
        const boost::filesystem::path& executable,
        const ChildProcess::Args& args
    )
{
    EasyChildProcess::Ptr ret( new EasyChildProcess( name, executable, args, *this, _io_service ) );

    return ret;
}


void ChildProcesses::notifySigChld()
{
    _strand.post(
            boost::bind( &ChildProcesses::_notifySigChldImpl, this )
        );
}


void ChildProcesses::waitForProcessToEnd(
        pid_t child_pid,
        const ProcessEndCallback& process_end_callback
    )
{
    _strand.post( boost::bind(
            &ChildProcesses::_waitForProcessToEndImpl,
            this,
            child_pid,
            process_end_callback
        ) );
}


void ChildProcesses::_childEnded(
        pid_t child_pid,
        const bgq::utility::ExitStatus& exit_status
    )
{
    _PidToCallback::iterator i(_pid_to_callback.find( child_pid ));

    if ( i == _pid_to_callback.end() ) {
        LOG_WARN_MSG( "Wasn't waiting for child process " << child_pid << " that ended" );
        _ended_processes[child_pid] = exit_status;
        return;
    }

    i->second( exit_status );

    _pid_to_callback.erase( i );
}


void ChildProcesses::_notifySigChldImpl()
{
    while ( true ) {
        int wait_stat(-1);

        pid_t waitpid_pid(waitpid( pid_t(-1), &wait_stat, WNOHANG ));
        if ( pid_t(-1) == waitpid_pid ) {
            int waitpid_errno(errno);
            if ( ECHILD == waitpid_errno ) {
                LOG_DEBUG_MSG( "No zombies waiting (ECHILD)" );
                break;
            }

            BOOST_THROW_EXCEPTION(
                    boost::system::system_error(
                            boost::system::errc::make_error_code(
                                    boost::system::errc::errc_t( waitpid_errno )
                                ),
                            "waitpid"
                        )
                    );
        }

        if ( static_cast<pid_t>(0) == waitpid_pid ) {
            LOG_DEBUG_MSG( "No zombies waiting." );
            break;
        }

        _childEnded( waitpid_pid, bgq::utility::ExitStatus( wait_stat ) );
    }
}


void ChildProcesses::_waitForProcessToEndImpl(
        pid_t child_pid,
        const ProcessEndCallback& process_end_callback
    )
{
    _EndedProcesses::iterator i(_ended_processes.find( child_pid ));

    if ( i == _ended_processes.end() ) {
        LOG_DEBUG_MSG( "Waiting for " << child_pid << " to end" );

        _pid_to_callback[child_pid] = process_end_callback;
        return;
    }

    LOG_DEBUG_MSG( "Process " << child_pid << " has already ended." );

    process_end_callback( i->second );

    _ended_processes.erase( i );
}


} // namespace bgws::utility
} // namespace bgws
