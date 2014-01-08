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

#include "Parameters.h"

#include "common/Properties.h"

#include <control/include/mcServer/MCServerPorts.h>

#include <cerrno>
#include <iostream>


using mmcs::common::Properties;


namespace mmcs {
namespace server {


Parameters::Parameters(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    _bringup_options(),
    _listeners_set(false)
{
    std::string property_file;

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
                property_file = argv[c + 1];
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
        Properties::init(property_file);
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
            Properties::setProperty(LISTEN_PORTS, argv[1]);
            _listeners_set = true;
            argv +=2;
            continue;
        }

        if (strcmp(argv[0], "--mcserverip") == 0 && argv[1] != 0)
        {
            Properties::setProperty(MC_SERVER_IP, argv[1]);
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--mcserverport") == 0 && argv[1] != 0)
        {
            // check that port value is valid
            long portNum = strtol(argv[1],NULL,10);
            if (portNum <= 0 || portNum > 65535)
            {
                std::cerr << "invalid mcserverport argument" << std::endl;
                usage();
            }
            else
            {
                Properties::setProperty(MC_SERVER_PORT, argv[1]);
            }
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--iolog") == 0 && argv[1] != 0)
        {
            // check that directory exists and is writable
            if (access(argv[1], F_OK | R_OK | W_OK | X_OK) == -1)
            {
                std::cerr << "invalid iolog directory: " << strerror(errno) << std::endl;
                usage();
            }
            Properties::setProperty(MMCS_LOGDIR, argv[1]);
            argv += 2;
            continue;
        }

        if (strcmp(argv[0], "--shutdown-timeout") == 0 && argv[1] != 0)
        {
            char* endstr;

            // check that timeout value is valid
            if (strtol(argv[1],&endstr,10) == 0 && (strlen(endstr) != 0 && strstr(argv[1], endstr) != 0))
            {
                std::cerr << "invalid shutdown timeout argument" << std::endl;
                usage();
            }
            else
            {
                Properties::setProperty(SHUTDOWN_TIMEOUT, std::string(argv[1]));
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
                std::cerr << "Invalid wait boot timeout value" << std::endl;
                usage();
            }
            else
            {
                Properties::setProperty(WAIT_BOOT_FREE_TIME, std::string(argv[1]));
            }

            argv+=2;
            continue;
        }

        std::cerr << Properties::getProperty(MMCS_PROCESS) << ": unrecognized option: " << argv[0] << std::endl;
        usage();
    }
}

void
Parameters::usage(
        int exit_status
        )
{
    std::cerr << "usage:" << Properties::getProperty(MMCS_PROCESS) << " [options]\n";
    std::cerr << "options:\n";
    std::cerr << "         --listen-ports <host:port,host:port> Comma separated list of host:port pairs on which to listen.  Defaults to localhost:32031\n";
    std::cerr << "         --properties <file>                  Configuration properties, defaults to /bgsys/local/etc/bg.properties\n";
    std::cerr << "         --help [-h]                          Provides this extended help information\n";
    std::cerr << "         --iolog <directory>                  Log output from I/O nodes to the specified directory\n";
    std::cerr << "         --mcserverip <ip address>            TCP/IP address for connecting to mcServer, defaults to 127.0.0.1\n";
    std::cerr << "         --mcserverport <portnumber>          TCP/IP port for connecting to mcServer, defaults to " << CLIENT_PORT_STR "\n";
    std::cerr << "         --shutdown-timeout <seconds>         Number of seconds to wait for block to shut down\n";
    std::cerr << "         --bringup-option <option>            Hardware bringup options. May be specified multiple times\n";
    std::cerr << "         --boot-timeout-and-free              Default number of minutes to wait for a boot to complete before freeing the block\n";
    std::cerr << std::endl;
    exit( exit_status );
}

} } // namespace mmcs::server
