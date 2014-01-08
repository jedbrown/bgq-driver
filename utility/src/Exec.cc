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
#include <csignal>
#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pwd.h>
#include <sys/wait.h>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <Log.h>
#include "Exec.h"
#include "ScopeGuard.h"

LOG_DECLARE_FILE("utility");

void dupfds(int logfd) {
    ::close( STDOUT_FILENO );
    if ( dup2( logfd, STDOUT_FILENO ) != STDOUT_FILENO ) {
        std::ostringstream error;
        error << "dup2 failed to set STDOUT to the log file: " << strerror(errno);
        //        throw Exec::ExecException(errno, error.str());
    }

    // setup stderr to write to log
    ::close( STDERR_FILENO );
    if ( dup2( logfd, STDERR_FILENO) != STDERR_FILENO ) {
        std::ostringstream error;
        error << "dup2 failed to set STDERR to the log file: " << strerror(errno);
        //        throw Exec::ExecException(errno, error.str());
    }
}

int logoutput(std::string logfilename, int errorfd) {
    int logfd = open(logfilename.c_str(), O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP);
    if(logfd < 0) {
        std::ostringstream msg;
        msg << "Could not open log file " << logfilename << " error " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        ::write(errorfd, msg.str().c_str(), msg.str().length());
        return 0;
    }
    
    fcntl(logfd, F_SETFD, FD_CLOEXEC);
    int fcntlFlags = fcntl(logfd, F_GETFL);
    if(fcntlFlags < 0) {
        std::ostringstream msg;
        msg << "fcntl to set log file descriptor to close on exec failed with error " 
            << strerror(errno);
        LOG_WARN_MSG(msg.str());
        if(logfd)
            ::close(logfd);
        return 0;
    }
    return logfd;
}

pid_t Exec::fexec(int& pipefd, const std::string& path_and_args,
                  std::string& errorstring,
                  const bool managed, const std::string logfilename, 
                  const std::string propfile, const std::string userid) {

    LOG_TRACE_MSG("path and args=" << path_and_args);
    std::ostringstream errorstream;

    char* arg_array[128];
    // Guarantee the last argument is NULL
    bzero(arg_array, 128);
    for(int i = 0; i < 128; ++i)
        arg_array[i] = NULL;


    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // pull out the full path
    boost::char_separator<char> sep(" ");
    tokenizer nametok(path_and_args, sep);
    tokenizer::iterator b = nametok.begin();
    std::string path = (*b);

    // now just the executable name
    boost::char_separator<char> slash("/");
    tokenizer pathtok(path, slash);
    std::string binname = "";
    for(tokenizer::iterator beg = pathtok.begin(); beg != pathtok.end(); ++beg) {
        binname = (*beg);
    }

    arg_array[0] = (char*)(binname.c_str());

    // Create a temporary vector to stuff the args in.
    // Do this because the tokenizer pointer changes
    // its pointee.
    std::vector<std::string> argvec;
    int i = 0;
    for(tokenizer::iterator beg=nametok.begin(); beg!=nametok.end();++beg){
        if(i == 0)  // first is the path.  We don't want that.
            ;
        else {
            LOG_TRACE_MSG("arg[" << i << "]=" << *beg);
            argvec.push_back(*beg);
        }
        ++i;
    }

    // Now have the array of char* point to the vector
    // contents, not the tokenizer contents.
    for(unsigned int k = 0; k < argvec.size(); ++k) 
        arg_array[k+1] = (char*)(argvec[k].c_str());
    
    // Create a pipe to read output from the child proc
    int pipeFromChild[2];
    if(pipe(pipeFromChild) != 0) {
        // Error on pipe command.
        std::string error("pipe() failed");
    }

    fcntl(pipeFromChild[0], F_SETFD, FD_CLOEXEC);
    fcntl(pipeFromChild[1], F_SETFD, FD_CLOEXEC);
    bool fcntlfail = false;
    int fcntlFlags = fcntl(pipeFromChild[0], F_GETFL);
    if(fcntlFlags < 0) fcntlfail = true;
    fcntlFlags = fcntl(pipeFromChild[1], F_GETFL);
    if(fcntlFlags < 0) fcntlfail = true;
    if(fcntlfail) {
        std::ostringstream msg;
        msg << "fcntl to set log file descriptor to close on exec failed with error " 
            << strerror(errno);
        LOG_WARN_MSG(msg);
    }

    // Create error pipe
    int errorPipe[2];
    if(pipe(errorPipe) != 0) {
        // Error on pipe command.
        std::string error("pipe() failed");
    }

    fcntl(errorPipe[0], F_SETFD, FD_CLOEXEC);
    fcntl(errorPipe[1], F_SETFD, FD_CLOEXEC);
    fcntlFlags = fcntl(errorPipe[0], F_GETFL);
    if(fcntlFlags < 0) fcntlfail = true;
    fcntlFlags = fcntl(errorPipe[1], F_GETFL);
    if(fcntlFlags < 0) fcntlfail = true;
    if(fcntlfail) {
        std::ostringstream msg;
        msg << "fcntl to set error file descriptor to \'close on exec\' failed with error " 
            << strerror(errno);
        LOG_WARN_MSG(msg);
    }


    uid_t my_euid = geteuid();
    gid_t my_egid = getegid();
    bool isroot = false;
    if(userid.length() != 0) {
        int rc = seteuid(0); // Go back to root while we do this.
        if(rc != 0) {
            LOG_WARN_MSG("Cannot change uid to root.  Current uid is " << geteuid() << ".  " << strerror(errno));
            return -2;
        } else isroot = true;
    }
    
    // Get a shared memory segment.  We use this for a flag
    // to indicate when we are about to exec the child.  This way,
    // we can be assured that any error strings are returned.
    const int exec_shmid = shmget(IPC_PRIVATE, sizeof(int), (IPC_CREAT|S_IRWXO|S_IRWXU|S_IRWXG));
    if(exec_shmid < 0) {
        LOG_ERROR_MSG("could not get shared memory segment: " << strerror(errno));
        return -1;;
    }
    LOG_TRACE_MSG( "got shared memory segment with id: " << exec_shmid );

    // remove shared memory segment when it goes out of scope
    bgq::utility::ScopeGuard shmid_remove_guard(
            boost::bind(
                &shmctl, exec_shmid, IPC_RMID, static_cast<struct shmid_ds*>(NULL)
                )
            );

    // Fork the new process.
    pid_t pid = fork();

    if(pid < 0) {
        // Error on fork.
        std::string error("fork() failed");
    }
    else if(pid == 0) {
        // child process
        
        // Don't need the read side.  We only write errors.
        close(errorPipe[0]);

        // Attach the shared memory segment to a local int.
        void* exec_flag_buff = 0;
        exec_flag_buff = shmat(exec_shmid, (char *)0,0);
        if(exec_flag_buff == (void*)-1) {
            errorstream << "Could not attach shared memory segment in child " << strerror(errno);
            ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
        }

        *((char*)(exec_flag_buff)) = 0;

        // Set the bg.properties file if we have one
        if(propfile.length() != 0)
            setenv("BG_PROPERTIES_FILE", propfile.c_str(), true);


        if(userid.length() != 0 && isroot) {
            // We have an assigned user id and we are root.
            struct passwd* my_entry;
            my_entry = getpwnam(userid.c_str());
            if(my_entry == NULL) {
                *((char*)(exec_flag_buff)) = -1;
                errorstream << "Could not get password entry for " << userid 
                            << ". Check that " << userid << " is a valid user on this system. "
                            << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            }

            int rc = setregid(my_entry->pw_gid, my_entry->pw_gid);
            if(rc != 0) {
                *((char*)(exec_flag_buff)) = -1;
                errorstream << "Could not set real and effective gids of this process: " << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            }

            // Now, find all of the groups for the user (up to 25) and set them:
            gid_t groups[25];
            int groupcount = 25;
            rc = getgrouplist(userid.c_str(), my_entry->pw_gid, groups, &groupcount);
            if(rc < 0) {
                LOG_INFO_MSG("User is a member of more than 25 groups.");
            }
            rc = setgroups(groupcount, groups);
            if(rc < 0) {
                *((char*)(exec_flag_buff)) = -1;
                errorstream << "Could not set secondary groups of this process: " << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            }

            std::ostringstream groupmsg;
            for(int i = 0; i < groupcount; ++i) {
                groupmsg << groups[i] << " ";
            }
            LOG_TRACE_MSG("Set the following supplementary groups " << groupmsg.str());
            // Finally, set the user id.
            rc = setreuid(my_entry->pw_uid, my_entry->pw_uid);
            if(rc != 0) {
                *((char*)(exec_flag_buff)) = -1;
                errorstream << "Could not set real and effective uids of this process" << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            }
        } else {
            int rc = setregid(my_egid, my_egid);
            if(rc != 0) {
                *((char*)(exec_flag_buff)) = -1;
                errorstream << "Could not set real and effective gids of this process to " << my_egid << ": " << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            }

            rc = setreuid(my_euid, my_euid);
            if(rc != 0) {
                *((char*)(exec_flag_buff)) = -1;
                errorstream << "Could not set real and effective uids of this process to " << my_euid << ": " << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
                ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            }
        }

        if(managed) {
            if(logfilename.length() == 0) {
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
                if(logfd <= 0) {
                    *((char*)(exec_flag_buff)) = -1;
                } else {
                    dupfds(logfd);
                    // close the pipefds
                    ::close(pipeFromChild[0]);
                    ::close(pipeFromChild[1]);
                }
            }
        } else {
            if(logfilename.length() != 0) {
                int logfd = logoutput(logfilename, errorPipe[1]);
                if(!logfd) {
                    *((char*)(exec_flag_buff)) = -1;
                } else {
                    LOG_DEBUG_MSG("Logging output to file descriptor " << logfd);
                    // setup stdout to write to log
                    dupfds(logfd);
                    ::close(pipeFromChild[0]);
                    ::close(pipeFromChild[1]);
                }
            } // else, in the unmanaged, no logfile case, do nothing.
        }

        // start a new process group
        pid_t my_pgid = setpgid(0,0);
        if (my_pgid < 0) {
            LOG_ERROR_MSG( "setpgrp: " << strerror(errno));
            _exit(-1);
        }
    
        int default_signals[] = { SIGCHLD, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE, SIGABRT }; 
        unsigned num_default_signals = sizeof(default_signals) / sizeof(default_signals[0]);
        int ignore_signals[] = { SIGALRM, SIGHUP }; 
        unsigned num_ignore_signals = sizeof(ignore_signals) / sizeof(ignore_signals[0]);

        // restore signal handlers to defaults
        for (unsigned int j = 0; j < num_default_signals; ++j)
            signal(default_signals[j], SIG_DFL);

        // ignore SIGALRM, SIGHUP
        for (unsigned int j = 0; j < num_ignore_signals; ++j)
            signal(ignore_signals[j], SIG_IGN);
        int exec_flag = (unsigned char)*((unsigned char*)(exec_flag_buff));
        if(managed) {
            // If the parent dies, kill the child
            int ret = prctl(PR_SET_PDEATHSIG, SIGKILL);
            if(ret < 0) {
                LOG_WARN_MSG("Could not set process death signal " << strerror(errno));
            }
        }
        if(exec_flag == 0) {
            *((char*)(exec_flag_buff)) = 1;
            execv((char*)path.c_str(), arg_array);
        }
        _exit(errno);
    } else {
        // Parent process.
        // Don't need the write side.  We only write errors.
        ::close(errorPipe[1]);

        // Set uid back to non-privileged user.
        int rc = seteuid(my_euid);
        if(rc != 0) {
            errorstream << "Cannot reset effective uid to " << my_euid << ". " << strerror(errno);
            ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
            LOG_ERROR_MSG(errorstream.str());
        } else {
            LOG_DEBUG_MSG("successfully reset euid to " << my_euid);
        }

        void* exec_flag_buff = 0;
        exec_flag_buff = shmat(exec_shmid, (char *)0,0);
        if(exec_flag_buff == (void*)-1) {
            errorstream << "Failed to attached shared memory segment in parent." << strerror(errno);
            ::write(errorPipe[1], errorstream.str().c_str(), errorstream.str().length());
        }

        int exec_flag = 0;
        while(exec_flag == 0) {
            exec_flag = (unsigned char)*((unsigned char*)(exec_flag_buff));
            usleep(1000);
        }

        if(exec_flag != 1) {
            // Read the error pipe for error string.
            char buff[1024]; // error message length max is 1024 for convenience
            bzero(buff, 1024);
            int bytes_read = ::read(errorPipe[0], &buff, sizeof(buff));
            LOG_TRACE_MSG(bytes_read << " of error data found.");
            std::string es(buff);
            errorstring = es;
            int zero = 0;
            LOG_DEBUG_MSG("waiting for " << pid);
            if(waitpid(pid, &zero, zero) < 0) {
                LOG_ERROR_MSG("waitpid failed " << strerror(errno));
            }
            pid = -1;
        }

    }

    if(logfilename.length() == 0) {
        if(pipeFromChild[0]) {
            pipefd = pipeFromChild[0];
            ::close(pipeFromChild[1]);
        }
    } else {
        // Close the pipes.  We don't need them if output is going to a log file.
        pipefd = 0;
        ::close(pipeFromChild[0]);
        ::close(pipeFromChild[1]);
    }
    ::close(errorPipe[0]);
    //    errorstring = errorstream.str();
    return pid;
}

void Exec::ChildMonitor::monitor_child_once(int& child_fd, std::ostream* outstream) {
    _stopping = true;
    monitor_child(child_fd, outstream);
}

void Exec::ChildMonitor::monitor_child(int& child_fd, std::ostream* outstream) {

    _my_tid = pthread_self();
    LOG_INFO_MSG("monitoring thread " << _my_tid);

    int BUFFSIZE = 1024;
    char buff[BUFFSIZE];

    int bytes_read = -1;

    // Now read from it
    do {
        bzero(buff, sizeof(BUFFSIZE));
        bytes_read = read(child_fd, buff, BUFFSIZE);
        LOG_TRACE_MSG("Read " << bytes_read << " bytes from child");
        if(bytes_read < 0) {
            if(errno == EINTR || errno == EAGAIN) {
                LOG_TRACE_MSG("Read error " << errno);

                if(!_stopping)
                    continue;
                else 
                    break;
            } else {
                LOG_ERROR_MSG("Read error " << strerror(errno));
                _stopping = true;
                break;
            }
        } else if (bytes_read == 0) { // EOF
            _stopping = true;
            break;
        }

        outstream->write(buff, bytes_read);
        outstream->flush();
    } while(!_stopping);
    outstream->flush();
}

void Exec::ChildMonitor::end() {
    LOG_INFO_MSG("Killing thread " << _my_tid);
    _stopping = true;
    pthread_kill(_my_tid, SIGUSR1);
}
