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
#include "BinaryController.h"
#include "BGMasterClientApi.h"
#include "BGMasterExceptions.h"

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;

void doStatus(std::string& target) {
    std::map<BinaryId, BinaryControllerPtr, Id::Comp> mm;
    bool bins_specified = false;
    if(!target.empty()) {
        bins_specified = true;
        BinaryControllerPtr p(new BinaryController);
        mm[target] = p;
    }
    client.status(mm);
    if(mm.size() == 0 && bins_specified) {
        std::cerr << target << " specifies no existing binaries." << std::endl;
        exit(EXIT_FAILURE);
    } else if(mm.size() == 0 && !bins_specified) {
        std::cerr << "No binary status available." << std::endl;
        exit(EXIT_FAILURE);
    }

    for(std::map<BinaryId, BinaryControllerPtr, Id::Comp>::iterator it = mm.begin();
        it != mm.end(); ++it) {
        BinaryId id = it->first;
        BinaryControllerPtr ptr = it->second;
        std::cout << id.str() << "|" << ptr->get_alias_name() << "|"
                  << BinaryController::status_to_string((BinaryController::Status)(ptr->get_status()))
                  << "|" << ptr->get_user() << "|" << ptr->get_start_time() << std::endl;
    }
}

void help() {
    std::cerr << "Returns the status of one or all binaries currently" << std::endl;
    std::cerr << "under control of bgmaster_server.  By default it returns" << std::endl;
    std::cerr << "the status of all binaries  and their associated agent ids." << std::endl;
    std::cerr << "If a binary id is specified, it returns the status of that binary." << std::endl;
}

void usage() {
    std::cerr << "binary_status [binary id] [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int main(int argc, const char** argv) {

    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    validargs.push_back("*"); // One argument without a "--" is allowed
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

    if(pargs->size() != 0)
        doStatus(*(pargs->begin()));
    else {
        std::string empty = "";
        doStatus(empty);
    }
}
