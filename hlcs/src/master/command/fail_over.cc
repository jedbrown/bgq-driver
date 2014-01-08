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
doFailover(
        std::string& target,
        std::string& trigger
        )
{
    std::vector<BinaryId> bids;
    bids.push_back(target);
    try {
        client.fail_over(bids, trigger);
    } catch(exceptions::BGMasterError& e) {
        std::cerr << "Could not fail over selected binaries. Error is: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!bids.empty()) {
        // Some didn't fail over
        for (std::vector<BinaryId>::iterator it = bids.begin();
            it != bids.end(); ++it) {
            std::cerr << "Could not fail over " << it->str() << std::endl;
        }
        exit(EXIT_FAILURE);
    }
}

void
usage()
{
    std::cerr << "fail_over binary [ OPTIONS ]" << std::endl;
}

void
help()
{
    usage();
    std::cerr << "Force a specific binary id to be killed and restarted on another node" << std::endl;
    std::cerr << "This behavior is managed by the configured policy." << std::endl << std::endl;
    std::cerr << "The 'binary' argument is the binary id of the specific binary to fail over." << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  --trigger (b/k/[a])  Trigger (binary, killed, agent) that will be used" << std::endl;
    std::cerr << "                            to find the associated policy for failover" << std::endl;
    std::cerr << "  --properties arg     Blue Gene configuration file" << std::endl;
    std::cerr << "  --help               This help text" << std::endl;
    std::cerr << "  --host               host:port pair to use to connect to bgmaster_server" << std::endl;
    std::cerr << "Administrative authority required." << std::endl;
}

int main(int argc, const char** argv)
{
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
    catch(exceptions::BGMasterError& e) {
        std::cerr << "Unable to contact bgmaster_server, server may be down." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pargs->size() != 0) {
        std::string trigger = (*pargs)[trigarg];
        if (trigger.length() != 0) {
            if (trigger != "a" && trigger != "k" && trigger != "b") {
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
