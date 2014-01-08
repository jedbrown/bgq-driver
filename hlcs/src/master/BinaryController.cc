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

#include <iostream>
#include <fstream>
#include <sys/prctl.h>
#include <sys/types.h>
#include <csignal>
#include <cerrno>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/scope_exit.hpp>
#include <utility/include/Log.h>
#include <utility/include/Exec.h>
#include <pwd.h>
#include "BinaryController.h"
#include "Thread.h"
#include "BGMasterExceptions.h"
#include "BGAgent.h"

LOG_DECLARE_FILE("master");

#define LOGGING_DECLARE_ID_MDC(value) \
    log4cxx::MDC _location_mdc( "ID", std::string("{") + boost::lexical_cast<std::string>(value) + "} " );

extern volatile int signal_number;

void switchBackUID(uid_t my_euid, gid_t my_egid) {
    LOG_INFO_MSG(__FUNCTION__);
    std::ostringstream errorstream;
    // Done.  Go back to our user, but do it through root.
    int rc = seteuid(0); // Go back to root while we do this.
    if(rc != 0) {
        LOG_WARN_MSG("Cannot change uid back to root.  Current uid is " << geteuid() << ". " << strerror(errno));
    }

    rc = setegid(0);
    if(rc != 0) {
        errorstream << "Could not set effective gid back to root: " << strerror(errno);
        LOG_ERROR_MSG(errorstream.str());
    }

    rc = setegid(my_egid);
    if(rc != 0) {
        errorstream << "Could not set effective gid back to " << my_egid << ": " << strerror(errno);
        LOG_ERROR_MSG(errorstream.str());
    }

    rc = seteuid(my_euid);
    if(rc != 0) {
        errorstream << "Cannot reset effective uid to " << my_euid << ".  Current euid is " << geteuid() << ".  " << strerror(errno);
        LOG_ERROR_MSG(errorstream.str());
    }
    LOG_INFO_MSG("Current UID: " << geteuid() << " Current GID: " << getegid());
}

BinaryId BinaryController::startBinary() {
    LOGGING_DECLARE_ID_MDC(_binid.str());
    LOG_INFO_MSG(__FUNCTION__);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    int pipefd = 0;
    // pull out the full path and see if it exists
    boost::char_separator<char> sep(" ");
    tokenizer nametok(_binary_bin_path, sep);
    tokenizer::iterator b = nametok.begin();
    std::string path = (*b);

    if(_user.length() == 0) {
        LOG_WARN_MSG("No user id defined for " << _alias_name);
    } else {
        if(BGAgent::_user_list.find(_user) == std::string::npos) {
            std::ostringstream err;
            err << "Invalid user id " << _user << " specified";
            LOG_ERROR_MSG(err.str());
            _exec_error = err.str();
            BinaryId badid(-1, "");
            return badid;
        }
    }

    pid_t pid = 0;
    if(boost::filesystem::exists(path)) {
        std::string error;
        Exec exec;
        pid = exec.fexec(pipefd, _binary_bin_path, error, true, _logfile.c_str(), "", _user);
        LOG_DEBUG_MSG("Returned from fexec for " << _alias_name << " pid=" << pid);
        if(pid < 0) {
            LOG_DEBUG_MSG("Error from exec: " << error);
            _exec_error = error;
            BinaryId badid(pid,"");
            return badid;
        }
    } else {
        std::ostringstream msg;
        msg << "Command or path " << path << " does not exist.";
        LOG_ERROR_MSG(msg.str());
        throw BGMasterExceptions::FileError(BGMasterExceptions::WARN, msg.str());
    }

    // If we get this far, we're successful.
    BinaryId child(pid, _host.ip());
    _binid = child;
    _start_time = boost::posix_time::second_clock::local_time();
    LOG_DEBUG_MSG("Executed binary=" << _binid.str() << " at "
                  << boost::posix_time::to_simple_string(_start_time));
    return _binid;
}

bool isRunning(pid_t pid)
{
    LOG_DEBUG_MSG(__FUNCTION__);
    bool retval = false;
    if (pid != 0)
    {
	int rc = kill(pid, 0);		// send a 0 signal just to test the pid
	if (rc == 0)			// normal return implies process is running
	    retval = true;
	else
	    if (errno != ESRCH) {		// ESRCH is normal for a terminated process
                LOG_INFO_MSG("isRunning failed, assuming process still running. " << strerror(errno));
                retval = true;
            }
    }
    return retval;
}

int BinaryController::stopBinary(int signal) {
    LOG_INFO_MSG(__FUNCTION__);
    // This is how this works:
    // signal_number is a process global variable that may be
    // set when we catch a signal and want to propagate it
    // to the child processes.  So, we check that first and
    // if it is set, we send it to the child processes.  If it
    // ISN'T set, then we use the signal passed to us as the
    // initial signal... unless it is unspecified (zero).
    // In that case, we default to SIGTERM.  After we
    // try our initial signal, we wait a bit and start sending
    // SIGKILLs to REALLY kill it.
    LOGGING_DECLARE_ID_MDC(_binid.str());
    LOG_DEBUG_MSG("Stopping binary " << _binid.str());

    stopping(true);
    int pid = _binid.get_pid();
    uid_t my_euid = geteuid();
    gid_t my_egid = getegid();
    std::ostringstream errorstream;

    bool isroot = false;

    if(pid != 0) {
        // Switch back to root so we can switch to assigned user.
        if(_user.length() != 0) {
            int rc = seteuid(0); // Go back to root while we do this.
            if(rc != 0) {
                LOG_WARN_MSG("Cannot change uid to root.  Current uid is " << geteuid() << ". " << strerror(errno));
            } else {
                isroot = true;
                LOG_INFO_MSG("Changed effective uid to root.");
            }
        }

        if(_user.length() != 0 && isroot) {
            struct passwd* my_entry;
            my_entry = getpwnam(_user.c_str());
            if(my_entry == NULL) {
                errorstream << "Could not get password entry for " << _user
                            << ". Check that " << _user << " is a valid user on this system. "
                            << strerror(errno);
                LOG_ERROR_MSG(errorstream.str());
            } else {

                int rc = setegid(my_entry->pw_uid);
                if(rc != 0) {
                    errorstream << "Could not set real and effective gids of this process: " << strerror(errno);
                    LOG_ERROR_MSG(errorstream.str());
                }

                rc = seteuid(my_entry->pw_uid);
                if(rc != 0) {
                    errorstream << "Could not set real and effective uids of this process" << strerror(errno);
                    LOG_ERROR_MSG(errorstream.str());
                }
            }
        }

        BOOST_SCOPE_EXIT( (&isroot) (&my_euid) (&my_egid) ) {
            switchBackUID(my_euid, my_egid);
        } BOOST_SCOPE_EXIT_END;

        LOG_INFO_MSG("Running as " << geteuid() << " to kill " << pid << " with user " << _user);

        boost::posix_time::ptime kill_sent_time = boost::posix_time::second_clock::local_time();
        unsigned sleeptime = 1;
        while(isRunning(pid)) {
            if(signal_number == 0) {
                if(signal == 0) {
                    signal = SIGTERM;
                    LOG_INFO_MSG("Killing " << pid << " with a SIGTERM");
                    int killrc = ::kill(pid, SIGTERM);
                    if(killrc < 0) {
                        LOG_WARN_MSG("Failed to kill " << pid << " with SIGTERM: " << strerror(errno));
                    }
                } else {
                    LOG_INFO_MSG("Killing " << pid << " with signal " << signal);
                    int killrc = ::kill(pid, signal);
                    if(killrc < 0) {
                        LOG_WARN_MSG("Failed to kill " << pid << " with " << signal << ": " << strerror(errno));
                    }
                }
            } else {
                LOG_INFO_MSG("Killing " << pid << " with a signal number " << signal_number);
                signal = signal_number;
                int killrc = ::kill(pid, signal_number);
                if(killrc < 0) {
                    LOG_WARN_MSG("Failed to kill " << pid << " with " << signal << ": " << strerror(errno));
                }
            }

            boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
            boost::posix_time::time_duration td = now - kill_sent_time;
            if(td.total_seconds() > 300) { // Five minute timeout.
                LOG_INFO_MSG("Initial signal failed.  Killing " << pid
                             << " with a SIGKILL.");
                signal = SIGKILL;
                int killrc = ::kill(pid, SIGKILL);
                if(killrc < 0) {
                    LOG_ERROR_MSG("Failed to kill " << pid << " with SIGKILL: " << strerror(errno));
                }
                break;
            }
            sleep(sleeptime);
            sleeptime += 1;
        }
    }
    return signal;
}
