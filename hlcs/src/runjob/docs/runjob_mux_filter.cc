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
#include <utility/include/version.h>

#include "common/defaults.h"

#include <boost/date_time/gregorian/gregorian.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iostream>
#include <string>

#include <sys/un.h>

int
main(int argc, const char** argv)
{
    // validate args
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }

    // open input file
    std::ifstream file( argv[1] );
    if ( !file ) {
        exit(EXIT_FAILURE);
    }

    // get line
    std::string line;
    while ( getline(file, line) ) {
        // replace driver tag
        boost::replace_first(
                line, 
                "DRIVER_NAME",
                bgq::utility::DriverName
                );

        // replace version number
        boost::replace_first(
                line, 
                "VERSION_NUMBER",
                bgq::utility::Revision
                );

        // replace date
        boost::replace_first(
                line, 
                "DOCUMENTATION_DATE",
                boost::gregorian::to_simple_string( boost::gregorian::day_clock::local_day() )
                );

        // replace UNIX_PATH_MAX
        sockaddr_un size;
        boost::replace_first(
                line,
                "UNIX_PATH_MAX",
                boost::lexical_cast<std::string>( sizeof( size.sun_path ) - 1 )
                );

        // output line
        std::cout << line << std::endl;
    }

    return 0;
}


