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

void doReload(std::string& config_file) {
    try {
        client.reload_config(config_file);
    } catch(BGMasterExceptions::ConfigError& e) {
        std::cerr << e.what() << std::endl;
        if(e.errcode == BGMasterExceptions::FATAL)
            exit(EXIT_FAILURE);
        else
            exit(0);
    }
}

void help() {
    std::cerr << "Reload the policies defined by bg.properties." << std::endl;
    std::cerr << "Old policies are not discarded, only new ones are added." << std::endl;
    std::cerr << "Administrative authority required." << std::endl;
}

void usage() {
    std::cerr << "bgmaster_server_refresh_config [ --help ] [ --host host:port ] [ --verbose verbosity ] filename" << std::endl;
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

    if(pargs->size() == 0) {
        std::string empty = "";
        doReload(empty);
    }
    else {
        LOG_DEBUG_MSG("reloading using file " << *(pargs->begin()));
        doReload(*(pargs->begin()));
    }
}
