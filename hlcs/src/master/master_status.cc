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

void doStat(bool fancy) {
    int mpid = 0;
    std::string start_time = "";
    std::string version = "";

    if(fancy) {
        std::vector<std::string> idles;
        try {
            client.idle_aliases(idles);
        } catch (BGMasterExceptions::BGMasterError& e) {
            std::cerr << "idle_aliases failed: " << e.what() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "idle_aliases failed: " << e.what() << std::endl;
        }
        std::cout << std::endl;
        if(idles.size() > 0) {
            std::cout << "Aliases not currently running" << std::endl;
            std::cout << "-----------------------------" << std::endl; 
            BOOST_FOREACH(std::string& al, idles) {
                if(al != "bgmaster_server")
                    std::cout << al << std::endl;
            }
        } else {
            std::cout << "All configured aliases running." << std::endl;
        }
    }

    try {
        mpid = client.master_status(start_time, version);
    } catch (BGMasterExceptions::BGMasterError& e) {
        std::cerr << "master_status failed: " << e.what() << std::endl;
    } catch (std::runtime_error& e) {
        std::cerr << "master status failed: " << e.what() << std::endl;
    }
    if(mpid > 0) {
        ListAgents::doListAgents(client, false, fancy);
        std::cout << std::endl << version << " running under pid " << mpid 
                  << " since " << start_time << std::endl;
    } else {
        std::cerr << "bgmaster_server process id unavailable" << std::endl;
    }

}

void help() {
    std::cerr << "Return the process id of bgmaster_server and information about managed processes. " << std::endl;
}

void usage() {
    std::cerr << "master_status [ --properties filename ] [ --help ] [ --fancy ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
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
    } catch (BGMasterExceptions::CommunicationError& e) {
        std::cerr << "Could not connect to bgmaster_server.  Presumed dead.  " << e.what() << std::endl;
        exit(1);
    }
    doStat(fancy);
}
