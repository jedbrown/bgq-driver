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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "areBlocksActive.h"
#include "BlockControllerBase.h"
#include "CommandProcessor.h"
#include "ConsoleListener.h"
#include "ConsoleMonitorParameters.h"
#include "DatabaseMonitorThread.h"
#include "DefaultListener.h"
#include "mcServerInit.h"
#include "Options.h"
#include "ras.h"
#include "ReconnectBlocks.h"
#include "RunJobConnection.h"

#include "env/Monitor.h"

#include "master/Monitor.h"

#include "common/AbstractCommand.h"
#include "common/Properties.h"

#include "libmmcs_client/ConsolePort.h"

#include <bgq_util/include/TempFile.h>
#include <bgq_util/include/string_tokenizer.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/ioUsage.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>
#include <utility/include/performance.h>
#include <utility/include/version.h>

#include <boost/scoped_ptr.hpp>

#include <openssl/rand.h>

#include <cerrno>
#include <csignal>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <poll.h>

#include <sys/resource.h>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace mmcs::server;

using mmcs::common::Properties;

DatabaseMonitorThread*          databaseMonitor = NULL;     // background thread for handling commands via database
env::Monitor::Ptr               envMonitor;
ConsoleListener*            consoleListener = NULL;     // background thread for handling commands from console processes

namespace {

int signal_fd = 0;

}

void
mmcs_signal_handler(int signum, siginfo_t* siginfo, void*)
{
    if ( signal_fd ) {
        (void)write( signal_fd, siginfo, sizeof(siginfo_t) );
        return;
    }

    if (signum == SIGUSR1 || signum == SIGPIPE || signum == SIGHUP) {
        return;
    }

    // Otherwise restore default handler and re-raise
    struct sigaction sigact;
    sigact.sa_handler = SIG_DFL;
    (void)sigaction(signum, &sigact, 0);
    raise(signum);
}

void
mmcs_terminate()
{
    // Terminate MMCS console port listener thread
    if (consoleListener != NULL) {
        consoleListener->stop(SIGUSR1);
        delete consoleListener;
        consoleListener = NULL;
    }

    // Terminate database monitor thread
    if (databaseMonitor != NULL) {
        databaseMonitor->stop(SIGUSR1);
        delete databaseMonitor;
        databaseMonitor = NULL;
    }

    // Terminate environmental monitor thread
    if (envMonitor) {
        envMonitor->stop();
        envMonitor.reset();
    }

    RunJobConnection::instance().stop();
}

int
main(int argc, char **argv)
{
    // Parse command line parameters
    const mmcs::server::Options options(argc, argv);

    struct rlimit rlimit_nofile = {0, 0};     // process limit on number of files
    struct rlimit rlimit_core = {0, 0};       // process limit on core file size
    getrlimit(RLIMIT_CORE, &rlimit_core);     // get the current process core file limit
    getrlimit(RLIMIT_NOFILE, &rlimit_nofile); // get the current process file descriptor limit
    LOG_DEBUG_MSG("Core limits: " << rlimit_core.rlim_cur);
    LOG_DEBUG_MSG("File limits: " << rlimit_nofile.rlim_cur);

    // Seed random number generator
    {
        // /dev/urandom will not block
        const int rc = RAND_load_file( "/dev/urandom", 16 );
        if ( rc != 16 ) {
            LOG_WARN_MSG("Could not seed random number generator. Security may be compromised.");
        }
    }


    // Initialize the RasEventHandlerChain
    RasEventHandlerChain::initChain();
    RasEventHandlerChain::setProperties( Properties::getProperties() );

    // Initialize the database for the process
    BGQDB::init( Properties::getProperties(), "mmcs" );

    // Initialize performance counters
    bgq::utility::performance::init( Properties::getProperties() );

    // Create the list of mmcs_server commands
    mmcs::common::AbstractCommand::Attributes attr;  attr.mmcsServerCommand(true);
    mmcs::common::AbstractCommand::Attributes mask;  mask.mmcsServerCommand(true);
    boost::scoped_ptr<mmcs::MMCSCommandMap> mmcsCommands(
            mmcs::MMCSCommandProcessor::createCommandMap(attr, mask)
            );

    // Monitor the console port for incoming connections
    boost::scoped_ptr<mmcs_client::ConsolePortServer> consolePort;
    try {
        consolePort.reset( new mmcs_client::ConsolePortServer(options.getServerConfig()->getPairs()) );
        LOG_INFO_MSG("Console port successfully opened to listen for incoming client connections.");
    } catch (const mmcs_client::ConsolePort::Error& e) {
        if (e.errcode == EADDRINUSE) {
            LOG_FATAL_MSG("Console port is already in use, there may be another mmcs_server running: " << e.what() );
        } else {
            LOG_FATAL_MSG("Problem trying to open console port: " << e.what());
        }
        exit(1);
    } catch (const std::runtime_error& e) {
        LOG_FATAL_MSG("Failed to open console server port: " << e.what());
        exit(1);
    }

    // fetch machine description from the database
    std::vector<std::string> invalid_memory_locations;
    std::stringstream machineStream;
    int result = BGQDB::getMachineXML(machineStream, &invalid_memory_locations);
    if (result != BGQDB::OK) {
        LOG_FATAL_MSG("Cannot fetch machine description");
        if (result == BGQDB::XML_ERROR) {
            LOG_FATAL_MSG( " -- check directory permissions" );
        } else {
            // Dump the XML stream to /tmp
            TempFile machineFile("__MMCS_tmp_machine.XXXXXX");
            machineFile.keep = true;
            (void)write(machineFile.fd, machineStream.str().c_str(), machineStream.str().size());
            LOG_FATAL_MSG( " -- XML file is " << machineFile.fname );
        }
        exit(1);
    }

    // For any locations reported by getMachineXML where the memory size is not valid send a RAS event.
    for (
            std::vector<std::string>::const_iterator i(invalid_memory_locations.begin());
            i != invalid_memory_locations.end();
            ++i
        )
    {
        RasEventImpl badMem(MMCSOps_0009);
        badMem.setDetail(RasEvent::LOCATION, *i);
        RasEventHandlerChain::handle(badMem);
        BGQDB::putRAS(badMem);
    }

    // Establish signal handler for ctrl-C, etc
    const std::vector<int> signals = boost::assign::list_of
        (SIGHUP)(SIGINT)(SIGQUIT)(SIGUSR1)(SIGTERM)(SIGPIPE)
        (SIGSEGV)(SIGILL)(SIGFPE)(SIGTERM)(SIGBUS)
        ;

    for (size_t i = 0; i < signals.size(); i++) {
        struct sigaction action;
        action.sa_sigaction = &mmcs_signal_handler;
        action.sa_flags = SA_SIGINFO;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0) {
            LOG_ERROR_MSG("mmcs_server signal: " << strerror(errno));
            exit(1);
        }
    }

    // Initialize I/O usage statics.
    BGQDB::ioUsage::init();

    // Get the bringup options from bg.properties
    const std::string bringupString = Properties::getProperty(BRINGUP_OPTS);
    StringTokenizer bringup_options;
    bringup_options.tokenize(bringupString, ",");
    // Add bringup options from command line
    for (
            StringTokenizer::const_iterator it = options.bringupOptions().begin();
            it != options.bringupOptions().end();
            ++it
        )
    {
        if (
                find( bringup_options.begin(), bringup_options.end(), *it )
                == bringup_options.end()
           )
        {
            bringup_options.push_back(*it);
        }
    }

    const bool blocks_are_booted = areBlocksActive();

    // Start the runjob connection
    (void)RunJobConnection::instance();

    // Initialize mcServer. This has to come after the master monitor starts and before
    // reconnect happens. It isn't threaded, so there's no race condition with reconnectBlocks().
    if (
            Properties::getProperty(POLL_DB) == "false" &&
            Properties::getProperty(POLL_ENVS) == "false"
       )
    {
        LOG_INFO_MSG( "Database polling disabled" );
        LOG_INFO_MSG( "Environmental polling disabled" );
    } else {
        mmcs_client::CommandReply reply;
        mcServerInit(bringup_options, reply, blocks_are_booted);
        if (reply.getStatus()) {
            LOG_ERROR_MSG("Initialization with mc_server failed: " << reply.str());
            exit(1);
        }
    }

    // Start the master monitor before reconnecting.
    boost::scoped_ptr<master::Monitor> master_mon;
    if (Properties::getProperty(MASTER_MON) == "true") {
        master_mon.reset( new master::Monitor );
        master_mon->setOptions(bringup_options);
        master_mon->start();

        // Wait for the master monitor to complete its initialization.
        boost::unique_lock<boost::mutex> ulock(master::Monitor::_startup_lock);
        while (master::Monitor::_started == false) {
            master::Monitor::_startup_notifier.wait(ulock);
        }
    } else {
        // We can't reconnect blocks when master_mon is disabled
        LOG_DEBUG_MSG( "Disabling block reconnection because master_mon = false" );
        Properties::setProperty(RECONNECT_BLOCKS, "false");
    }

    // The MMCS server that is running the db monitor thread is the only one that
    // will create permanent target sets or perform reconnect_blocks
    if (Properties::getProperty(POLL_DB) == "true") {
        // Set default target set type
        Properties::setProperty(DFT_TGTSET_TYPE, "targetset=perm");

        // Reconnect blocks
        try {
            mmcs::server::CommandProcessor commandProcessor( mmcsCommands.get() );
            reconnectBlocks(&commandProcessor);
        } catch ( const std::exception& e ) {
            LOG_FATAL_MSG( "Reconnect blocks: " << e.what() );
            _exit( EXIT_FAILURE );
        }

        // Start the database monitor thread
        databaseMonitor = new DatabaseMonitorThread( mmcsCommands.get() );
        databaseMonitor->start();
    } else {
        // Set default target set type
        Properties::setProperty(DFT_TGTSET_TYPE, "targetset=temp");
    }

    // Start the environmental monitor threads
    if (Properties::getProperty(POLL_ENVS) == "true") {
        LOG_INFO_MSG("Reconnect done, starting environment monitor.");
        std::vector<std::string> racks;
        unsigned i = 0;
        while ( true ) {
            const std::string subnet = "Name." + boost::lexical_cast<std::string>(i);
            const std::string subnet_name = Properties::getProperty(subnet);
            if ( subnet_name.empty() ) break;

            const std::string subnet_hw = Properties::getProperty(subnet_name);
            racks.push_back(subnet_hw);
            ++i;
        }

        std::vector<std::string> invalidHW;
        result = BGQDB::checkRack(racks, invalidHW);
        if (result != BGQDB::OK) {
            LOG_FATAL_MSG("Error on BGQDB::checkRacks; exiting.");
            _exit( EXIT_FAILURE );
        }
        if (!invalidHW.empty()) {
            for (size_t i=0; i < invalidHW.size(); ++i) {
                LOG_FATAL_MSG("HardwareToManage: rack " << invalidHW[i] << " in properties file, but not found in database");
            }
            LOG_FATAL_MSG("Exiting due to inconsistency between properties file and database data.");
            _exit( EXIT_FAILURE );
        }

        envMonitor = env::Monitor::create();
    }

    // Start the consolePort listener thread
    consoleListener = new ConsoleListener;
    const boost::shared_ptr<ConsoleMonitorParameters> consoleParms(
            new ConsoleMonitorParameters( consolePort.get(), mmcsCommands.get() )
            );
    consoleListener->setArg(consoleParms);
    consoleListener->start();

    LOG_INFO_MSG("mmcs_server started");

    // Create pipe for signal handler
    int signal_descriptors[2];
    if ( pipe(signal_descriptors) != 0 ) {
        LOG_FATAL_MSG( "Could not create pipe for signal handler" );
        exit( EXIT_FAILURE );
    }
    signal_fd = signal_descriptors[1];

    while ( true ) {
        struct pollfd pollfd;
        pollfd.fd = signal_descriptors[0];
        pollfd.events = POLLIN;
        pollfd.revents = 0;
        const int seconds = 5000;
        const int rc = poll( &pollfd, 1, seconds );

        if ( rc == -1 ) {
            if ( errno != EINTR) {
                LOG_ERROR_MSG( "Could not poll: " << strerror(errno) );
            }
        } else if ( rc ) {
            // Read siginfo from pipe
            siginfo_t siginfo;
            while ( 1 ) {
                const int rc = read( signal_descriptors[0], &siginfo, sizeof(siginfo) );
                if ( rc > 0 )  {
                    break;
                }
                if ( rc == -1 && errno == EINTR ) {
                    continue;
                }
                LOG_FATAL_MSG( "Could not read: " << strerror(errno) );
                exit(1);
            }
            if ( siginfo.si_signo == SIGUSR1 || siginfo.si_signo == SIGPIPE || siginfo.si_signo == SIGHUP) {
                LOG_DEBUG_MSG( "Received signal " << siginfo.si_signo << " from " << siginfo.si_pid );
            } else {
                // Restore default signal handler
                struct sigaction act;
                memset(&act, 0, sizeof(act));
                act.sa_handler = SIG_DFL;
                sigaction(siginfo.si_signo, &act, NULL);

                LOG_WARN_MSG( "mmcs_server halting due to signal " << siginfo.si_signo );
                LOG_WARN_MSG( "Sent from pid " << siginfo.si_pid );
                LOG_WARN_MSG( "Sent from uid " << siginfo.si_uid );
                raise( siginfo.si_signo );
            }
        } else if ( !rc ) {
            if (!DefaultListener::get()->getBase()->isMailboxStarted()) {
                LOG_WARN_MSG("mc_server has terminated, mmcs_server must now terminate.");
                break;
            }
        }
    }

    // End the various mmcs threads
    mmcs_terminate();

    // Use _exit to avoid calling destructors for objects with static storage duration.
    // We need this because mmcs_server does not join all of its threads, so they can
    // persist when the main thread falls below main here.
    _exit(EXIT_FAILURE);
}
