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

#include <boost/tokenizer.hpp>
#include "ArgParse.h"
#include "BGMasterClientApi.h"
#include "BGMasterExceptions.h"

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;
void help() {
    std::cerr << "Get the current contents of bgmaster_server's event history ring buffer." << std::endl;
}

void usage() {
    std::cerr << "get_history [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int main(int argc, const char** argv) {
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
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

    std::vector<std::string> history;
    try {
        client.get_history(history);
    } catch (BGMasterExceptions::BGMasterError& e) {
        std::cerr << "get_history failed: " << e.what() << std::endl;
    }

    for(std::vector<std::string>::iterator it = history.begin();
        it != history.end(); ++it) {
        std::cout << *it << std::endl;
    }
}
