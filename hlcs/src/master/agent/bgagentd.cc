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

#include "Agent.h"

#include "common/AgentProtocol.h"
#include "common/ArgParse.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <csignal>
#include <boost/filesystem.hpp>
#include <utility/include/Log.h>
#include <utility/include/version.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/cxxsockets/SocketTypes.h>

LOG_DECLARE_FILE( "master" );

static Agent agent;

// Need a signal handler to bring down all threads
// and all binaries gently.

static int signals[] = {
    SIGHUP,
    SIGINT,
    SIGQUIT,
    SIGILL,
    SIGABRT,
    SIGFPE,
    SIGSEGV,
    SIGPIPE,
    SIGALRM,
    SIGTERM,
    SIGUSR1,
    SIGUSR2,
    SIGXFSZ,
    SIGBUS,
    SIGPOLL,
    SIGPROF,
    SIGSYS,
    SIGVTALRM,
    SIGXCPU
};

static int num_signals = sizeof(signals) / sizeof(signals[0]);
volatile int signal_number = 0;          // flag to terminate bgagentd
                                         // set to termination signal by signal handler
Args* pargs;

void
bgagentd_sighandler(
        int signum,
        siginfo_t* siginfo,
        void*)
{
    if (signum == SIGUSR1 || signum == SIGPIPE || signum == SIGHUP) {
        return;
    } else {
        std::cerr << "bgagentd ending due to signal " << signum <<  " in thread "
                  << pthread_self() << " from " << siginfo->si_pid << "." << std::endl;
        signal_number = signum;
        agent.cleanup();
        // Reset to default action for our signal
        struct sigaction sigact;
        sigact.sa_handler = SIG_DFL;
        int rc = sigaction(signum, &sigact, 0);
        if (rc < 0) {
            std::cerr << "Error resetting default action for bgagentd signal handler." << std::endl;
            exit(1);
        }
        // now raise it again
        raise(signum);
    }
}

std::string
setlogging(
        std::string& logdir
        )
{
    if (logdir.empty()) {
        // Use default
        logdir = "/var/log";
    }

    std::string logfile = "";
    logfile = logdir + "/" + agent.get_hostname().uhn() + "-bgagentd.log";

    // Now open it. User and group readable. User writable.
    int openfd = open(logfile.c_str(), O_WRONLY|O_APPEND|O_CREAT,
                      S_IRUSR|S_IWUSR|S_IRGRP);
    if (openfd == -1) {
        std::string error_str = "Error opening log file " + logfile;
        perror(error_str.c_str());
        exit(0);
    }

    // One last notification before dumping output to file
    std::cout << "bgagentd [" << getpid() << "] starting and logging to " << logfile << std::endl;

    // And send stdout and stderr to it.
    dup2(openfd, STDOUT_FILENO);
    dup2(openfd, STDERR_FILENO);

    // Not using it any more.
    close(openfd);

    return logfile;
}

void
help()
{
    std::cerr << "bgagentd is the BGmaster system's process monitor." << std::endl;
}

void
usage()
{
    std::cerr << "bgagentd [ --properties < filename > ] [ --help ] [ --logdir < directory > ] [ --debug < true | false > ] [ --workingdir < directory > ]  [ --host host:port ] --users < users >" << std::endl;
}

int main(int argc, const char** argv) {
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    std::string debugarg = "--debug";
    std::string logdirarg = "--logdir";
    std::string userarg = "--users";
    std::string workingdirarg = "--workingdir";
    validargs.push_back(debugarg);
    validargs.push_back(logdirarg);
    validargs.push_back(userarg);
    validargs.push_back(workingdirarg);
    validargs.push_back("*"); // One argument without a "--" is allowed
    Args largs(argc, argv, &usage, &help, validargs, singles, 32041, true);
    pargs = &largs;

    std::string debugstr = (*pargs)[debugarg];
    bool debug = false;
    if (debugstr == "true")
        debug = true;
    else if (debugstr == "false")
        debug = false;
    else if (!debugstr.empty()) {
        std::cerr << "Please indicate 'true' or 'false' after --debug" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Signal handlers
    for (int i = 0; i < num_signals; i++)
    {
        struct sigaction action;
        action.sa_sigaction = &bgagentd_sighandler;
        action.sa_flags = SA_SIGINFO;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0)
        {
            std::cerr << "Error setting up bgagentd signal handler: " << strerror(errno) << std::endl;
            exit(1);
        }
    }

    std::string logdir = (*pargs)[logdirarg];
    if (!debug) {
        // Now find the logdir
        if (logdir.length() == 0) {
            try {
                logdir = pargs->get_props()->getValue("master.agent", "logdir");
            } catch(std::invalid_argument& e) {
                LOG_ERROR_MSG("No logging directory specified or missing section. " << e.what());
                exit(1);
            }
        }

        // Create log file and symlink
        setlogging(logdir);
        // daemonize
        if (daemon(0, 1) < 0) {
            std::cerr << "Error trying to daemonize bgagentd: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }

    if (debug) {
        LOG_INFO_MSG( argv[0] << " [" << getpid() << "] starting in debug mode." );
    }

    const std::string workingdir = (*pargs)[workingdirarg];
    if (!workingdir.empty() ) {
        LOG_DEBUG_MSG("Changing working directory to: " << workingdir);
        const int rc = chdir(workingdir.c_str());
        if (rc) {
            LOG_FATAL_MSG("Could not change working directory to '" << workingdir << "', error is: " << strerror(errno));
            exit(EXIT_FAILURE);
        }
    }


    bgq::utility::Properties::Ptr props = pargs->get_props();
    agent.set_users((*pargs)[userarg]);
    std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
    std::ostringstream version;
    version << "Blue Gene/Q " << basename << " " << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;

    struct rlimit rlimit_nofile = {0, 0}; // process limit on number of files
    struct rlimit rlimit_core = {0, 0};    // process limit on core file size
    rlimit_core.rlim_cur = RLIM_INFINITY;
    rlimit_core.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_CORE, &rlimit_core) < 0) {
        LOG_WARN_MSG("Could not set core dump limit to unlimited. May not be able to capture any core dumps for debug.");
    }
    getrlimit(RLIMIT_NOFILE, &rlimit_nofile); // get the current process file descriptor limit
    if (rlimit_nofile.rlim_cur < 819200) {
        rlimit_nofile.rlim_max = 819200;
        rlimit_nofile.rlim_cur = 819200;

        if (setrlimit(RLIMIT_NOFILE, &rlimit_nofile) < 0) {
            LOG_WARN_MSG("Could not increase file descriptor rlimits, some programs may be resource constrained. Current rlimits setting is "
                         << rlimit_nofile.rlim_cur);
        }
    }

    getrlimit(RLIMIT_CORE, &rlimit_core); // get the current process core file limit
    LOG_DEBUG_MSG("Core limits: " << rlimit_core.rlim_cur);
    LOG_DEBUG_MSG("File limits: " << rlimit_nofile.rlim_cur);

    LOG_INFO_MSG("bgagentd [" << getpid() << "] " << version.str() << " starting");
    agent.set_pairs(pargs->get_portpairs());
    agent.startup(props);
}
