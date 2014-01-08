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
#include "lib/ListAgents.h"

#include <utility/include/Log.h>

#include <iostream>

LOG_DECLARE_FILE( "master" );

void
help()
{
    std::cerr << "Returns a list of ids for active agents and associated running binaries." << std::endl;
}

void
usage()
{
    std::cerr << "list_agents [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    std::string fancyarg = "--fancy"; // hidden for backwards compatibility
    std::string uglyarg = "--normal";
    singles.push_back(fancyarg);
    singles.push_back(uglyarg);
    const Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;

    // assume fancy by default
    bool fancy = true;
    if (largs.find_arg(uglyarg)) {
        fancy = false;
    }

    try {
        client.connectMaster(largs.get_props(), largs.get_portpairs());
    }
    catch ( const exceptions::BGMasterError& e ) {
        std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    ListAgents::doListAgents(client, true, fancy);
}
