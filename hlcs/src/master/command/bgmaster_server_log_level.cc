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

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <csignal>

LOG_DECLARE_FILE( "master" );

BGMasterClient client;
Args* pargs;

void help()
{
    std::cerr << "bgmaster_server_log_level" << std::endl;
    std::cerr << std::endl << "Displays the logging configuration for the Blue Gene bgmaster server."
              << " Optionally sets the logging configuration." << std::endl << std::endl;
    std::cerr << "Options:"  << std::endl;
    std::cerr << "  --host|-H arg                         Server to connect to" << std::endl;
    std::cerr << "  --help|-h                             This help text" << std::endl;
    std::cerr << "  --properties|-p arg                   Blue Gene configuration file" << std::endl;
    std::cerr << "  --verbose|-v arg                      Logging configuration" << std::endl;
    std::cerr << std::endl;
}

void usage()
{
    std::cerr << "bgmaster_server_log_level [ --properties filename ] [ --help ] [ --host host:port ] [ --verbose verbosity ] [ logger=level ]" << std::endl;
}

void
doLoglevel(
        std::map<BGMasterClient::LogName,
        BGMasterClient::LogLevel>& logmap
        )
{
    try {
        client.log_level(logmap);
        typedef std::pair<BGMasterClient::LogName, BGMasterClient::LogLevel> logpair;
        BOOST_FOREACH(logpair curr_pair, logmap) {
            if (curr_pair.first.length() != 0) {
                std::cout << std::setw(40) << std::left
                          << curr_pair.first << std::right
                          << curr_pair.second << std::endl;
            }
        }
    } catch(exceptions::BGMasterError& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, const char** argv)
{
    std::vector<std::string> validargs;
    std::vector<std::string> singles;
    validargs.push_back("*"); // One argument without a "--" is allowed

    Args largs(argc, argv, &usage, &help, validargs, singles);
    pargs = &largs;
    client.initProperties(pargs->get_props());
    try {
        client.connectMaster(pargs->get_portpairs());
    }
    catch(exceptions::BGMasterError& e) {
        std::cerr << "Unable to contact bgmaster_server, server may be down." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::map<BGMasterClient::LogName, BGMasterClient::LogLevel> logmap;
    for(Args::iterator it = pargs->begin(); it != pargs->end(); ++it) {
        if ((*it).find("=") == std::string::npos) {
            std::cerr << "Invalid logger=level pair \'" << *it << "\'" << std::endl;
            usage();
            exit(EXIT_FAILURE);
        }
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep("=");
        tokenizer tok(*it, sep);
        tokenizer::iterator curr_tok = tok.begin();
        std::string lname = *curr_tok;
        ++curr_tok;
        std::string llev = *curr_tok;
        logmap[lname] = llev;
    }
    doLoglevel(logmap);
}
