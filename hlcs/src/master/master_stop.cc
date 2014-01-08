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
#include "ArgParse.h"
#include "BGMasterClientApi.h"
#include "BGMasterExceptions.h"

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;

enum TypeToStop { BGMASTER_ONLY, BINARY, BGMASTER, BGAGENT, ALIAS, BINARIES };

void doStop( std::string& target, TypeToStop stoptype = ALIAS, int signal = 15 ) {
    BinaryId id(target);
    std::string errormsg;
    switch(stoptype) {
    case BGMASTER_ONLY:
        { // scope braces for variables local to this case.
            // Kill and restart bgmaster_server without affecting the agents
            // or managed binaries.
            // Connect to bgmaster_server.  Use the master_only flag.  Wait a bit,
            // THEN send the SIGKILL if it doesn't respond.
            std::string start_time;
            std::string version;
            int pid_of_master = client.master_status(start_time, version);
            client.end_master(true);
            int timeout = 4; // seconds to wait for it to die
            bool alive = true;
            while(timeout > 0 && alive) {
                int rc = kill(pid_of_master, 0); // check for existence
                if(rc < 0) {
                    alive = false; // process is done
                }
            }

            if(alive) {
                int rc = kill(pid_of_master, SIGKILL);
                if(rc != 0) {
                    std::cerr << "Could not kill bgmaster_server: " << strerror(errno) << std::endl;
                }
            }
            break;
        }
    case ALIAS:
        try {
            client.stop(target, signal, errormsg);
        } catch(BGMasterExceptions::BGMasterError& e) {
            std::cerr << "stop failed: " << e.what() << std::endl;
            return;
        }
        break;
    case BINARY:
        try {
            client.stop(id, signal, errormsg);
        } catch(BGMasterExceptions::BGMasterError& e) {
            std::cerr << "stop failed: " << e.what() << std::endl;
            return;
        }
        break;
    case BGAGENT:
        try {
            LOG_DEBUG_MSG("Stopping bgagentd " << target);
            BGAgentId agent_to_die(target);
            client.end_agent(agent_to_die);

        } catch(BGMasterExceptions::BGMasterError& e) {
            std::cerr << "agent stop failed: " << errormsg << std::endl;
            return;
        }
        if(stoptype == BGAGENT)
            break;  // If it's just the agent, then break.  Otherwise, fall through.
    default:
        // Default behavior is BGMASTER and BINARIES
    case BGMASTER:
        try {
            LOG_DEBUG_MSG("Stopping bgmaster_server");
            client.end_master(false, signal);
        } catch(BGMasterExceptions::BGMasterError& e) {
            std::cerr << "master stop failed: " << e.what() << std::endl;
            return;
        }
        break;
        // We don't bother to fall through to BINARIES because
        // BGMASTER already takes care of them anyway.
    case BINARIES:
        BinaryId emptyid;
        // An empty id says kill 'em all.
        client.stop(emptyid, signal, errormsg);
        break;
    }
    std::cout << "stopped " << target << std::endl;
}

void help() {
    std::cerr << "Stop any or all controlled processes, bgmaster_server, " << std::endl;
    std::cerr << "any or all bgagentd processes, or everything.  By default, it" << std::endl;
    std::cerr << "stops bgmaster_server and all managed binaries." << std::endl;
    std::cerr << "Processes are allowed an orderly completion with a timeout" << std::endl;
    std::cerr << "before they are forced to end.  bgmaster_server is only forced "<< std::endl;
    std::cerr << "with bgmaster_only option." << std::endl;
    std::cerr << "The signal number argument provides an initial signal to send. " << std::endl;
    std::cerr << "By default the initial signal is SIGTERM. " << std::endl;
    std::cerr << "Administrative authority required." << std::endl;
}

void usage() {
    std::cerr << "master_stop [ alias ] | [ \"bgmaster\" ] | [ \"binaries\" ] | [ \"bgmaster_only\" ] "
              << "[ --binary binary id ] | [ --agent agent id ] [ --signal signal number ] [ --properties filename ] "
              << "[ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int stringSigToIntSig(std::string& strsig) {
    int intsig = 0;
    if(strsig == "SIGHUP")
        intsig = SIGHUP;
    else if(strsig == "SIGINT")
        intsig = SIGINT;
    else if(strsig == "SIGQUIT")
        intsig = SIGQUIT;
    else if(strsig == "SIGILL")
        intsig = SIGILL;
    else if(strsig == "SIGABRT")
        intsig = SIGABRT;
    else if(strsig == "SIGFPE")
        intsig = SIGFPE;
    else if(strsig == "SIGKILL")
        intsig = SIGKILL;
    else if(strsig == "SIGSEGV")
        intsig = SIGSEGV;
    else if(strsig == "SIGPIPE")
        intsig = SIGPIPE;
    else if(strsig == "SIGALRM")
        intsig = SIGALRM;
    else if(strsig == "SIGALRM")
        intsig = SIGALRM;
    else if(strsig == "SIGTERM")
        intsig = SIGTERM;
    else if(strsig == "SIGUSR1")
        intsig = SIGUSR1;
    else if(strsig == "SIGUSR2")
        intsig = SIGUSR2;
    else if(strsig == "SIGCHLD")
        intsig = SIGCHLD;
    else if(strsig == "SIGCONT")
        intsig = SIGCONT;
    else if(strsig == "SIGSTOP")
        intsig = SIGSTOP;
    else if(strsig == "SIGTSTP")
        intsig = SIGTSTP;
    else if(strsig == "SIGTTIN")
        intsig = SIGTTIN;
    else if(strsig == "SIGCONT")
        intsig = SIGCONT;
    else if(strsig == "SIGTTOU")
        intsig = SIGTTOU;
    else if(strsig == "SIGBUS")
        intsig = SIGBUS;
    else if(strsig == "SIGPOLL")
        intsig = SIGPOLL;
    else if(strsig == "SIGIO")
        intsig = SIGIO;
    else if(strsig == "SIGPROF")
        intsig = SIGPROF;
    else if(strsig == "SIGSYS")
        intsig = SIGSYS;
    else if(strsig == "SIGTRAP")
        intsig = SIGTRAP;
    else if(strsig == "SIGURG")
        intsig = SIGURG;
    else if(strsig == "SIGVTALRM")
        intsig = SIGVTALRM;
    else if(strsig == "SIGXCPU")
        intsig = SIGXCPU;
    else if(strsig == "SIGXFSZ")
        intsig = SIGXFSZ;
    else if(strsig == "SIGIOT")
        intsig = SIGIOT;
    else if(strsig == "SIGSTKFLT")
        intsig = SIGSTKFLT;
    else if(strsig == "SIGCLD")
        intsig = SIGCLD;
    else if(strsig == "SIGPWR")
        intsig = SIGPWR;
    else if(strsig == "SIGWINCH")
        intsig = SIGWINCH;
    else if(strsig == "SIGUNUSED")
        intsig = SIGUNUSED;
    else if(strsig == "SIGPWR")
        intsig = SIGPWR;
    else {
        try {
            intsig = boost::lexical_cast<int>(strsig);
        } catch(boost::bad_lexical_cast& e) {
            std::cerr << "Invalid signal \"" << strsig << "\" specified." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return intsig;
}

int main(int argc, const char** argv) {

    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    std::string binarg = "--binary";
    std::string agarg = "--agent";
    std::string sigarg = "--signal";

    validargs.push_back("*"); // One argument without a "--" is allowed
    validargs.push_back(binarg);
    validargs.push_back(agarg);
    validargs.push_back(sigarg);

    Args largs(argc, argv, &usage, &help, validargs, singles);
    pargs = &largs;
    client.initProperties(pargs->_props);

    std::string binary = (*pargs)[binarg];
    std::string agent = (*pargs)[agarg];
    std::string sig = (*pargs)[sigarg];
    int signal = 0;
    std::string target = "";
    TypeToStop stoptype;

    if(!sig.empty())
        signal = stringSigToIntSig(sig);
    if(!agent.empty()) {
        target = agent;
        stoptype = BGAGENT;
    }
    else if(!binary.empty()) {
        target = binary;
        stoptype = BINARY;
    } else {
        if(pargs->size() != 0) {
            target = *(pargs->begin());
        } else {
            // No arguments, no valid command.
            usage();
            exit(EXIT_FAILURE);
        }
        if(target == "bgmaster")
            stoptype = BGMASTER;
        else if(target == "binaries") {
            target = "";
            stoptype = BINARY;
        }
        else if(target == "bgmaster_only")
            stoptype = BGMASTER_ONLY;
        else
            stoptype = ALIAS;
    }
    
    try {
        client.connectMaster(pargs->get_portpairs());
    }
    catch(BGMasterExceptions::BGMasterError& e) {
        LOG_FATAL_MSG("Unable to contact bgmaster_server to stop. " << e.what());
        exit(EXIT_FAILURE);
    }

    doStop(target, stoptype, signal);
}
