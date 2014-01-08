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
#include <boost/tokenizer.hpp>
#include "BGMasterClientApi.h"
#include "BGMasterExceptions.h"

LOG_DECLARE_FILE( "master" );

enum Command { status, start, stop, failover, list_agents, list_clients, exit_status, wait_bin, end_agent, end_master, reload };

bgq::utility::Properties::Ptr props;

BGMasterClient client;

pid_t startMaster(std::string& path, std::string& args)
{

    // fork a new process
    pid_t child = fork();

    // fork failed
    if (child < 0)
    {
	LOG_ERROR_MSG("fork: " << strerror(errno));
	return 0;
    }

    //
    // Parent process
    //
    if (child > 0)
	return child;

    //
    // Child process
    //
    LOG_INFO_MSG(path << " [" << getpid() << "]: starting ");

    // start a new process group
    pid_t my_pgid = setpgid(0,0);
    if (my_pgid < 0)
    {
        LOG_ERROR_MSG( "setpgrp: " << strerror(errno));
	exit(-1);
    }

    int default_signals[] = { SIGCHLD, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE, SIGABRT };
    unsigned num_default_signals = sizeof(default_signals) / sizeof(default_signals[0]);
    int ignore_signals[] = { SIGALRM, SIGHUP };
    unsigned num_ignore_signals = sizeof(ignore_signals) / sizeof(ignore_signals[0]);

    // restore signal handlers to defaults
    for (unsigned i = 0; i < num_default_signals; ++i)
	signal(default_signals[i], SIG_DFL);

    // ignore SIGALRM, SIGHUP
    for (unsigned i = 0; i < num_ignore_signals; ++i)
	signal(ignore_signals[i], SIG_IGN);

    LOG_INFO_MSG("executing: " << path << args);
    std::string command = path + " " + args;

    // We at least have to check it first
    system(command.c_str());
    return 0;
}

void doStatus(std::string& target) {
    std::map<BinaryId, BinaryController::Status, Id::Comp> mm;
    client.status(mm);
    for(std::map<BinaryId, BinaryController::Status, Id::Comp>::iterator it = mm.begin();
        it != mm.end(); ++it) {
        BinaryId id = it->first;
        std::cout << id.str() << ":" << BinaryController::status_str(it->second) << std::endl;
    }
}

void doStart(std::string& target, std::string location="") {
    std::string commandstring = props->getValue("master.binmap", target);

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ");
    tokenizer tok(commandstring, sep);


    int i = 0;
    std::string path="";
    std::string arguments="";
    for(tokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg){
        if(i == 0)
            path = (*beg);
        else {
            arguments += (*beg);
            arguments += " ";
        }
        ++i;
    }

    std::string st;

    if(target == "bgmaster") {// bgmaster_server

        // Get our host name
        char hostname[HOST_NAME_MAX];
        if(gethostname(hostname, sizeof(hostname)) < 0) {
            LOG_FATAL_MSG("Host name error: " << strerror(errno));
        }

        // Figure out the date and time to make it part of the filename
        time_t rawtime;
        struct tm * timeinfo;
        char date [80];

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        strftime (date,80,"%Y-%m%d-%H:%M:%S",timeinfo);

        // Build the log file path
        std::string filearg;
        filearg += " > " + props->getValue("master", "logdir") +
            "/" + hostname + "-bgmaster-" + date + ".log";
        arguments += filearg;
        startMaster(path, arguments);
    } else {

        LOG_DEBUG_MSG("Sending start message " << path << " " << arguments
                      << " for " << target);
        BGAgentId id(location);

        BinaryId started_id = client.start(target, &id);
        st = started_id;
    }

    std::cout << "started " << target << ":" << st << std::endl;
}

void doStop( std::string& target) {
    BinaryId id(target);
    std::string errormsg;
    if(target.find(":") != std::string::npos) {
        try {
            client.stop(id, 15, errormsg);
        } catch(BGMasterExceptions::BGMasterError& e) {
            std::cerr << "stop failed: " << errormsg << std::endl;
        }
    } else {
        try {
            client.stop(target, 15, errormsg);
        } catch(BGMasterExceptions::BGMasterError& e) {
            std::cerr << "stop failed: " << errormsg << std::endl;
        }
    }
    std::cout << "stopped " << target << std::endl;
}

void doFailover( std::string& target) {

}

void doReload(std::string& config_file) {
    client.reload_config(config_file);
}

void doListAgents() {
    std::map<BGAgentId, std::vector<BinaryControllerPtr>, Id::Comp > agentmap;
    client.get_agents(agentmap);

    LOG_DEBUG_MSG("Sending list_agents command");

    // Loop through the map and print the agents and associated binaries
    for(std::map<BGAgentId, std::vector<BinaryControllerPtr>, Id::Comp >::iterator it = agentmap.begin();
        it != agentmap.end(); ++it) {
        // Get the Id string
        BGAgentId* id = const_cast<BGAgentId*>(&(it->first));
        std::string idstr = *id;

        // Now iterate through the binary vector
        typedef std::vector<BinaryControllerPtr> BinVector;
        BinVector vec = it->second;

        bool foundbin = false;
        for(BinVector::iterator bin_it = vec.begin();
            bin_it != vec.end(); ++bin_it) {
            foundbin = true;

            std::cout << idstr << "::" << (*bin_it)->getBid().str()
                      << "::" << BinaryController::status_str((*bin_it)->getStatus()) << std::endl;
        }
        if(!foundbin)
            std::cout << idstr << std::endl;
    }
}

void doListClients() {
}

void doExitStatus( std::string& target) {

    std::map<BGAgentId, BinaryControllerPtr, Id::Comp> emap;

    BGAgentId id;
    BinaryControllerPtr ptr;
    if(target != "") {
        // If we have a specific agent, we'll put it in the list
        id = target;
        emap[id] = ptr;
    }

    client.exit_status(emap);

    for(std::map<BGAgentId, BinaryControllerPtr, Id::Comp>::iterator mapit = emap.begin();
        mapit != emap.end(); ++mapit) {
        BGAgentId baid = mapit->first;
        std::cout << baid.str() << "::"
                  << (*mapit).second->getBid().str() << ":"
                  << (*mapit).second->getExit() << std::endl;
    }
}

void doWaitBin( std::string& target) {
    BinaryId bid(target);
    int rc = client.wait_for_terminate(bid);
    std::cout << "Exit status=" << rc << std::endl;
}

void doDie( std::string& target) {
    BGAgentId agent_to_die(target);
    client.end_agent(agent_to_die);
}

void doTerminate() {
    client.end_master();
}

int main(unsigned int argc, const char** argv) {
    std::cout << "bgmaster_client [" << getpid() << "] starting..." << std::endl;

    for (unsigned int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--properties") || !strcasecmp(argv[i], "-p")) {
            if (argc == ++i) {
                std::cerr << "please give a file name after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            props = bgq::utility::Properties::create(argv[i]);
        }
    }

    if (!props) {
        props = bgq::utility::Properties::create();
    }
    // Needs to get master location from properties and command line
    std::string port = props->getValue("master", "client_port");
    std::string host = props->getValue("master", "client_host");

    Command cmd;
    std::string target = "";
    std::string location = ""; // agent to execute command
    bool connect = true;

    for (unsigned int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "--serverport") || !strcasecmp(argv[i], "-s")) {
            if (argc == ++i) {
                std::cerr << "please specify a port number after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            port = argv[i];
        }

        else if (!strcasecmp(argv[i], "--serverhost") || !strcasecmp(argv[i], "-h")) {
            if (argc == ++i) {
                std::cerr << "please specify a host after " << argv[i-1] << std::endl;
                exit(EXIT_FAILURE);
            }
            port = argv[i];
        }
        else if (!strcasecmp(argv[i], "status")) {
            cmd = status;
            if(argc != ++i)
                target = argv[i];
            break;
        }
        else if (!strcasecmp(argv[i], "start")) {
            cmd = start;
            if(argc != ++i) {
                target = argv[i];
                if(target == "bgmaster")
                    connect = false;
                size_t pos = target.find("@");
                if(pos != std::string::npos) {
                    // We have a location
                    typedef boost::tokenizer<boost::char_separator<char> > target_tokenizer;
                    boost::char_separator<char> tar_sep("@");
                    target_tokenizer tar_tok(target, tar_sep);
                    int j = 0;
                    for(target_tokenizer::iterator beg = tar_tok.begin();
                        beg != tar_tok.end(); ++beg) {
                        if(j != 0)
                            location = *beg;
                        else
                            target = *beg;
                        ++j;
                    }
                }
            }
            break;
        }
        else if (!strcasecmp(argv[i], "stop")) {
            cmd = stop;
            if(argc != ++i)
                target = argv[i];
            break;
        }
        else if (!strcasecmp(argv[i], "failover")) {
            cmd = failover;
            if(argc != ++i)
                target = argv[i];
            break;
        }
        else if (!strcasecmp(argv[i], "agents")) {
            cmd = list_agents;
            break;
        }
        else if (!strcasecmp(argv[i], "clients")) {
            cmd = list_clients;
            break;
        }
        else if (!strcasecmp(argv[i], "exit_status")) {
            if(argc != ++i)
                target = argv[i];
            cmd = exit_status;
            break;
        }
        else if (!strcasecmp(argv[i], "end_agent")) {
            if(argc != ++i)
                target = argv[i];
            cmd = end_agent;
            break;

        }
        else if (!strcasecmp(argv[i], "end_master")) {
            cmd = end_master;
            break;
        }
        else if (!strcasecmp(argv[i], "reload")) {
            if(argc != ++i)
                target = argv[i];
            cmd = reload;
            break;

        } else {
            std::cerr << "invalid command " << argv[i] << std::endl;
            exit(1);
        }

    }

    if(host.length() == 0) {
        std::cerr << "Host for bgmaster_server not found in properties file and not specified" << std::endl;
        exit(1);
    }
    if(port.length() == 0) {
        std::cerr << "Port for bgmaster_server not found in properties file and not specified" << std::endl;
        exit(1);
    }

    // Initialize properties for everybody
    CxxSockets::setProperties(props);
    bgq::utility::initializeLogging(*props);
    client.init_properties(pargs->_props);
    //    BGMasterClient client;

    if(connect)
        client.connect_master(port, host);
    switch(cmd) {
    case status:
        doStatus( target);
        break;
    case start:
        doStart( target, location);
        break;
    case stop:
        doStop( target);
        break;
    case failover:
        doFailover( target);
        break;
    case list_agents:
        doListAgents();
        break;
    case list_clients:
        doListClients();
        break;
    case exit_status:
        doExitStatus( target);
        break;
    case wait_bin:
        doWaitBin( target);
        break;
    case end_agent:
        doDie( target);
        break;
    case end_master:
        doTerminate();
        break;
    case reload:
        doReload(target);
        break;
    default:
        std::cerr << "Invalid command" << std::endl;
    }
    return 0;
}
