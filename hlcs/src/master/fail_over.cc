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

void doFailover( std::string& target, std::string& trigger) {
    std::vector<BinaryId> bids;
    bids.push_back(target);
    try {
        client.fail_over(bids, trigger);
    } catch(BGMasterExceptions::BGMasterError& e) {
        LOG_FATAL_MSG("Could not fail over selected binaries.  " << e.what());
        exit(EXIT_FAILURE);
    }

    if(bids.size() != 0) {
        // Some didn't fail over
        for(std::vector<BinaryId>::iterator it = bids.begin();
            it != bids.end(); ++it) {
            std::cerr << "Could not fail over " << it->str() << std::endl;
        }
        exit(EXIT_FAILURE);
    }
}

void usage() {
    std::cerr << "fail_over binary [ OPTIONS ]  " << std::endl;
}

void help() {
    usage();
    std::cerr << "Force a specific binary id to be killed and restarted" << std::endl;
    std::cerr << "on another node. This behavior is governed by the" << std::endl;
    std::cerr << "configured policy." << std::endl << std::endl;
    std::cerr << "The 'binary' argument is the binary ID of the specific binary" << std::endl;
    std::cerr << "to fail over." << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  --trigger (b/k/[a])  Trigger (binary, killed, agent) that" << std::endl;
    std::cerr << "                            will be used to find the associated" << std::endl;
    std::cerr << "                            policy for failover" << std::endl;
    std::cerr << "  --properties arg   Blue Gene configuration file" << std::endl;
    std::cerr << "  --help             This help text" << std::endl;
    std::cerr << "  --host             host:port pair to use to connect to bgmaster_server" << std::endl;
    std::cerr << "Administrative authority required." << std::endl;
}

int main(int argc, const char** argv) {

    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    std::string trigarg = "--trigger";
    validargs.push_back("*"); // One argument without a "--" is allowed
    validargs.push_back(trigarg);
    Args largs(argc, argv, &usage, &help, validargs, singles);
    pargs = &largs;
    client.initProperties(pargs->_props);

    try {
        client.connectMaster(pargs->get_portpairs());
    }
    catch(BGMasterExceptions::BGMasterError& e) {
        LOG_FATAL_MSG("Unable to contact bgmaster_server. " << e.what());
        exit(EXIT_FAILURE);
    }
    if(pargs->size() != 0) {
        std::string trigger = (*pargs)[trigarg];
        if(trigger.length() != 0) {
            if(trigger != "a" &&
               trigger != "k" &&
               trigger != "b") {
                std::cerr << "Invalid trigger " << trigger << " specified." << std::endl;
                usage();
                exit(EXIT_FAILURE);
            }
        }
        doFailover(*(pargs->begin()), trigger);
    } else {
        usage();
        exit(EXIT_FAILURE);
    }
}
