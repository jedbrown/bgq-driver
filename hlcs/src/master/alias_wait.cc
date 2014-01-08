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
#include <boost/lexical_cast.hpp>
#include "ArgParse.h"
#include "BGMasterClientApi.h"
#include "BGMasterExceptions.h"

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;

void doAliasWait(std::string& target, std::string& timeout) {
    BinaryId id;
    int to = 0;
    if(timeout.length() != 0) {
        try {
            to = boost::lexical_cast<unsigned>(timeout);
        } catch(boost::bad_lexical_cast& e) {
            std::cerr << "Invalid timeout value " << timeout << ".  " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
        if(to < 0) {
            std::cerr << "Invalid timeout " << to << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    try {
        id = client.alias_wait(target, to);
    } catch(BGMasterExceptions::BGMasterError& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
    std::cout << "Waited alias " << target << " running under id " << id.str() << std::endl;
}

void help() {
    std::cerr << "Waits for a specified alias to become active. " << std::endl;
}

void usage() {
    std::cerr << "alias_wait [alias] [ --timeout seconds ] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int main(int argc, const char** argv) {

    std::string timeout = "--timeout";
    std::vector<std::string> validargs;
    std::vector<std::string> singles;

    validargs.push_back("*"); // One argument without a "--" is allowed
    validargs.push_back(timeout);

    Args largs(argc, argv, &usage, &help, validargs, singles);
    pargs = &largs;
    client.initProperties(pargs->_props);

    std::string time_out = (*pargs)[timeout];
    std::string target;
    if(pargs->size() != 0)
        target = *(pargs->begin());
    else {
        LOG_FATAL_MSG("No alias specified");
        exit(EXIT_FAILURE);
    }

    try {
        client.connectMaster(pargs->get_portpairs());
    }
    catch(BGMasterExceptions::BGMasterError& e) {
        LOG_FATAL_MSG("Unable to contact bgmaster_server. " << e.what());
        exit(EXIT_FAILURE);
    }

    doAliasWait(target, time_out);
}
