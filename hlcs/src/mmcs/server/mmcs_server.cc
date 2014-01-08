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


#include "BlockControllerBase.h"
#include "CommandProcessor.h"
#include "ConsoleListener.h"
#include "ConsoleMonitorParameters.h"
#include "DatabaseMonitorThread.h"
#include "DefaultControlEventListener.h"
#include "mcServerInit.h"
#include "Parameters.h"
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
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/TxObject.h>
#include <db/include/api/ioUsage.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>
#include <utility/include/performance.h>
#include <utility/include/version.h>

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/filesystem.hpp>
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


using namespace std;

using namespace mmcs::server;

using mmcs::common::Properties;


DatabaseMonitorThread*          databaseMonitor = NULL;     // background thread for handling commands via database
env::Monitor*                   envMonitor = NULL;          // background thread for handling environmentals
ConsoleListener*            consoleListener = NULL;     // background thread for handling commands from console processes
RunJobConnectionMonitor*        runjob_mon = 0;
boost::mutex                    reconnect_lock;
bool                            reconnect_done = false;
boost::condition_variable       reconnect_notifier;

namespace {

const int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE,
                        SIGSEGV, SIGILL, SIGFPE, SIGTERM, SIGBUS };
const unsigned num_signals = sizeof(signals) / sizeof(signals[0]);

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

    // otherwise restore default handler and re-raise
    struct sigaction sigact;
    sigact.sa_handler = SIG_DFL;
    (void)sigaction(signum, &sigact, 0);
    raise(signum);
}

void
mmcs_terminate()
{
    // terminate MMCS console port listener thread
    if (consoleListener != NULL)
    {
        consoleListener->stop(SIGUSR1);
        delete consoleListener;
        consoleListener = NULL;
    }

    // terminate database monitor thread
    if (databaseMonitor != NULL)
    {
        databaseMonitor->stop(SIGUSR1);
        delete databaseMonitor;
        databaseMonitor = NULL;
    }

    // terminate environmental monitor thread
    if (envMonitor != NULL)
    {
        envMonitor->stop(SIGUSR1);
        delete envMonitor;
        envMonitor = NULL;
    }

    if(runjob_mon != NULL) {
        runjob_mon->stop(SIGUSR1);
        delete runjob_mon;
        runjob_mon = NULL;
    }
}

// Returns true if any block is not in FREE state
bool
areBlocksActive()
{
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return false;
    }

    cxxdb::ResultSetPtr result = connection->query(
            "SELECT count(*) as count from tbgqblock where status <> 'F'"
            );
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "could not get active block count" );
        return false;
    }
    if ( result->columns()[ "count" ].getInt32() ) {
        LOG_INFO_MSG( "found " << result->columns()[ "count" ].getInt32() << " block(s) active" );
        return true;
    }

    result = connection->query(
            "SELECT count(*) as count from tbgqnodecard where status = 'S' "
            "UNION ALL "
            "SELECT count(*) as count from tbgqiodrawer where status = 'S'"
            );
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "could not get node board in service count" );
        return false;
    }
    if ( result->columns()[ "count" ].getInt32() ) {
        LOG_INFO_MSG( "found " << result->columns()[ "count" ].getInt32() << " I/O drawer(s) in Service" );
        return true;
    }
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "could not get I/O drawers in service count" );
        return false;
    }
    if ( result->columns()[ "count" ].getInt32() ) {
        LOG_INFO_MSG( "found " << result->columns()[ "count" ].getInt32() << " node board(s) in Service" );
        return true;
    }

    // getting here means no blocks are active, and no hardware is in service
    return false;
}

int
main(int argc, char **argv)
{
    std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
    ostringstream version;
    version << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    Properties::setProperty(MMCS_VERSION, version.str());
    Properties::setProperty(MMCS_PROCESS, basename);// do this before parsing command line parameters

    // Parse command line parameters
    mmcs::server::Parameters mmcsParms(argc, argv);

    // log args
    std::ostringstream args;
    std::copy(argv, argv + argc, std::ostream_iterator<char*>(args, " "));
    LOG_INFO_MSG("MMCS[" << getpid() << "]: starting: " << Properties::getProperty(MMCS_VERSION));
    LOG_INFO_MSG("Startup parameters: " << args.str());

    struct rlimit rlimit_nofile = {0, 0}; // process limit on number of files
    struct rlimit rlimit_core = {0, 0};    // process limit on core file size
    getrlimit(RLIMIT_CORE, &rlimit_core); // get the current process core file limit
    getrlimit(RLIMIT_NOFILE, &rlimit_nofile); // get the current process file descriptor limit
    LOG_DEBUG_MSG("Core limits: " << rlimit_core.rlim_cur);
    LOG_DEBUG_MSG("File limits: " << rlimit_nofile.rlim_cur);

    // Seed random number generator
    {
        // /dev/urandom will not block
        const int rc = RAND_load_file( "/dev/urandom", 16 );
        if ( rc != 16 ) {
            LOG_WARN_MSG("Could not seed random number generator.  Security may be compromised.");
        }
    }


    // Initialize the RasEventHandlerChain
    RasEventHandlerChain::initChain();
    RasEventHandlerChain::setProperties( Properties::getProperties() );

    // Initialize the database for the process
    BGQDB::init( Properties::getProperties(), "mmcs" );

    // initialize performance counters
    bgq::utility::performance::init( Properties::getProperties() );

    // create the list of mmcs_server commands
    mmcs::common::AbstractCommand::Attributes attr;  attr.mmcsServerCommand(true);
    mmcs::common::AbstractCommand::Attributes mask;  mask.mmcsServerCommand(true);
    boost::scoped_ptr<mmcs::MMCSCommandMap> mmcsCommands(
            mmcs::MMCSCommandProcessor::createCommandMap(attr, mask)
            );

    // Create the command processor
    boost::scoped_ptr<mmcs::server::CommandProcessor> commandProcessor(
            new mmcs::server::CommandProcessor( mmcsCommands.get() )
            );

    // Monitor the console port for incoming connections
    boost::scoped_ptr<mmcs_client::ConsolePortServer> consolePort;
    try {
        bgq::utility::PortConfiguration::Pairs portpairs;
        std::string servname = "32031";
        bgq::utility::ServerPortConfiguration port_config(servname);
        port_config.setProperties( Properties::getProperties(), "mmcs");
        port_config.notifyComplete();
        if (!mmcsParms._listeners_set) {
            // Using the config file
            portpairs = port_config.getPairs();
        } else {
            // We got a command line parameter, so use that instead.
            std::string portstr = Properties::getProperty(LISTEN_PORTS);
            bgq::utility::PortConfiguration::parsePortsStr(portstr, servname, portpairs);
        }
        consolePort.reset( new mmcs_client::ConsolePortServer(portpairs) );
        LOG_INFO_MSG("MMCS: console port open.");
    } catch (const mmcs_client::ConsolePort::Error& e) {
        if (e.errcode == EADDRINUSE) {
            LOG_ERROR_MSG("MMCS: console port is already in use, there may be another mmcs_server running: " << e.what() );
        } else {
            LOG_ERROR_MSG("MMCS: mmcs_client::ConsolePort error: " << e.what());
        }
        exit(1);
    } catch (const std::runtime_error& e) {
        LOG_ERROR_MSG("Failed to create console server port " << e.what());
        exit(1);
    }

    // fetch machine description from the database
    vector<string> invalid_memory_locations;
    stringstream machineStream;
    int result = BGQDB::getMachineXML(
            machineStream,
            &invalid_memory_locations
            );
    if (result != BGQDB::OK) {
        LOG_FATAL_MSG("MMCS can't fetch machine description");
        if (result == BGQDB::XML_ERROR) {
            LOG_FATAL_MSG( " -- check directory permissions" );
        } else {
            //dump the XML stream to /tmp
            TempFile machineFile("__MMCS_tmp_machine.XXXXXX");
            machineFile.keep = true;
            (void)write(machineFile.fd, machineStream.str().c_str(), machineStream.str().size());
            LOG_FATAL_MSG( " -- XML file is " << machineFile.fname );
        }
        exit(1);
    }

    // For any locations reported by getMachineXML where the memory size is not valid,
    // send a RAS event.

    for (
            vector<string>::iterator i(invalid_memory_locations.begin());
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
    for (unsigned i = 0; i < num_signals; i++) {
        struct sigaction action;
        action.sa_sigaction = &mmcs_signal_handler;
        action.sa_flags = SA_SIGINFO;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0) {
            LOG_ERROR_MSG("MMCS signal: " << strerror(errno));
            exit(1);
        }
    }

    // Initialize IO usage statics.
    BGQDB::ioUsage::init();

    StringTokenizer bringup_options;
    string bringupString;
    mmcs_client::CommandReply reply;

    // get the bringup options from bg.properties
    bringupString = Properties::getProperty(BRINGUP_OPTS);
    bringup_options.tokenize(bringupString, ",");
    // add bringup options from command line
    for (
            StringTokenizer::iterator it = mmcsParms._bringup_options.begin();
            it != mmcsParms._bringup_options.end();
            ++it
        )
    {
        if (
                find(
                    bringup_options.begin(),
                    bringup_options.end(),
                    *it
                    )
                == bringup_options.end()
           )
        {
            bringup_options.push_back(*it);
        }
    }

    bool blocks_are_booted = areBlocksActive();

    // Start the runjob connection
    if (Properties::getProperty(RUNJOB_CONNECT) == "true") {
        runjob_mon = new RunJobConnectionMonitor;
        runjob_mon->start();
    }

    // Initialize mcServer.  This has to come after the master monitor starts and
    // before reconnect happens.  It isn't threaded, so there's no race condition
    // with reconnectBlocks().
    if (
            Properties::getProperty(POLL_DB) == "false" &&
            Properties::getProperty(POLL_ENVS) == "false"
       )
    {
        LOG_INFO_MSG( "database polling disabled" );
        LOG_INFO_MSG( "environmental polling disabled" );
    } else {
        mcServerInit(bringup_options, reply, blocks_are_booted);
        if (reply.getStatus() != 0) {
            LOG_ERROR_MSG("mcserver initialization failed: " << reply.str());
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
        while(master::Monitor::_started == false) {
            master::Monitor::_startup_notifier.wait(ulock);
        }
    } else {
        // we can't reconnect blocks when master_mon is disabled
        LOG_DEBUG_MSG( "disabling block reconnection because master_mon = false" );
        Properties::setProperty(RECONNECT_BLOCKS, "false");
    }

    // The MMCS server that is running the db monitor thread is the only one that
    // will create permanent target sets or perform reconnect_blocks
    if (Properties::getProperty(POLL_DB) == "true") {
        // set default target set type
        Properties::setProperty(DFT_TGTSET_TYPE, "targetset=perm");

        // Reconnect or free blocks and mcserver targets
        try {
            reconnectBlocks(commandProcessor.get());
        } catch ( const std::exception& e ) {
            LOG_FATAL_MSG( "reconnect blocks: " << e.what() );
            _exit( EXIT_FAILURE );
        }

        // Start the database monitor thread
        databaseMonitor = new DatabaseMonitorThread( mmcsCommands.get() );
        databaseMonitor->monitorBlockTable = Properties::getProperty(POLL_DB) == "true"? true:false;
        databaseMonitor->start();
    } else {
        // set default target set type
        Properties::setProperty(DFT_TGTSET_TYPE, "targetset=temp");
    }

    if (Properties::getProperty(POLL_DB) == "true") {
        // Start the environmental monitor threads
        boost::unique_lock<boost::mutex> recon_lock(reconnect_lock);
        while(reconnect_done == false) {
            reconnect_notifier.wait(recon_lock);
        }
    }

    if (Properties::getProperty(POLL_ENVS) == "true") {
        LOG_INFO_MSG("Reconnect done, starting environment monitor.");
        std::vector<std::string> racks;
        unsigned i = 0;
        while(true) {
            std::string subnet = "Name." + boost::lexical_cast<std::string>(i);
            std::string subnet_name = Properties::getProperty((char*)(subnet.c_str()));
            if(subnet_name.length() == 0)
                break;
            std::string subnet_hw = Properties::getProperty((char*)(subnet_name.c_str()));
            racks.push_back(subnet_hw);
            ++i;
        }

        std::vector<std::string>  invalidHW;
        result = BGQDB::checkRack(racks, &invalidHW);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG("Error on BGQDB::checkRacks; exiting.");
            _exit( EXIT_FAILURE );
        }
        if (!invalidHW.empty()) {
            for (unsigned i=0; i < invalidHW.size(); ++i) {
                LOG_FATAL_MSG("HardwareToManage: rack " << invalidHW[i] << " in properties file, but not found in database");
            }
            LOG_FATAL_MSG("exiting");
            _exit( EXIT_FAILURE );
        }

        envMonitor = new env::Monitor;
        envMonitor->start();
    }

    // Start the consolePort listener thread
    consoleListener = new ConsoleListener;
    const boost::shared_ptr<ConsoleMonitorParameters> consoleParms(
            new ConsoleMonitorParameters( consolePort.get(), mmcsCommands.get() )
            );
    consoleListener->setArg(consoleParms);
    consoleListener->start();

    LOG_INFO_MSG("MMCS started");

    // create pipe for signal handler
    int signal_descriptors[2];
    if ( pipe(signal_descriptors) != 0 ) {
        LOG_ERROR_MSG( "could not create pipe for signal handler" );
        exit( EXIT_FAILURE );
    }
    signal_fd = signal_descriptors[1];

    while ( true ) {
        struct pollfd pollfd;
        pollfd.fd = signal_descriptors[0];
        pollfd.events = POLLIN;
        pollfd.revents = 0;
        const int seconds = 5;
        const int rc = poll( &pollfd, 1, seconds );

        if ( rc == -1 ) {
            if ( errno != EINTR) LOG_ERROR_MSG( "could not poll: " << strerror(errno) );
        } else if ( rc ) {
            // read siginfo from pipe
            siginfo_t siginfo;
            while ( 1 ) {
                const int rc = read( signal_descriptors[0], &siginfo, sizeof(siginfo) );
                if ( rc > 0 ) break;
                if ( rc == -1 && errno == EINTR ) continue;
                LOG_FATAL_MSG( "could not read: " << strerror(errno) );
                exit(1);
            }
            if ( siginfo.si_signo == SIGUSR1 || siginfo.si_signo == SIGPIPE || siginfo.si_signo == SIGHUP) {
                LOG_DEBUG_MSG( "received signal " << siginfo.si_signo << " from " << siginfo.si_pid );
            } else {
                // restore default signal handler
                struct sigaction act;
                memset(&act, 0, sizeof(act));
                act.sa_handler = SIG_DFL;
                sigaction(siginfo.si_signo, &act, NULL);

                LOG_WARN_MSG( "mmcs_server halting due to signal " << siginfo.si_signo );
                LOG_WARN_MSG( "sent from pid " << siginfo.si_pid );
                LOG_WARN_MSG( "sent from uid " << siginfo.si_uid );
                raise( siginfo.si_signo );
            }
        } else if ( !rc ) {
            if (!DefaultControlEventListener::getDefaultControlEventListener()->getBase()->isMailboxStarted()) {
                LOG_WARN_MSG("MCServer has terminated. MMCS must now terminate.");
                break;
            }
        }
    }

    // end the various mmcs threads
    mmcs_terminate();

    // use _exit to avoid calling destructors for objects with static storage duration.
    // we need this because mmcs_server does not join all of its threads, so they can

    // persist when the main thread falls below main here.
    _exit(EXIT_FAILURE);
}
