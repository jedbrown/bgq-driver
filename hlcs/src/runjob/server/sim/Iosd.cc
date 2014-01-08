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
#include "server/sim/Iosd.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/properties.h"

#include "server/cios/Connection.h"

#include "server/sim/SharedMemory.h"
#include "server/sim/Inotify.h"

#include "server/Options.h"

#include <utility/include/Log.h>

#include <boost/filesystem.hpp>

#include <iostream>

#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace sim {

// anonymous namespace for simulation ID counter
namespace {
boost::shared_ptr<SharedMemory> _simCounter;
}

void
Iosd::setCounter(
        boost::shared_ptr<SharedMemory> counter
        )
{
    _simCounter = counter;
}

Iosd::Iosd(
        const Inotify::Ptr& iNotify,
        const Options& options,
        const std::string& location,
        const std::string& path,
        const std::string& name
        ) :
    _iNotify( iNotify ),
    _options( options ),
    _location( location ),
    _path( path ),
    _name( name ),
    _pid( 0 ),
    _id( _simCounter->increment() )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_DEBUG_MSG( _id );
}

void
Iosd::fork()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );

    std::ostringstream log;
    log << "/tmp/cios" << _id << "/" << _location << ".log";
    LOG_INFO_MSG( "using log file " << log.str() );

    // log path, name, and id
    LOG_DEBUG_MSG( "path   " << _path );
    LOG_DEBUG_MSG( "name   " << _name );

    // fork
    _pid = ::fork();
    if ( _pid == -1 ) {
        // fork error
        int error = errno;
        char buf[256];
        LOG_RUNJOB_EXCEPTION( 
                error_code::block_invalid,
                "could not fork " << error << " (" << strerror_r(error, buf, sizeof(buf)) << ")"
                );
    } else if ( _pid == 0 ) {
        // child process
        (void)close(STDIN_FILENO);
  
        // ask kernel to kill child if parent dies
        if ( prctl(PR_SET_PDEATHSIG, SIGKILL) ) {
            perror( "prctl( PR_SET_PDEATHSIG, SIGKILL )" );
            _exit( EXIT_FAILURE );
        }

        // open stdout file
        (void)umask( S_IWGRP | S_IWOTH );
        const mode_t access = S_IRWXU | S_IRWXG | S_IROTH;
        int fd = open( log.str().c_str(), O_CREAT|O_WRONLY|O_TRUNC, access );
        if ( fd == -1 ) {
            perror( "open" );
            _exit( EXIT_FAILURE );
        }

        // setup stdout to write to log
        ::close( STDOUT_FILENO );
        if ( dup2( fd, STDOUT_FILENO ) != STDOUT_FILENO ) {
            perror( "dup2 stdout" );
            _exit( EXIT_FAILURE );
        }
        
        // setup stderr to write to log
        ::close( STDERR_FILENO );
        if ( dup2( fd, STDERR_FILENO) != STDERR_FILENO ) {
            perror( "dup2 stderr" );
            _exit( EXIT_FAILURE );
        }

        // done with log
        ::close( fd );

        // set process group
        if ( setpgrp() < 0 ) {
            perror( "setpgrp" );
            _exit(EXIT_FAILURE);
        }

        // zero signal mask inherited from parent
        sigset_t mask;
        sigemptyset( &mask );
        if ( pthread_sigmask( SIG_SETMASK, &mask, NULL ) == -1 ) {
            _exit( EXIT_FAILURE );
        }

        // exec
        int rc = execl(
                std::string(_path + "/" + _name).c_str(),
                _name.c_str(),
                "--simulation_id", boost::lexical_cast<std::string>(_id).c_str(),
                "--properties", _options.getProperties()->getFilename().c_str(),
                NULL
                );

        // handle errors
        if ( rc == -1 ) {
            std::cerr << "could not exec " << _path << "/" << _name << ": " << strerror(errno) << std::endl;
        }

        // shouldn't get here
        _exit(EXIT_FAILURE);
    } else {
        // parent, fall through
    }
    LOG_INFO_MSG( "forked child with pid " << _pid );
}
 
void
Iosd::start(
        const cios::Connection::Ptr& control,
        const cios::Connection::Ptr& data
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_TRACE_MSG( "starting" );

    // create directory
    const boost::filesystem::path path(
            "/tmp/cios" +
            boost::lexical_cast<std::string>(_id)
            );
    try {
        if ( !boost::filesystem::create_directory(path) ) {
            LOG_RUNJOB_EXCEPTION( error_code::block_invalid, "Could not create directory: " << path );
        }
    } catch ( const boost::filesystem::filesystem_error& e ) {
        LOG_RUNJOB_EXCEPTION( error_code::block_invalid, e.what() );
    }

    const sim::Inotify::Ptr iNotify( _iNotify.lock() );
    if ( !iNotify ) {
        LOG_ERROR_MSG( "lost Inotify" );
        return;
    }

    iNotify->watch( path, "jobcontrol.port", control );
    iNotify->watch( path, "standardio.port", data );

    this->fork();
}

Iosd::~Iosd()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
   
    if ( _pid ) {
        LOG_DEBUG_MSG( "sending SIGKILL to " << _pid );
        ::killpg( _pid, SIGKILL );
    }
    
    boost::filesystem::path path(
            "/tmp/cios" +
            boost::lexical_cast<std::string>(_id)
            );

    bool removeLogFiles = true;
    const std::string key( "iosd_remove_log_files" );
    try {
        const std::string& value = _options.getProperties()->getValue( PropertiesSection, key );
        std::istringstream is( value );
        is >> std::boolalpha >> removeLogFiles;
        if ( !is ) {
            LOG_WARN_MSG( "garbage value in key " << key << ": " << value );
            LOG_WARN_MSG( "using default value " << std::boolalpha << removeLogFiles );
        }
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( "missing key " << key << " in properties file" );
        LOG_WARN_MSG( "using default value " << std::boolalpha << removeLogFiles );
    }
    
    if ( removeLogFiles ) {
        try {
            size_t n = boost::filesystem::remove_all( path );
            LOG_TRACE_MSG( "removed " << n << " files from " << path );
        } catch ( const boost::filesystem::filesystem_error& e ) {
            LOG_WARN_MSG( e.what() );
        }
    } else {
        LOG_TRACE_MSG( "leaving log files in " << path );
    }
}

} // sim
} // server
} // runjob
