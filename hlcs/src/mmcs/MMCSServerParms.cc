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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

#include "MMCSProperties.h"
#include "MMCSServerParms.h"

#include <control/include/mcServer/MCServerPorts.h>

#include <cerrno>
#include <iostream>

using namespace std;


MMCSServerParms::MMCSServerParms(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    _program_name(*argv),
    _property_file(),
    _bringup_options(),
    _listeners_set(false)
{
    // Loop through the arguments looking for --properties or --help
    for(int c = 1; c < argc;)
    {
        if (
                strcmp(argv[c], "--bgproperties") == 0 ||
                strcmp(argv[c], "--properties") == 0 ||
                strcmp(argv[c], "-p") == 0
           )
        {
            if( argv[c + 1] != 0)
            {
                _property_file = argv[c + 1];
                c += 2;
            }
            else
            {
                std::cerr << "No properties file specified." << std::endl << std::endl;
                usage();
            }
        }
        else if (
                strcmp(argv[c], "--help") == 0 ||
                strcmp(argv[c], "-h") == 0
                )
        {
            usage(EXIT_SUCCESS);
        }
        else
        {
            ++c;
        }
    }

    try
    {
        // Read the properties file for execution options
        MMCSProperties::init(_property_file);
    }
    catch (const std::exception& e)
    {
        std::cerr << "MMCS: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    // parse the command line parameters
    for (++argv; *argv; )
    {
        if ((strcmp(argv[0], "--bgproperties") == 0 || strcmp(argv[0], "--properties") == 0 || strcmp(argv[0], "-p") == 0)
            && argv[1] != 0)
        { // already did this, so skip it.
            argv +=2;
            continue;
        }

        if (strcmp(argv[0], "--listen-ports") == 0 && argv[1] != 0)
        {
            MMCSProperties::setProperty(LISTEN_PORTS, argv[1]);
            _listeners_set = true;
            argv +=2;
            continue;
        }

        if (strcmp(argv[0], "--mcserverip") == 0 && argv[1] != 0)
        {
            MMCSProperties::setProperty(MC_SERVER_IP, argv[1]);
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--mcserverport") == 0 && argv[1] != 0)
        {
            // check that port value is valid
            long portNum = strtol(argv[1],NULL,10);
            if (portNum <= 0 || portNum > 65535)
            {
                cerr << "invalid mcserverport argument" << endl;
                usage();
            }
            else
            {
                MMCSProperties::setProperty(MC_SERVER_PORT, argv[1]);
            }
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--no-poll-db") == 0)
        {
            MMCSProperties::setProperty(POLL_DB, "false");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--no-poll-envs") == 0)
        {
            MMCSProperties::setProperty(POLL_ENVS, "false");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--monitor-master") == 0)
        {
            MMCSProperties::setProperty(MASTER_MON, "true");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--no-reconnect") == 0)
        {
            MMCSProperties::setProperty(RECONNECT_BLOCKS, "false");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--poll-db") == 0)
        {
            MMCSProperties::setProperty(POLL_DB, "true");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--poll-envs") == 0)
        {
            MMCSProperties::setProperty(POLL_ENVS, "true");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--reconnect") == 0)
        {
            MMCSProperties::setProperty(RECONNECT_BLOCKS, "true");
            argv += 1;
            continue;
        }

        if (strcmp(argv[0], "--test") == 0)
        {
            argv += 1;
            MMCSProperties::setProperty(POLL_ENVS, "false");
            MMCSProperties::setProperty(POLL_DB, "false");
            MMCSProperties::setProperty(RUNJOB_CONNECT, "false");
            continue;
        }

        if (strcmp(argv[0], "--secure") == 0)
            {
                if(strcmp(argv[1], "false") == 0) {
                    MMCSProperties::setProperty(SECURE_CONSOLE, "false");
                } else if(strcmp(argv[1], "true") == 0) {
                    MMCSProperties::setProperty(SECURE_CONSOLE, "true");
                } else {
                    cerr << "invalid argument '" << argv[1] << "' for 'secure' parameter" << std::endl;
                    usage();
                }
                argv+=2;
                continue;
            }

        if (strcmp(argv[0], "--iolog") == 0 && argv[1] != 0)
        {
            // check that directory exists and is writable
            if (access(argv[1], F_OK | R_OK | W_OK | X_OK) == -1)
            {
                cerr << "invalid iolog directory: " << strerror(errno) << endl;
                usage();
            }
            MMCSProperties::setProperty(MMCS_LOGDIR, argv[1]);
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--shutdown-timeout") == 0 && argv[1] != 0)
        {
            char* endstr;

            // check that timeout value is valid
            if (strtol(argv[1],&endstr,10) == 0 && (strlen(endstr) != 0 && strstr(argv[1], endstr) != 0))
            {
                cerr << "invalid shutdown timeout argument" << endl;
                usage();
            }
            else
            {
                MMCSProperties::setProperty(SHUTDOWN_TIMEOUT, string(argv[1]));
            }
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--bringup-option") == 0 && argv[1] != 0)
        {
            _bringup_options.push_back(argv[1]);
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--boot-timeout-and-free") == 0 && argv[1] != 0)
        {
            if (strtol(argv[1], 0, 10) < 0)
            {
                cerr << "Invalid wait boot timeout value" << endl;
                usage();
            }
            else
            {
                MMCSProperties::setProperty(WAIT_BOOT_FREE_TIME, string(argv[1]));
            }

            argv+=2;
            continue;
        }

        cerr << MMCSProperties::getProperty(MMCS_PROCESS) << ": unrecognized option: " << argv[0] << endl;
        usage();
    }
}

void
MMCSServerParms::usage(
        int exit_status
        )
{
    cerr << "usage:" << MMCSProperties::getProperty(MMCS_PROCESS) << " [options]\n";
    cerr << "options:\n";
    cerr << "         --listen-ports <host:port,host:port> Comma separated list of host:port pairs on which to listen.  Defaults to localhost:32031\n";
    cerr << "         --properties <file>                  Configuration properties, defaults to /bgsys/local/etc/bg.properties\n";
    cerr << "         --help [-h]                          Provides this extended help information\n";
    cerr << "         --iolog <directory>                  Log output from I/O nodes to the specified directory\n";
    cerr << "         --mcserverip <ip address>            TCP/IP address for connecting to mcServer, defaults to 127.0.0.1\n";
    cerr << "         --mcserverport <portnumber>          TCP/IP port for connecting to mcServer, defaults to " << CLIENT_PORT_STR "\n";
    cerr << "         --no-free-blocks                     Don't mark all blocks as free on startup\n";
    cerr << "         --no-poll-db                         Don't monitor database for allocate/free requests\n";
    cerr << "         --no-poll-envs                       Don't monitor environmental statistics\n";
    cerr << "         --no-reconnect                       Don't reconnect initialized blocks\n";
    cerr << "         --poll-db                            Monitor database for allocate/free requests\n";
    cerr << "         --poll-envs                          Monitor environmental statistics\n";
    cerr << "         --reconnect                          Reconnect initialized blocks\n";
    cerr << "         --shutdown-timeout <seconds>         Number of seconds to wait for block to shut down\n";
    cerr << "         --bringup-option <option>            Hardware bringup options. May be specified multiple times\n";
    cerr << "         --test                               Combination of --no-poll-db, --no-poll-envs, and --no-reconnect. Used with --listen-ports\n";
    cerr << "         --boot-timeout-and-free              Default number of minutes to wait for a boot to complete before freeing the block\n";
    cerr << "         --monitor-master                     Monitor BGmaster for Subnet_mc failures\n";
    cerr << endl;
    exit( exit_status );
}
