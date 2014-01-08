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

#include <utility/include/Log.h>



LOG_DECLARE_FILE( "master" );

void
doReload(
        const BGMasterClient& client,
        const std::string& config_file
        )
{
    try {
        client.reload_config(config_file);
    } catch ( const exceptions::ConfigError& e ) {
        std::cerr << e.what() << std::endl;
        if (e.errcode == exceptions::FATAL) {
            exit(EXIT_FAILURE);
        } else {
            exit(0);
        }
    } catch ( const exceptions::CommunicationError& e ) {
        std::cerr <<  "Configuration file not reloaded, error is: " << e.what() << std::endl;
        if (e.errcode == exceptions::FATAL) {
            exit(EXIT_FAILURE);
        } else {
            exit(0);
        }
    }
}

void
help()
{
    std::cerr << "Reload the policies defined by bg.properties." << std::endl;
    std::cerr << "Old policies are not discarded, only new ones are added." << std::endl;
    std::cerr << "Administrative authority required." << std::endl;
}

void
usage()
{
    std::cerr << "bgmaster_server_refresh_config [ --help ] [ --host host:port ] [ --verbose verbosity ] filename" << std::endl;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    validargs.push_back("*"); // One argument without a "--" is allowed
    const Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;

    try {
        client.connectMaster(largs.get_props(), largs.get_portpairs());
    } catch ( const exceptions::BGMasterError& e ) {
        std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (largs.size() == 0) {
        const std::string empty;
        doReload(client, empty);
    } else {
        std::cout << "Reloading configuration using file " << *largs.begin() << "." << std::endl;
        doReload(client, *largs.begin());
    }
    std::cout << "Successfully reloaded configuration." << std::endl;
}
