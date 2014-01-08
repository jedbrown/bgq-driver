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

#include <cerrno>
#include <csignal>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <openssl/rand.h>
#include <sys/resource.h>

#include <bgq_util/include/TempFile.h>
#include <bgq_util/include/string_tokenizer.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/tableapi/TxObject.h>
#include <db/include/api/ioUsage.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>
#include <utility/include/performance.h>
#include <utility/include/version.h>

#include <boost/filesystem.hpp>

#include "DefaultControlEventListener.h"
#include "mcServerInit.h"
#include "MMCSConsoleMonitor.h"
#include "MMCSConsolePort.h"
#include "MMCSDBMonitor.h"
#include "MMCSEnvMonitor.h"
#include "MMCSProperties.h"
#include "MMCSCommandProcessor.h"
#include "MMCSSocketList.h"
#include "MMCSServerParms.h"
#include "ReconnectBlocks.h"
#include "MMCSMasterMonitor.h"
#include "RunJobConnection.h"
#include "ras.h"

using namespace std;

LOG_DECLARE_FILE( "mmcs" );


MMCSConsolePortServer*          consolePort = NULL;         // server side console port for listening for consoles and scripts
DatabaseMonitorThread*          databaseMonitor = NULL;     // background thread for handling commands via database
EnvMonitorThread*               envMonitor = NULL;          // background thread for handling environmentals
MasterMonitor*                  master_mon = NULL;          // background thread for handling bgmaster monitoring
MMCSConsoleListener*            consoleListener = NULL;     // background thread for handling commands from console processes
MMCSSocketList*                 clientSockets = NULL;       // list of client sockets
MMCSCommandMap*                 mmcsCommands = NULL;        // map of mmcs commands to MMCSCommand objects
MMCSServerCommandProcessor*     commandProcessor = NULL;    // executes mmcs server commands
volatile int                    signal_number = 0;          // flag to terminate mmcs -- set to termination signal by signal handler
bool                            test_mode = false;
RunJobConnection*               runjob_connection = 0;
LocationThreadList*             locThreads = NULL;
RunJobConnectionMonitor*        runjob_mon = 0;
boost::mutex                    reconnect_lock;
bool                            reconnect_done = false;
boost::condition_variable       reconnect_notifier;
MCServerMessageType             message_type;
bool                            subnets_home = true;

static int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE, SIGXFSZ, 
                         SIGSEGV, SIGILL, SIGFPE, SIGTERM, SIGBUS };
static int num_signals = sizeof(signals) / sizeof(signals[0]);

void
mmcs_sigxfsz_handler(int signal_number)
{
    // This signal occurs when a write to the MMCS log or an I/O log
    // would cause the file size limit to be exceeded.
    static int signalCount = 0;

    if (--signalCount < 0) // limit the number of log messages written
    {
        signalCount = 1000;

        // generate a RAS event
        const char* rasMessage = "insert into tbgqeventlog (component,errcode,severity,message) values('MMCS','0','ERROR','mmcs or I/O log file size limit exceeded')";
        BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
        if (!tx.getConnection()) {
            LOG_ERROR_MSG("Unable to obtain database connection");
            return;
        }
        tx.execStmt(rasMessage);

        LOG_ERROR_MSG("MMCS log or I/O log file size limit exceeded");
    }
}

void
mmcs_signal_handler(int signum, siginfo_t* siginfo, void*)
{
    if (signum == SIGUSR1 || signum == SIGPIPE || signum == SIGHUP)
    {
        return;
    }
    else if (signum == SIGXFSZ)
    {
        mmcs_sigxfsz_handler(signum);
    }
    else
    {
        //  print some useful information
        std::cerr << "mmcs_server halting due to signal " << signum << " in thread "
                  << pthread_self() << " from " << siginfo->si_pid << "." << std::endl;
        if (signal_number == 0)
        {
            signal_number = signum;
        }
        // Reset to default action for our signal
        struct sigaction sigact;
        sigact.sa_handler = SIG_DFL;
        int rc = sigaction(signum, &sigact, 0);
        if (rc < 0)
            {
                std::cerr << "mmcs_server signal: " << strerror(errno) << std::endl;
                exit(1);
            }
        // now raise it again
        raise(signum);
    }
}

void
mmcs_terminate(void)
{
    // stop console listener socket
//     if (consolePort)
//     {
//         shutdown(consolePort->getFd(), SHUT_RDWR);
//     }

    // stop client sockets
    if (clientSockets)
    {
        clientSockets->stopConnections();
    }

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

    if(runjob_mon != 0) {
        runjob_mon->stop(SIGUSR1);
        delete runjob_mon;
        runjob_mon = 0;
    }

    // skip the mcserverTerm if we are a --test server
    if (test_mode == false) {
        // terminate the generic RAS listener
        MMCSCommandReply reply;
        mcServerTerm(reply);
    }
}

// Returns true if any block is not in FREE state
bool areBlocksActive() {
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::STATUS result;
    std::vector<string> allBlocks;
    string whereClause = "";
    result = BGQDB::getBlockIds(whereClause, allBlocks);
    if (result != BGQDB::OK)
        {
            LOG_ERROR_MSG("getBlockIds() failed with error " << result);
            return false;
        }
    BOOST_FOREACH(std::string& curr_block, allBlocks) {
        // get the current block state
        BGQDB::BLOCK_STATUS blockState;
        if ((result = BGQDB::getBlockStatus(curr_block, blockState)) != BGQDB::OK)
            {
                LOG_ERROR_MSG("getBlockStatus(" << curr_block << ") failed with error " << result);
                continue;
            }

        // if the block is not fully initialized, remove it from the reconnectBlocks list
        if (blockState != BGQDB::FREE) {
            LOG_DEBUG_MSG("Found at least one block is active.");
            return true;
        }
    }
    return false;
}

int
main(int argc, char **argv)
{
    std::string basename = boost::filesystem::basename( boost::filesystem::path(argv[0]) );
    ostringstream version;
    version << bgq::utility::DriverName << " (revision " << bgq::utility::Revision << ")";
    version << " " << __DATE__ << " " << __TIME__;
    MMCSProperties::setProperty(MMCS_VERSION, version.str());
    MMCSProperties::setProperty(MMCS_PROCESS, basename);// do this before parsing command line parameters

    // Parse command line parameters
    MMCSServerParms mmcsParms(argc, argv);

    // log args
    std::ostringstream args;
    std::copy(argv, argv + argc, std::ostream_iterator<char*>(args, " "));
    LOG_INFO_MSG("MMCS[" << getpid() << "]: starting: " << MMCSProperties::getProperty(MMCS_VERSION));
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
        int rc = RAND_load_file( "/dev/urandom", 16 );
        if ( rc != 16 ) {
            LOG_WARN_MSG("Could not seed random number generator.  Security may be compromised.");
        }
    }


    // Initialize the RasEventHandlerChain
    RasEventHandlerChain::initChain();

    // Initialize the database for the process
    BGQDB::init( MMCSProperties::getProperties(), "mmcs" );

    // initialize performance counters
    bgq::utility::performance::init( MMCSProperties::getProperties() );

    // create the list of mmcs_server commands
    MMCSCommandAttributes attr;  attr.mmcsServerCommand(true);
    MMCSCommandAttributes mask;  mask.mmcsServerCommand(true);
    mmcsCommands = MMCSCommandProcessor::createCommandMap(attr, mask);

    // Create the command processor
    commandProcessor = new MMCSServerCommandProcessor(mmcsCommands);

    // Keep track of client socket connections
    clientSockets = new MMCSSocketList;

    // Keep track of locations being polled for environmentals
    locThreads = new LocationThreadList;

    // Monitor the console port for incoming connections
    try {
        bgq::utility::PortConfiguration::Pairs portpairs;
        std::string servname = "32031";
        bgq::utility::ServerPortConfiguration port_config(servname);
        port_config.setProperties( MMCSProperties::getProperties(), "mmcs");
        port_config.notifyComplete();
        if(mmcsParms._listeners_set == false) {
            // Using the config file
            portpairs = port_config.getPairs();
        } else {
            // We got a command line parameter, so use that instead.
            std::string portstr = MMCSProperties::getProperty(LISTEN_PORTS);
            bgq::utility::PortConfiguration::parsePortsStr(portstr, servname, portpairs);
        }
        consolePort = new MMCSConsolePortServer(portpairs);
        LOG_INFO_MSG("MMCS: console port open.");// at " << MMCSProperties::getProperty(MMCS_SERVER_IP) << ":" << MMCSProperties::getProperty(MMCS_SERVER_PORT));
    }
    catch (const MMCSConsolePort::Error& e)
    {
        if (e.errcode == EADDRINUSE)
        {
            LOG_ERROR_MSG("MMCS: console port " << MMCSProperties::getProperty(MMCS_SERVER_PORT) << " is already in use, there may be another mmcs_server running");
        }
        else
        {
            LOG_ERROR_MSG("MMCS: MMCSConsolePort error: " << e.what());
        }
        exit(1);
    }
    catch (std::runtime_error& e) {
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

    for (vector<string>::iterator i(invalid_memory_locations.begin());
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
    for (int i = 0; i < num_signals; i++)
    {
        struct sigaction action;
        action.sa_sigaction = &mmcs_signal_handler;
        action.sa_flags = SA_SIGINFO;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0)
        {
            LOG_ERROR_MSG("MMCS signal: " << strerror(errno));
            exit(1);
        }
    }

    // Initialize IO usage statics.
    BGQDB::ioUsage::init();

    StringTokenizer bringup_options;
    string bringupString;
    MMCSCommandReply reply;

    // get the bringup options from bg.properties
    bringupString = MMCSProperties::getProperty(BRINGUP_OPTS);
    bringup_options.tokenize(bringupString, ",");
    // add bringup options from command line
    for ( StringTokenizer::iterator it = mmcsParms._bringup_options.begin(); it != mmcsParms._bringup_options.end(); ++it)
        {
            if (find(bringup_options.begin(), bringup_options.end(), *it) == bringup_options.end())
                {
                    bringup_options.push_back(*it);
                }
        }


    bool blocks_are_booted = areBlocksActive();

    // Start the runjob connection
    if(MMCSProperties::getProperty(RUNJOB_CONNECT) == "true") {
        LOG_INFO_MSG("Starting runjob_server monitor.");
        runjob_connection = new RunJobConnection;
        runjob_mon = new RunJobConnectionMonitor;
        runjob_mon->start();
        runjob_mon->_runjob_start_barrier.wait();
    }

    // Initialize mcServer.  This has to come after the master monitor starts and
    // before reconnect happens.  It isn't threaded, so there's no race condition
    // with reconnectBlocks().
    if (MMCSProperties::getProperty(POLL_DB) == "false" && MMCSProperties::getProperty(POLL_ENVS) == "false")
    {
        // skip the mcserver init if we are a --test server
        test_mode = true;
    }
    else
    {
        mcServerInit(bringup_options, reply, blocks_are_booted);
        if (reply.getStatus() != 0)
        {
            LOG_ERROR_MSG("mcserver initialization failed: " << reply.str());
            exit(1);
        }
    }

    // Start the master monitor before reconnecting.
    if (MMCSProperties::getProperty(MASTER_MON) == "true")
    {
        master_mon = new MasterMonitor;
        master_mon->setOptions(bringup_options);
        master_mon->start();

        // Wait for the master monitor to complete its initialization.
        boost::unique_lock<boost::mutex> ulock(MasterMonitor::_startup_lock);
        while(MasterMonitor::_started == false) {
            MasterMonitor::_startup_notifier.wait(ulock);
        }
    } else {
        // we can't reconnect blocks when master_mon is disabled
        LOG_DEBUG_MSG( "disabling block reconnection because master_mon = false" );
        MMCSProperties::setProperty(RECONNECT_BLOCKS, "false");
    }

    // The MMCS server that is running the db monitor thread is the only one that
    // will create permanent target sets or perform reconnect_blocks
    if (MMCSProperties::getProperty(POLL_DB) == "true")
    {
        // set default target set type
        MMCSProperties::setProperty(DFT_TGTSET_TYPE, "targetset=perm");

        // Reconnect or free blocks and mcserver targets
        try {
            MMCSCommandReply reply;
            reconnectBlocks(&mmcsParms, reply, commandProcessor);
        } catch ( const std::exception& e ) {
            LOG_FATAL_MSG( "reconnect blocks: " << e.what() );
            exit( EXIT_FAILURE );
        }

        // Start the database monitor thread
        databaseMonitor = new DatabaseMonitorThread;
        databaseMonitor->monitorBlockTable = MMCSProperties::getProperty(POLL_DB) == "true"? true:false;
        databaseMonitor->start();
    }
    else
    {
        // set default target set type
        MMCSProperties::setProperty(DFT_TGTSET_TYPE, "targetset=temp");
        //        MMCSProperties::setProperty(DFT_TGTSET_TYPE, "targetset=perm");
    }

    if(MMCSProperties::getProperty(POLL_DB) == "true") {
        // Start the environmental monitor threads
        boost::unique_lock<boost::mutex> recon_lock(reconnect_lock);
        while(reconnect_done == false) {
            reconnect_notifier.wait(recon_lock);
        }
    }

    if (MMCSProperties::getProperty(POLL_ENVS) == "true")
    {
        LOG_INFO_MSG("Reconnect done, starting environment monitor.");
        std::vector<std::string> racks;
        int i = 0;
        while(true) {
            std::string subnet = "Name." + boost::lexical_cast<std::string>(i);
            std::string subnet_name = MMCSProperties::getProperty((char*)(subnet.c_str()));
            if(subnet_name.length() == 0)
                break;
            std::string subnet_hw = MMCSProperties::getProperty((char*)(subnet_name.c_str()));
            racks.push_back(subnet_hw);
            ++i;
        }

        std::vector<std::string>  invalidHW;
        result = BGQDB::checkRack(racks, &invalidHW);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG("Error on BGQDB::checkRacks; exiting.");
            exit(1);
        }
        if (!invalidHW.empty()) {
            for (unsigned i=0; i < invalidHW.size(); ++i) {
                LOG_FATAL_MSG("HardwareToManage: rack " << invalidHW[i] << " in properties file, but not found in database");
            }
            LOG_FATAL_MSG("exiting");
            exit(1);
        }
        
        envMonitor = new EnvMonitorThread;
        envMonitor->start();
    }

    // Start the consolePort listener thread
    consoleListener = new MMCSConsoleListener;
    MMCSConsoleMonitorParms* consoleParms = new MMCSConsoleMonitorParms(consolePort, mmcsCommands, clientSockets);
    consoleListener->setArg(consoleParms);
    consoleListener->start();

    LOG_INFO_MSG("MMCS started");

    while (signal_number == 0)
    {
        sleep(5);

        if (test_mode == false && (DefaultControlEventListener::getDefaultControlEventListener()->getBase()->isMailboxStarted() == false))
        {
            LOG_WARN_MSG("MCServer has terminated. MMCS must now terminate.");
            signal_number = EXIT_FAILURE;
            break;
        }
    }

    // end the various mmcs threads
    mmcs_terminate();

    if (signal_number != 0)
    {
        LOG_ERROR_MSG("MMCS[" << getpid() << "]: exiting due to signal " << signal_number);
    }

    exit(signal_number);
}
