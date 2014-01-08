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


#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <utility/include/BoolAlpha.h>
#include <utility/include/version.h>
#include <utility/include/LoggingProgramOptions.h>

#include <boost/filesystem.hpp>

LOG_DECLARE_FILE( "master" );

std::string
setlogging(
        std::string& logdir,
        const std::string& hostname
        )
{
    if (logdir.empty()) {
        // Use default
        logdir = "/var/log";
    }

    const std::string logfile( logdir + "/" + hostname + "-bgagentd.log" );

    // Now open it. User and group readable. User writable.
    const int openfd = open(logfile.c_str(), O_WRONLY|O_APPEND|O_CREAT,
                      S_IRUSR|S_IWUSR|S_IRGRP);
    if (openfd == -1) {
        const std::string error_str = "Error opening log file " + logfile;
        perror(error_str.c_str());
        exit( EXIT_FAILURE );
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
        exit( EXIT_FAILURE );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    bgq::utility::BoolAlpha debug;
    std::string logdir;
    std::string workingdir;
    std::string users;

    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("debug,d", po::value(&debug)->implicit_value(true), "enable debug mode (do not daemonize)")
        ("logdir", po::value(&logdir), "path to logging directory")
        ("workingdir", po::value(&workingdir), "path to working directory")
        ("users", po::value(&users), "comma separated list of usernames")
        ;

    // Add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm.master" );
    lpo.addTo( options );

    // Add host option
    bgq::utility::ClientPortConfiguration host(
            32041,
            bgq::utility::ClientPortConfiguration::ConnectionType::Administrative
            );
    host.addTo( options );

    po::variables_map vm;
    po::command_line_parser cmd_line( argc, const_cast<char**>(argv) );
    cmd_line.options( options );
    const po::positional_options_description positional;
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
        exit( EXIT_SUCCESS );
    }

    host.setProperties( props, "master.agent" );
    host.notifyComplete();

    Agent agent( props );

    if (!debug._value) {
        // Now find the logdir
        if (logdir.empty()) {
            try {
                logdir = props->getValue("master.agent", "logdir");
            } catch (const std::invalid_argument& e) {
                LOG_ERROR_MSG("No logging directory specified or missing section. " << e.what());
                exit( EXIT_FAILURE );
            }
        }

        // Create log file and symlink
        setlogging(logdir, agent.get_hostname().uhn());
        // daemonize
        if (daemon(0, 1) < 0) {
            std::cerr << "Error trying to daemonize bgagentd: " << strerror(errno) << std::endl;
            exit( EXIT_FAILURE );
        }
    }

    if (debug._value) {
        LOG_INFO_MSG( argv[0] << " [" << getpid() << "] starting in debug mode." );
    }

    if (!workingdir.empty() ) {
        LOG_DEBUG_MSG("Changing working directory to: " << workingdir);
        const int rc = chdir(workingdir.c_str());
        if (rc) {
            LOG_FATAL_MSG("Could not change working directory to '" << workingdir << "', error is: " << strerror(errno));
            exit( EXIT_FAILURE );
        }
    }

    agent.set_users(users);
    
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

    LOG_INFO_MSG(
            "bgagentd [" << getpid() << "] Blue Gene/Q " << 
            boost::filesystem::basename( boost::filesystem::path(argv[0]) ) <<
            " " << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ") " <<
            __DATE__ << " " << __TIME__ << " starting"
            );
    agent.start( host.getPairs() );
}
