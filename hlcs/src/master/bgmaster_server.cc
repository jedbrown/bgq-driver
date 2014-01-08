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
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include "SocketTypes.h"
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/version.h>
#include "Properties.h"
#include "BGMaster.h"
#include "MasterRasMetadata.h"
#include "BGMasterExceptions.h"
#include "LockFile.h"

// Need a signal handler to bring down all threads gently.

// Parse command line

LOG_DECLARE_FILE( "master" );

static int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE, SIGXFSZ, SIGABRT,
                         SIGSEGV, SIGILL, SIGFPE, SIGTERM, SIGBUS };
static int num_signals = sizeof(signals) / sizeof(signals[0]);
volatile int signal_number = 0;          // flag to terminate bgmaster -- set to termination signal by signal handler

static LockFile* lock_file = 0;

void bgmaster_server_sighandler(int signum, siginfo_t* siginfo, void*) {
    if (signum == SIGUSR1 || signum == SIGPIPE || signum == SIGHUP) {
        std::cerr << "bgmaster_server thread " << pthread_self() << " received signal " 
                  << signum << " from " << siginfo->si_pid << std::endl;
        return;
    } else {
        std::cerr << "bgmaster_server halting due to signal " << signum << " in thread "
                  << pthread_self() << " from " << siginfo->si_pid << "." << std::endl;
        signal_number = signum;

        // Send RAS
        std::map<std::string, std::string> details;
        details["PID"] = boost::lexical_cast<std::string>(getpid());
        details["SIGNAL"] = boost::lexical_cast<std::string>(signum);
        BGMasterController::putRAS(MASTER_FAIL_RAS, details);
        if(lock_file)
            delete lock_file;

        // Reset to default action for our signal
        struct sigaction sigact;
        sigact.sa_handler = SIG_DFL;
        int rc = sigaction(signum, &sigact, 0);
        if (rc < 0)
            {
                std::cerr << "bgmaster_server signal: " << strerror(errno) << std::endl;
                exit(1);
            }
        // now raise it again
        raise(signum);
    }
}


bool setlogging(std::string& logdir) {
    if(logdir.empty()) {
        // Use default
        logdir = "/var/log";
    }

    char hostname[HOST_NAME_MAX];
    if(gethostname(hostname, sizeof(hostname)) < 0) {
        std::cout << "Host name error: " << strerror(errno);
    }

    Host host(hostname);

    std::string logfile = logdir + "/" + host.uhn() + "-bgmaster_server.log";
    std::cerr << "log file is " << logfile << std::endl;
    // Now open it.  User and group readable.  User writable.
    int openfd = open(logfile.c_str(), O_WRONLY|O_APPEND|O_CREAT,
                      S_IRUSR|S_IWUSR|S_IRGRP);
    if(openfd == -1) {
        std::string error_str = "open error " + logfile;
        perror(error_str.c_str());
        return false;
    }

    // And send stdout and stderr to it.
    dup2(openfd, STDOUT_FILENO);
    dup2(openfd, STDERR_FILENO);

    // Not using it any more.
    close(openfd);

    return true;
}

int main(int argc, const char** argv) {
    std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
    std::ostringstream version;
    version << "BG/Q " << basename << " " << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    bgq::utility::Properties::Ptr props;
    bool debug = false;

    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--properties") || !strcasecmp(argv[i], "-p")) {
            if (argc == ++i) {
                std::cerr << "please give a file name after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }

            try {
                props = bgq::utility::Properties::create(argv[i]);
            } catch(std::runtime_error& e) {
                std::cerr << "Properties file configuration error. " << e.what() << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        if (!strcasecmp(argv[i], "--debug")) {
            debug = true;
        }
    }

    if (!props) {
        try {
            props = bgq::utility::Properties::create();
        } catch(std::runtime_error& e) {
            std::cerr << "Properties file configuration error. " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Initialize properties for everybody
    CxxSockets::setProperties(props);
    const std::string default_logger( "ibm.master" );
    bgq::utility::LoggingProgramOptions logging_program_options( default_logger );

    bgq::utility::initializeLogging(*props, logging_program_options, "master");
    std::string logdir = "";
    try {
        logdir = props->getValue("master.server", "logdir");
    } catch(std::invalid_argument& e) {
        std::cerr << "No log directory found.  Will use default.  " << e.what() << std::endl;
    }

    std::cout << "bgmaster_server [" << getpid() + 1 << "] " << version.str() << " starting..." << std::endl;

    std::string master_instances = "1";
    try {
        master_instances = props->getValue("master.policy.instances", "bgmaster_server");
    } catch(std::invalid_argument& e) {
        // Don't care if it isn't there.
    }

    BOOST_SCOPE_EXIT( ( &lock_file ) ) {
        if(lock_file) {
            delete lock_file;
            lock_file = 0;
        }
    } BOOST_SCOPE_EXIT_END;

    if(master_instances == "1") {
        lock_file = new LockFile("bgmaster_server");
        if(lock_file->_fileExists) {
            std::cerr << "Lock file for bgmaster_server found.  End bgmaster_server process "
                      << lock_file->_pid
                      << " and remove " << lock_file->fname << std::endl;
            exit(EXIT_FAILURE);
        }
    }


    if(!debug) {
        if(!setlogging(logdir)) {
            if(lock_file) {
                delete lock_file;
                lock_file = 0;
            }
            exit(0);
        }

        // run as background process
        if (daemon(1, 1) < 0) {
            std::cerr << "daemon: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }

    BOOST_SCOPE_EXIT( ( &lock_file ) ) {
        if(lock_file) {
            delete lock_file;
            lock_file = 0;
        }
    } BOOST_SCOPE_EXIT_END;


    if(lock_file)
        lock_file->setpid(getpid());

    // Signal handlers
    for (int i = 0; i < num_signals; i++)
    {
        struct sigaction action;
        action.sa_sigaction = &bgmaster_server_sighandler;
        action.sa_flags = SA_SIGINFO;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0)
        {
            LOG_ERROR_MSG("bgmaster_server signal: " << strerror(errno));
            exit(1);
        }
    }

    // Construct master controller
    BGMasterController master;
    BGMasterController::_version_string = version.str();
    try {
        master.startup(props);
    } catch (BGMasterExceptions::ConfigError& e) {
        LOG_ERROR_MSG("Invalid config file entry: " << e.what());
        std::map<std::string, std::string> details;
        details["PID"] = boost::lexical_cast<std::string>(getpid());
        details["ERROR"] = e.what();
        BGMasterController::putRAS(MASTER_CONFIG_RAS, details);
    }

    // Stop threads
    BGMasterController::stopThreads(false, true, 15);

    // OpenSSL hygiene
    ENGINE_cleanup();
    CONF_modules_free();
}
