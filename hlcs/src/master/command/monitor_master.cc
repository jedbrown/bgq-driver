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

static bool monitor_ending = false;
static int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGUSR1, SIGTERM, SIGPIPE, SIGXFSZ, SIGABRT,
                         SIGSEGV, SIGILL, SIGFPE, SIGTERM, SIGBUS };
static int num_signals = sizeof(signals) / sizeof(signals[0]);
static bool sig_caught = false;

void
sig_handler(
        int signum
        )
{
    if (signum != SIGPIPE && sig_caught == false) {
        sig_caught = true; // Don't keep trying to do stuff if we fail.
        monitor_ending = true;
    } else {
        // Don't end on a SIGPIPE.  It means master ended, but we want to retry.
    }
}

void
help()
{
    std::cerr << "Continuously monitor bgmaster_server for events and errors." << std::endl;
}

void
usage()
{
    std::cerr << "monitor_master [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

void
doMonitor(
        BGMasterClient& client
        )
{
    try {
        client.event_monitor();
    } catch (const exceptions::BGMasterError& e) {
        std::cerr << e.what() << std::endl;
    }
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    const Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;

    // Signal handlers
    for (int i = 0; i < num_signals; i++) {
        struct sigaction action;
        action.sa_handler = &sig_handler;
        action.sa_flags = 0;
        int rc = sigaction(signals[i], &action, 0);
        if (rc < 0) {
            std::cerr << "Error setting up monitor_master signal handler: " << strerror(errno) << std::endl;
            exit(1);
        }
    }

    unsigned int count = 1;
    while (!monitor_ending) {
        // We retry every second, but only update the user every minute.
        try {
            client.connectMaster(largs.get_props(), largs.get_portpairs());
        } catch (const exceptions::CommunicationError& e) {
            if (count % 60 == 0 || count == 1) {
                std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
                std::cerr << "Retrying..." << std::endl;
            }
            ++count;
            sleep(1);
            continue;
        }
        count = 1;
        doMonitor( client );
    }
    std::cout << "Ending bgmaster monitor." << std::endl;
}
