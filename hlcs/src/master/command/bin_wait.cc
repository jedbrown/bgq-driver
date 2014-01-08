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



LOG_DECLARE_FILE( "master" );

void
doWaitBin(
        BGMasterClient& client,
        const std::string& target
        )
{
    const BinaryId bid(target);
    int rc = client.wait_for_terminate(bid);
    if (rc < 0) {
        std::cerr << "Specified binary " << target << " not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Exit status=" << rc << std::endl;
}

void
help()
{
    std::cerr << "Connects to bgmaster_server and waits for a specific" << std::endl;
    std::cerr << "binary id to complete execution." << std::endl;
}

void
usage()
{
    std::cerr << "binary_wait [ binary id ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    validargs.push_back("*"); // One argument without a "--" is allowed
    const Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;

    try {
        client.connectMaster(largs.get_props(), largs.get_portpairs());
    }
    catch (exceptions::BGMasterError& e) {
        std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (largs.size() != 0) {
        doWaitBin(client, *largs.begin());
    } else {
        usage();
        exit(EXIT_FAILURE);
    }
}
