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
#include "ListAgents.h"

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;

void help() {
    std::cerr << "Returns a list of IDs for active agents " << std::endl;
    std::cerr << "and their associated running binaries." << std::endl;
}

void usage() {
    std::cerr << "list_agents [ --properties filename ] [ --help ] [ --fancy ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int main(int argc, const char** argv) {

    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    std::string fancyarg = "--fancy";
    singles.push_back(fancyarg);
    Args largs(argc, argv, &usage, &help, validargs, singles);
    pargs = &largs;
    client.initProperties(pargs->_props);

    bool fancy = largs.find_arg(fancyarg);

    try {
        client.connectMaster(pargs->get_portpairs());
    }
    catch(BGMasterExceptions::BGMasterError& e) {
        LOG_FATAL_MSG("Unable to contact bgmaster_server. " << e.what());
        exit(EXIT_FAILURE);
    }

    ListAgents::doListAgents(client, true, fancy);
}
