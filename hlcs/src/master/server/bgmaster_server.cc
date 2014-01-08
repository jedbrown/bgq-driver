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
#include "ras.h"

#include "lib/exceptions.h"

#include <utility/include/BoolAlpha.h>
#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/version.h>

#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <openssl/conf.h>
#include <openssl/engine.h>

#include <csignal>

#include <fcntl.h>

LOG_DECLARE_FILE( "master" );

namespace {

const std::vector<int> signals = boost::assign::list_of(SIGINT)(SIGUSR1)(SIGTERM)(SIGPIPE);

int signal_fd;

}

LockFile* lock_file = 0;

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
        LOG_WARN_MSG( "Host name error: " << strerror_r(errno, errorText, 256) );
    }

    const CxxSockets::Host host(hostname);

    const std::string logfile = logdir + "/" + host.uhn() + "-bgmaster_server.log";
    LOG_INFO_MSG( "Log file is " << logfile );
    // Now open it.  User and group readable.  User writable.
    const int openfd = open(logfile.c_str(), O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP);
    if (openfd == -1) {
        LOG_FATAL_MSG( "Error opening log file " << logfile );
        LOG_FATAL_MSG( strerror(errno) );
        return false;
    }

    // Send stdout and stderr to it.
    dup2(openfd, STDOUT_FILENO);
    dup2(openfd, STDERR_FILENO);

    // Not using it any more.
    close(openfd);

    return true;
}

int
main(int argc, const char** argv)
{
    // Parse --properties and --verbose before everything else
    namespace po = boost::program_options;
    bgq::utility::Properties::Ptr props;
    try {
        po::options_description temp;
        bgq::utility::Properties::ProgramOptions propertiesOptions;
        propertiesOptions.addTo( temp );
        bgq::utility::LoggingProgramOptions lpo( "ibm.master" );
        lpo.addTo( temp );
        po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
        cmd_line.allow_unregistered();
        cmd_line.options( temp );
        po::variables_map vm;
        po::store( cmd_line.run(), vm );
        po::notify( vm );

        // Create properties and initialize logging
        props = bgq::utility::Properties::create( propertiesOptions.getFilename() );
        bgq::utility::initializeLogging(*props, lpo, "master");
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading configuration file: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    bgq::utility::BoolAlpha debug;

    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("debug,d", po::value(&debug)->implicit_value(true), "enable debug mode (do not daemonize)")
        ;

    // Add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm.master" );
    lpo.addTo( options );

    po::variables_map vm;
    po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
    cmd_line.options( options );
    po::positional_options_description positional;
    cmd_line.positional( positional );
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

    std::string logdir;
    try {
        logdir = props->getValue("master.server", "logdir");
    } catch (const std::invalid_argument& e) {
        LOG_WARN_MSG( "No log directory found, will use default. Error is: " << e.what() );
    }

    std::string master_instances = "1";
    try {
        master_instances = props->getValue("master.policy.instances", "bgmaster_server");
    } catch (const std::invalid_argument& e) {
        // Don't care if it isn't there.
    }

    BOOST_SCOPE_EXIT( ( &lock_file ) ) {
        delete lock_file;
        lock_file = 0;
    } BOOST_SCOPE_EXIT_END;

    if (!debug._value) {
        if (master_instances == "1") {
            lock_file = new LockFile("bgmaster_server");
            if (lock_file->_fileExists ) {
                LOG_FATAL_MSG( 
                        "Lock file for bgmaster_server found. End bgmaster_server process "
                        << lock_file->_pid << " and remove " << lock_file->fname 
                        );
                exit(EXIT_FAILURE);
            }
        }

        if (!setlogging(logdir)) {
            if (lock_file) {
                delete lock_file;
                lock_file = 0;
            }
            exit(EXIT_FAILURE);
        }

        // Run as background process
        if (daemon(1, 1) < 0) {
            LOG_FATAL_MSG( "Error trying to daemonize bgmaster_server: " << strerror(errno) );
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
    for (size_t i = 0; i < signals.size(); ++i)
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
    MasterController master( props );
    try {
        master.startup(signal_descriptors[0]);
    } catch (const exceptions::ConfigError& e) {
        LOG_ERROR_MSG("Invalid configuration file entry: " << e.what());
        std::map<std::string, std::string> details;
        details["PID"] = boost::lexical_cast<std::string>(getpid());
        details["ERROR"] = e.what();
        MasterController::putRAS(MASTER_CONFIG_RAS, details);
    }

    // Stop threads
    MasterController::stopThreads(true, SIGTERM);

    // OpenSSL hygiene
    ENGINE_cleanup();
    CONF_modules_free();
}
