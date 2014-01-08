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
help()
{
    std::cerr << "Get the current contents of bgmaster_server error ring buffer." << std::endl;
}

void
usage()
{
    std::cerr << "get_errors [ --properties filename ] [ --host host:port ] [ --verbose verbosity ]" << std::endl;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    const Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;

    try {
        client.connectMaster(largs.get_props(), largs.get_portpairs());
    }
    catch ( const exceptions::BGMasterError& e ) {
        std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> errors;
    try {
        client.get_errors(errors);
    } catch (exceptions::BGMasterError& e) {
        std::cerr << "get_errors failed: " << e.what() << std::endl;
    }

    for (std::vector<std::string>::iterator it = errors.begin();
        it != errors.end(); ++it) {
        std::cout << *it << std::endl;
    }
}
