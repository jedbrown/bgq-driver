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

#include "Exec.h"

#include "Log.h"
#include "Properties.h"
#include "UserId.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <iostream>

#include <csignal>
#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>

LOG_DECLARE_FILE("utility");

void
dupfds(
        const int logfd,
        const int errorfd
        )
{
    ::close( STDOUT_FILENO );
    if ( dup2(logfd, STDOUT_FILENO) != STDOUT_FILENO ) {
        std::ostringstream msg;
        char buf[256];
        msg << "dup2 failed to set STDOUT to the log file: " << strerror_r(errno, buf, sizeof(buf));
        LOG_ERROR_MSG(msg.str());
        ::write(errorfd, msg.str().c_str(), msg.str().length());
    }

    // setup stderr to write to log
    ::close( STDERR_FILENO );
    if ( dup2(logfd, STDERR_FILENO) != STDERR_FILENO ) {
        std::ostringstream msg;
        char buf[256];
        msg << "dup2 failed to set STDERR to the log file: " << strerror_r(errno, buf, sizeof(buf));
        LOG_ERROR_MSG(msg.str());
        ::write(errorfd, msg.str().c_str(), msg.str().length());
    }
}

int
logoutput(
        const std::string& logfilename,
        const int errorfd
        )
{
    #ifdef O_CLOEXEC
    const int logfd = open(logfilename.c_str(), O_RDWR|O_CREAT|O_APPEND|O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP);
    #else
    const int logfd = open(logfilename.c_str(), O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP);
    #endif
    if (logfd < 0) {
        std::ostringstream msg;
        char buf[256];
        msg << "Could not open log file " << logfilename << " error " << strerror_r(errno, buf, sizeof(buf));
        LOG_ERROR_MSG(msg.str());
        ::write(errorfd, msg.str().c_str(), msg.str().length());
        return 0;
    }
    
    return logfd;
}

pid_t
Exec::fexec(
        int& pipefd,
        const std::string& path_and_args,
        std::string& errorstring,
        const bool managed,
        const std::string& logfilename, 
        const std::string& propfile,
        const std::string& userid
        )
{
    LOG_TRACE_MSG("path and args=" << path_and_args);

    // arguments for exec
    char* arg_array[128] = { NULL };
    std::vector<std::string> arg_vector;

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // pull out the full path
    const tokenizer nametok(path_and_args, boost::char_separator<char>(" "));
    const std::string path = *nametok.begin();

    // now just the executable name
    const tokenizer pathtok(path, boost::char_separator<char>("/"));
    for (tokenizer::const_iterator i = pathtok.begin(); i != pathtok.end(); ++i) {
        arg_vector.clear();
        arg_vector.push_back( *i );
        arg_array[0] = const_cast<char*>(arg_vector[0].c_str());
    }

    for (tokenizer::const_iterator i = nametok.begin(); i != nametok.end(); ++i) {
        const size_t position = static_cast<size_t>(std::distance( nametok.begin(), i ));
        if ( position == 0 ) {
            // already got this guy
        } else if ( position >= sizeof(arg_array) - 1 ) {
            LOG_ERROR_MSG( "more than " << sizeof(arg_array) - 1 << " arguments is not supported" );
            return -1;
        } else {
            arg_vector.push_back( *i );
            arg_array[position] = const_cast<char*>(arg_vector[position].c_str());
        }
    }

    for ( std::vector<std::string>::iterator i = arg_vector.begin(); i != arg_vector.end(); ++i ) {
        const size_t position = static_cast<size_t>(std::distance( arg_vector.begin(), i ));
        LOG_TRACE_MSG("arg[" << position << "]=" << *i);
    }

    // Create a pipe to read output from the child proc
    int pipeFromChild[2];
    #ifdef O_CLOEXEC
    if (pipe2(pipeFromChild, O_CLOEXEC) != 0) {
    #else
    if (pipe(pipeFromChild) != 0) {
    #endif
        char buf[256];
        LOG_ERROR_MSG( "creating child pipe() failed: " << strerror_r(errno, buf, sizeof(buf)) );
        return -1;
    }

    // Create error pipe
    int errorPipe[2];
    #ifdef O_CLOEXEC
    if (pipe2(errorPipe, O_CLOEXEC) != 0) {
    #else
    if (pipe(errorPipe) != 0) {
    #endif
        char buf[256];
        LOG_ERROR_MSG( "creating error pipe() failed: " << strerror_r(errno, buf, sizeof(buf)) );
        return -1;
    }

    const uid_t my_euid = geteuid();
    const gid_t my_egid = getegid();
    bool isroot = false;
    if (!userid.empty()) {
        if ( seteuid(0) != 0 ) {
            char buf[256];
            LOG_WARN_MSG("Cannot change uid to root.  Current uid is " << geteuid() << ".  " << strerror_r(errno, buf, sizeof(buf)));
            return -1;
        }
        isroot = true;
    }
    
    // Fork the new process.
    pid_t pid = fork();

    if (pid < 0) {
        // Error on fork.
        char buf[256];
        LOG_ERROR_MSG("fork() failed: " << strerror_r(errno, buf, sizeof(buf)) );
        return -1;
    }

    if (pid == 0) {
        // child process
        
        // Set the bg.properties file if we have one
        if (!propfile.empty())
            setenv(bgq::utility::Properties::EnvironmentalName.c_str(), propfile.c_str(), true);

        if (!userid.empty() && isroot) {
            // We have an assigned user id and we are root.
            try {
                const bgq::utility::UserId uid( userid );
                const gid_t my_gid = uid.getGroups().front().first;
                if (setregid(my_gid, my_gid) != 0) {
                    char errorText[256];
                    std::ostringstream msg;
                    msg << "Cannot change real effective gid to " << my_gid << ": " << strerror_r(errno, errorText, 256);
                    throw std::runtime_error(msg.str());
                }
                // Extract gid_t from each group returned in the UserId object
                std::vector<gid_t> groups;
                BOOST_FOREACH( const bgq::utility::UserId::Group& i, uid.getGroups() ) {
                    groups.push_back( i.first );
                }
                // Assuming the storage of a std::vector is contiguous memory
                if (setgroups(groups.size(), &groups[0]) < 0) {
                    char errorText[256];
                    std::ostringstream msg;
                    msg << "Could not set secondary groups: " << strerror_r(errno, errorText, 256);
                    throw std::runtime_error(msg.str());
                }

                if (setreuid(uid.getUid(), uid.getUid()) != 0) { // Make the change
                    char errorText[256];
                    std::ostringstream msg;
                    msg << "Cannot change real effective uid to " << uid.getUid() << ": " << strerror_r(errno, errorText, 256);
                    throw std::runtime_error(msg.str());
                }
            } catch ( const std::exception& e ) {
                std::ostringstream msg;
                msg << e.what();
                ::write(errorPipe[1], msg.str().c_str(), msg.str().length());
                _exit(EXIT_FAILURE);
            }
        } else {
            int rc = setregid(my_egid, my_egid);
            if (rc != 0) {
                std::ostringstream errorstream;
                char buf[256];
                errorstream << "Could not set real and effective gids of this process to " << my_egid << ": " << strerror_r(errno, buf, sizeof(buf));
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
                _exit(EXIT_FAILURE);
            }

            rc = setreuid(my_euid, my_euid);
            if (rc != 0) {
                std::ostringstream errorstream;
                char buf[256];
                errorstream << "Could not set real and effective uids of this process to " << my_euid << ": " << strerror_r(errno, buf, sizeof(buf));
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
                _exit(EXIT_FAILURE);
            }
        }

        if (managed) {
            if (logfilename.empty()) {
                // If no log file descriptor has been specified, set up a pipe
                // for the parent to read
                LOG_DEBUG_MSG("No destination for stdout/stderr specified.  Set up pipe.");

                // Don't need the read side.
                close(pipeFromChild[0]);
            
                // send stdout and stderr to the pipe
                dup2(pipeFromChild[1], STDOUT_FILENO);
                dup2(pipeFromChild[1], STDERR_FILENO);
            
                // Since we've rerouted stdout, close the pipe fd.
                ::close(pipeFromChild[1]);
            } else {
                int logfd = logoutput(logfilename, errorPipe[1]);
                if (!logfd) {
                    // already wrote into error pipe
                    _exit(EXIT_FAILURE);
                } else {
                    dupfds(logfd, errorPipe[1]);
                }
            }
        } else {
            if (!logfilename.empty()) {
                const int logfd = logoutput(logfilename, errorPipe[1]);
                if (!logfd) {
                    // already wrote into error pipe
                    _exit(EXIT_FAILURE);
                }

                // setup stdout to write to log
                LOG_DEBUG_MSG("Logging output to file descriptor " << logfd);
                dupfds(logfd, errorPipe[1]);
            } // else, in the unmanaged, no logfile case, do nothing.
        }

        // start a new process group
        if (setpgid(0,0) < 0) {
            std::ostringstream msg;
            char buf[256];
            msg << "Could not set new process grou: " << strerror_r(errno, buf, sizeof(buf));
            ::write(errorPipe[1], msg.str().c_str(), msg.str().length());
            _exit(EXIT_FAILURE);
        }
    
        // If the parent dies, kill the child
        if (managed && prctl(PR_SET_PDEATHSIG, SIGKILL) != 0) {
            std::ostringstream msg;
            char buf[256];
            msg << "Could not set process death signal: " << strerror_r(errno, buf, sizeof(buf));
            ::write(errorPipe[1], msg.str().c_str(), msg.str().length());
            _exit(EXIT_FAILURE);
        }

        // zero signal mask inherited from parent process
        sigset_t mask;
        sigemptyset( &mask );
        if ( pthread_sigmask( SIG_SETMASK, &mask, NULL ) == -1 ) {
            std::ostringstream msg;
            char buf[256];
            msg << "Could not set signal mask: " << strerror_r(errno, buf, sizeof(buf));
            ::write(errorPipe[1], msg.str().c_str(), msg.str().length());
            _exit(EXIT_FAILURE);
        }

        execv(path.c_str(), arg_array);
        const int error = errno;
        std::ostringstream msg;
        char buf[256];
        msg << "execv(" << path << ") failed: " << strerror_r(error, buf, sizeof(buf));
        ::write(errorPipe[1], msg.str().c_str(), msg.str().length());
        _exit(EXIT_FAILURE);
    }

    // Parent process.
    // Don't need the write side.
    ::close(errorPipe[1]);

    // Set uid back to non-privileged user.
    if ( seteuid(my_euid) != 0 ) {
        char buf[256];
        LOG_ERROR_MSG("Cannot reset effective uid to " << my_euid << ": " << strerror_r(errno, buf, sizeof(buf)));
    } else {
        LOG_DEBUG_MSG("successfully reset euid to " << my_euid);
    }

    char buf[1024];
    bzero(buf, 1024);
    ssize_t rc = 0;
    while ( 1 ) {
        rc = ::read( errorPipe[0], buf, sizeof(buf) );
        if ( rc == -1 && errno == EINTR ) continue;
        break;
    }
    ::close(errorPipe[0]);

    LOG_TRACE_MSG(rc << " bytes of error data found.");
    if ( rc > 0 ) {
        errorstring = buf;
        LOG_DEBUG_MSG( errorstring );
        int zero = 0;
        LOG_DEBUG_MSG("waiting for " << pid);
        if (waitpid(pid, &zero, zero) < 0) {
            char buf[256];
            LOG_ERROR_MSG("waitpid failed " << strerror_r(errno, buf, sizeof(buf)));
        }
        pid = -1;
    } else {
        // pipe was closed means success
    }

    if (logfilename.empty()) {
        pipefd = pipeFromChild[0];
        ::close(pipeFromChild[1]);
    } else {
        // Close the pipes.  We don't need them if output is going to a log file.
        pipefd = 0;
        ::close(pipeFromChild[0]);
        ::close(pipeFromChild[1]);
    }
    return pid;
}
