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

#include "LockFile.h"
#include "MasterController.h"
#include "MasterRasMetadata.h"

#include "lib/exceptions.h"

#include <utility/include/cxxsockets/SocketTypes.h>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/version.h>
#include <utility/include/Properties.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>

#include <openssl/conf.h>
#include <openssl/engine.h>

#include <csignal>
#include <cerrno>

LOG_DECLARE_FILE( "master" );

static int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE, SIGXFSZ, SIGABRT,
                         SIGSEGV, SIGILL, SIGFPE, SIGTERM, SIGBUS };
static int num_signals = sizeof(signals) / sizeof(signals[0]);

LockFile* lock_file = 0;

int signal_fd;

extern "C" void
bgmaster_server_sighandler(
        int /* signum */,
        siginfo_t* siginfo,
        void*
        )
{
    (void)write( signal_fd, siginfo, sizeof(siginfo_t) );
}

bool
setlogging(
        std::string& logdir
        )
{
    if (logdir.empty()) {
        // Use default
        logdir = "/var/log";
    }

    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        char errorText[256];
        std::cout << "Host name error: " << std::string(strerror_r(errno, errorText, 256));
    }

    CxxSockets::Host host(hostname);

    std::string logfile = logdir + "/" + host.uhn() + "-bgmaster_server.log";
    std::cerr << "Log file is " << logfile << std::endl;
    // Now open it.  User and group readable.  User writable.
    int openfd = open(logfile.c_str(), O_WRONLY|O_APPEND|O_CREAT,
                      S_IRUSR|S_IWUSR|S_IRGRP);
    if (openfd == -1) {
        std::string error_str = "Error opening log file " + logfile;
        std::cerr << error_str << std::endl;
        return false;
    }

    // Send stdout and stderr to it.
    dup2(openfd, STDOUT_FILENO);
    dup2(openfd, STDERR_FILENO);

    // Not using it any more.
    close(openfd);

    return true;
}

int main(int argc, const char** argv) {
    std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
    std::ostringstream version;
    version << "Blue Gene/Q " << basename << " " << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    bgq::utility::Properties::Ptr props;

    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("debug,d", po::bool_switch(), "enable debug mode (do not daemonize)")
        ;

    // Add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm.master" );
    lpo.addTo( options );

    // Parse --properties before everything else
    try {
        po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
        cmd_line.allow_unregistered();
        cmd_line.options( options );
        po::variables_map vm;
        po::store( cmd_line.run(), vm );
        po::notify( vm );

        // Create properties and initialize logging
        props = bgq::utility::Properties::create( propertiesOptions.getFilename() );
        bgq::utility::initializeLogging(*props, lpo, "master");
    } catch(std::runtime_error& e) {
        std::cerr << "Error reading configuration file: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    po::variables_map vm;
    po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
    cmd_line.options( options );
    try {
        po::store( cmd_line.run(), vm );

        // Notify variables_map that we are done processing options
        po::notify( vm );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( vm["help"].as<bool>() ) {
        std::cout << argv[0] << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << options << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Initialize properties for everybody
    const std::string default_logger( "ibm.master" );
    bgq::utility::LoggingProgramOptions logging_program_options( default_logger );

    bgq::utility::initializeLogging(*props, logging_program_options, "master");
    std::string logdir = "";
    try {
        logdir = props->getValue("master.server", "logdir");
    } catch(std::invalid_argument& e) {
        std::cerr << "No log directory found, will use default. Error is: " << e.what() << std::endl;
    }

    std::cout << "bgmaster_server [" << getpid() + 1 << "] " << version.str() << " starting..." << std::endl;

    std::string master_instances = "1";
    try {
        master_instances = props->getValue("master.policy.instances", "bgmaster_server");
    } catch(std::invalid_argument& e) {
        // Don't care if it isn't there.
    }

    BOOST_SCOPE_EXIT( ( &lock_file ) ) {
        delete lock_file;
        lock_file = 0;
    } BOOST_SCOPE_EXIT_END;

    if (!vm["debug"].as<bool>()) {
        if (master_instances == "1") {
            lock_file = new LockFile("bgmaster_server");
            if (lock_file->_fileExists ) {
                std::cerr << "Lock file for bgmaster_server found. End bgmaster_server process "
                    << lock_file->_pid << " and remove " << lock_file->fname << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        if (!setlogging(logdir)) {
            if (lock_file) {
                delete lock_file;
                lock_file = 0;
            }
            exit(0);
        }

        // Run as background process
        if (daemon(1, 1) < 0) {
            std::cerr << "Error trying to daemonize bgmaster_server: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }

    if (lock_file) {
        lock_file->setpid();
    }

    // Create pipe for signal handler
    int signal_descriptors[2];
    if ( pipe(signal_descriptors) != 0 ) {
        LOG_ERROR_MSG( "Could not create pipe for signal handler." );
        exit( EXIT_FAILURE );
    }
    signal_fd = signal_descriptors[1];

    // Signal handlers
    for (int i = 0; i < num_signals; i++)
    {
        struct sigaction action;
        action.sa_sigaction = &bgmaster_server_sighandler;
        action.sa_flags = SA_SIGINFO;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0)
        {
            LOG_ERROR_MSG("Error setting up bgmaster_server signal handler: " << strerror(errno));
            exit(1);
        }
    }

    // Construct master controller
    MasterController master;
    MasterController::_version_string = version.str();
    try {
        master.startup(props, signal_descriptors[0]);
    } catch (exceptions::ConfigError& e) {
        LOG_ERROR_MSG("Invalid configuration file entry: " << e.what());
        std::map<std::string, std::string> details;
        details["PID"] = boost::lexical_cast<std::string>(getpid());
        details["ERROR"] = e.what();
        MasterController::putRAS(MASTER_CONFIG_RAS, details);
    }

    // Stop threads
    MasterController::stopThreads(false, true, 15);

    // OpenSSL hygiene
    ENGINE_cleanup();
    CONF_modules_free();
}
