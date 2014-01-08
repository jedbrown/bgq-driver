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

#include "lib/BGMasterClientApi.h"
#include "lib/exceptions.h"

#include <utility/include/Log.h>

#include <boost/tokenizer.hpp>

#include <csignal>

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;

void
doListClients()
{
    BGMasterClient::ClientAndUserMap clients;
    client.get_clients(clients);

    // Loop through the map and print the clients and associated binaries
    for (BGMasterClient::ClientAndUserMap::iterator it = clients.begin();
        it != clients.end(); ++it) {
        // Get the Id string
        std::string idstr = it->first.str();
        BGMasterClient::ClientUID uid = it->second;
        std::cout << idstr << "|" << uid << std::endl;
    }
}

void
help()
{
    std::cerr << "Returns a list of ids for active clients." << std::endl;
}

void
usage()
{
    std::cerr << "list_clients [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    Args largs(argc, argv, &usage, &help, validargs, singles);
    pargs = &largs;
    client.initProperties(pargs->get_props());

    try {
        client.connectMaster(pargs->get_portpairs());
    }
    catch(exceptions::BGMasterError& e) {
        std::cerr << "Unable to contact bgmaster_server, server may be down." << std::endl;
        exit(EXIT_FAILURE);
    }

    doListClients();
}
