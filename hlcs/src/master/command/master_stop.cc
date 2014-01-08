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

#include "common/ArgParse.h"

#include "lib/BGMasterClient.h"
#include "lib/exceptions.h"

#include <utility/include/Log.h>


#include <csignal>

LOG_DECLARE_FILE( "master" );


enum TypeToStop { BGMASTER_ONLY, BINARY, BGMASTER, BGAGENT, ALIAS, BINARIES };

int
doStop(
        BGMasterClient& client,
        const std::string& target,
        const TypeToStop stoptype = ALIAS,
        const int signal = 15
        )
{
    BinaryId id(target);
    std::string errormsg;
    switch(stoptype) {
    case BGMASTER_ONLY:
        try {
            client.end_master(true);
            errormsg = "Stopped " + target;
        } catch (const exceptions::BGMasterError& e) {
            std::cerr << "end master failed, error is: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        break;
    case ALIAS:
        try {
            client.stop(target, signal, errormsg);
        } catch (const exceptions::BGMasterError& e) {
            std::cerr << "Stop alias failed, error is: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        break;
    case BINARY:
        try {
            client.stop(id, signal, errormsg);
        } catch (const exceptions::BGMasterError& e) {
            std::cerr << "Stop binary failed, error is: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        break;
    case BGAGENT:
        std::cerr << "\n\nStopping bgagent via master_stop is no longer supported. " << std::endl;
	std::cerr << "\tUse '/etc/init.d/bgagent stop' to stop bgagentd. \n" << std::endl; 
        return EXIT_FAILURE;
        if (stoptype == BGAGENT)
            break;  // If it's just the agent, then break.  Otherwise, fall through.
    default:
        // Default behavior is BGMASTER and BINARIES
    case BGMASTER:
        try {
	    errormsg = "Stopping bgmaster (bgmaster_server)";
            client.end_master(false, signal);
        } catch (const exceptions::BGMasterError& e) {
            std::cerr << "Stopping bgmaster (bgmaster_server) failed, error is: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        break;
        // We don't bother to fall through to BINARIES because
        // BGMASTER already takes care of them anyway.
    case BINARIES:
        try {
            const BinaryId emptyid;
            // An empty id says kill 'em all.
            client.stop(emptyid, signal, errormsg);
        } catch (const exceptions::BGMasterError& e) {
            std::cerr << "Stopping binaries failed, error is: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        break;
    }

    std::cout << errormsg << std::endl;

    return 0;
}

void
help()
{
    std::cerr << "Stop any or all controlled processes and bgmaster_server. " << std::endl;
    std::cerr << "By default, it stops bgmaster_server and all managed binaries." << std::endl;
    std::cerr << "Processes are allowed an orderly completion" << std::endl;
    std::cerr << "with a timeout before they are forced to end."<< std::endl;
    std::cerr << "The signal number argument provides an initial signal" << std::endl;
    std::cerr << "to send. By default the initial signal is SIGTERM." << std::endl;
    std::cerr << "Administrative authority required." << std::endl;
}

void
usage()
{
    std::cerr << "master_stop [ alias ] | [ \"bgmaster\" ] | [ \"binaries\" ] | [ \"bgmaster_only\" ] "
              << "[ --binary binary id ] | [ --signal signal number ] [ --properties filename ] "
              << "[ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int
stringSigToIntSig(
        const std::string& strsig
        )
{
    int intsig = 0;
    if (strsig == "SIGHUP")
        intsig = SIGHUP;
    else if (strsig == "SIGINT")
        intsig = SIGINT;
    else if (strsig == "SIGQUIT")
        intsig = SIGQUIT;
    else if (strsig == "SIGILL")
        intsig = SIGILL;
    else if (strsig == "SIGABRT")
        intsig = SIGABRT;
    else if (strsig == "SIGFPE")
        intsig = SIGFPE;
    else if (strsig == "SIGKILL")
        intsig = SIGKILL;
    else if (strsig == "SIGSEGV")
        intsig = SIGSEGV;
    else if (strsig == "SIGPIPE")
        intsig = SIGPIPE;
    else if (strsig == "SIGALRM")
        intsig = SIGALRM;
    else if (strsig == "SIGTERM")
        intsig = SIGTERM;
    else if (strsig == "SIGUSR1")
        intsig = SIGUSR1;
    else if (strsig == "SIGUSR2")
        intsig = SIGUSR2;
    else if (strsig == "SIGCHLD")
        intsig = SIGCHLD;
    else if (strsig == "SIGCONT")
        intsig = SIGCONT;
    else if (strsig == "SIGSTOP")
        intsig = SIGSTOP;
    else if (strsig == "SIGTSTP")
        intsig = SIGTSTP;
    else if (strsig == "SIGTTIN")
        intsig = SIGTTIN;
    else if (strsig == "SIGTTOU")
        intsig = SIGTTOU;
    else if (strsig == "SIGBUS")
        intsig = SIGBUS;
    else if (strsig == "SIGPOLL")
        intsig = SIGPOLL;
    else if (strsig == "SIGIO")
        intsig = SIGIO;
    else if (strsig == "SIGPROF")
        intsig = SIGPROF;
    else if (strsig == "SIGSYS")
        intsig = SIGSYS;
    else if (strsig == "SIGTRAP")
        intsig = SIGTRAP;
    else if (strsig == "SIGURG")
        intsig = SIGURG;
    else if (strsig == "SIGVTALRM")
        intsig = SIGVTALRM;
    else if (strsig == "SIGXCPU")
        intsig = SIGXCPU;
    else if (strsig == "SIGXFSZ")
        intsig = SIGXFSZ;
    else if (strsig == "SIGIOT")
        intsig = SIGIOT;
    else if (strsig == "SIGSTKFLT")
        intsig = SIGSTKFLT;
    else if (strsig == "SIGCLD")
        intsig = SIGCLD;
    else if (strsig == "SIGPWR")
        intsig = SIGPWR;
    else if (strsig == "SIGWINCH")
        intsig = SIGWINCH;
    else if (strsig == "SIGUNUSED")
        intsig = SIGUNUSED;
    else {
        try {
            intsig = boost::lexical_cast<int>(strsig);
        } catch (const boost::bad_lexical_cast& e) {
            std::cerr << "Invalid signal \"" << strsig << "\" specified." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return intsig;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    std::string binarg = "--binary";
    std::string agarg = "--agent";
    std::string sigarg = "--signal";

    validargs.push_back("*"); // One argument without a "--" is allowed
    validargs.push_back(binarg);
    validargs.push_back(agarg);
    validargs.push_back(sigarg);

    const Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;

    const std::string binary = largs[binarg];
    const std::string agent = largs[agarg];
    const std::string sig = largs[sigarg];
    int signal = 0;
    std::string target;
    TypeToStop stoptype;

    if (!sig.empty())
        signal = stringSigToIntSig(sig);
    if (!agent.empty()) {
        target = agent;
        stoptype = BGAGENT;
    } else if (!binary.empty()) {
        target = binary;
        stoptype = BINARY;
    } else {
        if (largs.size() != 0) {
            target = *(largs.begin());
        } else {
            // No arguments, no valid command.
            usage();
            exit(EXIT_FAILURE);
        }
        if (target == "bgmaster")
            stoptype = BGMASTER;
        else if (target == "binaries") {
            target.clear();
            stoptype = BINARY;
        }
        else if (target == "bgmaster_only")
            stoptype = BGMASTER_ONLY;
        else
            stoptype = ALIAS;
    }

    try {
        client.connectMaster(largs.get_props(), largs.get_portpairs());
    } catch (const exceptions::BGMasterError& e) {
        std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    const int rc = doStop(client, target, stoptype, signal);
    exit( rc );
}
