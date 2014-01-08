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

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>


LOG_DECLARE_FILE( "master" );

void help()
{
    std::cerr << std::endl << "Displays the logging configuration for the Blue Gene bgmaster server."
              << " Optionally sets the logging configuration." << std::endl << std::endl;
    std::cerr << "Administrative authority required." << std::endl << std::endl;
    std::cerr << "Options:"  << std::endl;
    std::cerr << "  --host|-H arg                         Server to connect to" << std::endl;
    std::cerr << "  --help|-h                             This help text" << std::endl;
    std::cerr << "  --properties|-p arg                   Blue Gene configuration file" << std::endl;
    std::cerr << "  --verbose|-v arg                      Logging configuration" << std::endl;
    std::cerr << std::endl;
}

void usage()
{
    std::cerr << "bgmaster_server_log_level [logger=level [... logger=level]]" << std::endl;
}

int
doLoglevel(
        const BGMasterClient& client,
        std::vector<std::string>& input
        )
{
    try {
        typedef std::map<BGMasterClient::LogName,BGMasterClient::LogLevel> Output;
        Output output;
        client.log_level(input, output);
        BOOST_FOREACH(const Output::value_type& i, output) {
            if (i.first.empty()) continue;
            std::cout << std::setw(40) << std::left
                << i.first << std::right
                << i.second << std::endl;
        }
    } catch (const exceptions::BGMasterError& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int
main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    validargs.push_back("**"); // Unlimited number of positional arguments

    Args largs(argc, argv, &usage, &help, validargs, singles);
    BGMasterClient client;
    try {
        client.connectMaster(largs.get_props(), largs.get_portpairs());
    } catch (const exceptions::BGMasterError& e) {
        std::cerr << "Unable to contact bgmaster_server: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> loggers;
    for (Args::const_iterator it = largs.begin(); it != largs.end(); ++it) {
        loggers.push_back( *it );
    }

    return doLoglevel(client, loggers);
}
