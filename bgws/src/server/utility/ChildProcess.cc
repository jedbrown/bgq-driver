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

#include "ChildProcess.hpp"

#include "ChildProcesses.hpp"
#include "Pipe.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>

#include <unistd.h>

#include <sys/prctl.h>


using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace utility {


ChildProcess::ChildProcess(
        const std::string& name,
        const boost::filesystem::path& exe,
        const Args& args,
        ChildProcesses& child_processes,
        boost::asio::io_service& io_service
    ) :
        _name(name),
        _exe(exe),
        _args(args),
        _child_processes(child_processes),
        _io_service(io_service),
        _pid(-1)
{
    // Nothing to do.
}


void ChildProcess::setPreExecFn( const PreExecFn& pre_exec_fn )
{
    _pre_exec_fn = pre_exec_fn;
}


Pipe::SdPtr ChildProcess::start(
        EndedCallback ended_cb
    )
{
    _ended_cb = ended_cb;

    // open pipes for stdin, stdout, stderr.

    Pipe stdin_pipe( _io_service );
    Pipe stdout_pipe( _io_service );
    Pipe stderr_pipe( _io_service );

    pid_t fork_rc(fork());

    if ( fork_rc == -1 ) {
        int fork_errno(errno);

        BOOST_THROW_EXCEPTION(
            boost::system::system_error(
                    boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( fork_errno )
                        ),
                    "failed to fork process"
                )
            );
    }

    if ( fork_rc == 0 ) {
        // Child process.

        string msg;
        int exit_status(EXIT_FAILURE);

        try {
            _childProcess(
                    stdin_pipe,
                    stdout_pipe,
                    stderr_pipe
                );
        } catch ( boost::system::system_error& se ) {
            if ( se.code().value() == ENOENT ) {
                msg = string() + "failed to execute " + _exe.file_string() + ", because the file does not exist";
                exit_status = 127;
            } else if ( se.code().value() == EACCES ) {
                msg = string() + "failed to execute " + _exe.file_string() + ", because do not have access or the file is not executable";
                exit_status = 126;
            } else {
                msg = string() + "failed to execute " + _exe.file_string() + ", " + se.what();
            }
        } catch ( std::exception& e ) {
            msg = string() + "failed to execute " + _exe.file_string() + ", " + e.what();
        } catch ( ... ) {
            msg = string() + "failed to execute " + _exe.file_string() + ", unexpected exception from pwauth child process";
        }

        std::cerr << msg << std::endl;

        _exit( exit_status );
    }

    // parent process.

    _pid = fork_rc;

    Pipe::SdPtr stdin_sd_ptr(_mainProcess(
            stdin_pipe,
            stdout_pipe,
            stderr_pipe
        ));

    return stdin_sd_ptr;
}


void ChildProcess::kill( int sig )
{
    int rc = ::kill( _pid, sig );

    if ( rc == 0 ) {
        return;
    }

    BOOST_THROW_EXCEPTION(
        boost::system::system_error(
                boost::system::errc::make_error_code(
                        boost::system::errc::errc_t( errno )
                    ),
                "kill"
            )
        );
}


void ChildProcess::_childProcess(
        Pipe& stdin_pipe,
        Pipe& stdout_pipe,
        Pipe& stderr_pipe
    )
{
    // Clean up fds, set up stdin, stdout, stderr for the proc, exec()

    int rc;

    // This should make sure that if the bgws server goes away, child process will too.
    rc = prctl( PR_SET_PDEATHSIG, SIGKILL ); // Ignoring rc since just doing this for convenience.


    // make stdin read pipe fd==STDIN_FILENO, stdout write pipe fd==STDOUT_FILENO, stderr write=STDERR_FILENO
    rc = dup2( stderr_pipe.writeSdPtr()->native(), STDERR_FILENO );
    if ( rc != STDERR_FILENO ) {
        exit( EXIT_FAILURE );
    }

    rc = dup2( stdout_pipe.writeSdPtr()->native(), STDOUT_FILENO );
    if ( rc != STDOUT_FILENO ) {
        int dup2_errno(errno);

        BOOST_THROW_EXCEPTION(
            boost::system::system_error(
                    boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( dup2_errno )
                        ),
                    "dup2( STDOUT )"
                )
            );

    }

    rc = dup2( stdin_pipe.readSdPtr()->native(), STDIN_FILENO );
    if ( rc != STDIN_FILENO ) {
        int dup2_errno(errno);

        BOOST_THROW_EXCEPTION(
            boost::system::system_error(
                    boost::system::errc::make_error_code(
                            boost::system::errc::errc_t( dup2_errno )
                        ),
                    "dup2( STDIN )"
                )
            );
    }


    sigset_t mask;
    sigemptyset( &mask );
    rc = pthread_sigmask( SIG_SETMASK, &mask, NULL ); // Ignoring rc, do this for convenience.

    if ( _pre_exec_fn ) {
        _pre_exec_fn();
    }

    string exe_str(_exe.file_string());

    vector<char*> args_cp;

    args_cp.push_back( strdup( exe_str.c_str() ) );
    BOOST_FOREACH( const string &s, _args ) {
        args_cp.push_back( strdup( s.c_str() ) );
    }
    args_cp.push_back( NULL );

    rc = execv( exe_str.c_str(), args_cp.data() );

    int execv_errno(errno);

    BOOST_THROW_EXCEPTION(
        boost::system::system_error(
                boost::system::errc::make_error_code(
                        boost::system::errc::errc_t( execv_errno )
                    ),
                "execv()"
            )
        );
}


Pipe::SdPtr ChildProcess::_mainProcess(
        Pipe& stdin_pipe,
        Pipe& stdout_pipe,
        Pipe& stderr_pipe
    )
{
    // Main process.

    LOG_INFO_MSG( *this << " Started " << _exe << (_args.empty() ? "" : " ") << boost::algorithm::join( _args, " " ) );

    _child_processes.waitForProcessToEnd(
            _pid,
            boost::bind(
                    &ChildProcess::_handleProcessEnded,
                    shared_from_this(),
                    _1
                )
        );

    _stdout_in_sd_ptr = stdout_pipe.readSdPtr();
    _stderr_in_sd_ptr = stderr_pipe.readSdPtr();

    return stdin_pipe.writeSdPtr();
}


void ChildProcess::_handleProcessEnded(
        const bgq::utility::ExitStatus& exit_status
    )
{
    LOG_INFO_MSG( *this << " exited with " << exit_status );

    _ended_cb( exit_status );
    _ended_cb = EndedCallback();
}


std::ostream& operator<<( std::ostream& os, const ChildProcess& cp )
{
    os << cp.getName() << "[" << cp.getPid() << "]";
    return os;
}

std::ostream& operator<<( std::ostream& os, const ChildProcess::ConstPtr& cp_ptr )
{
    os << *cp_ptr;
    return os;
}

std::ostream& operator<<( std::ostream& os, const ChildProcess::Ptr& cp_ptr )
{
    os << *cp_ptr;
    return os;
}


} } // namespace bgws::utility
