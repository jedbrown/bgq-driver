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

const char* common_arguments = 
"\\subsection version --version\n"
"Display version information and exit.\n"
"\n"
"\\subsection properties --properties\n"
"Location of BlueGene properties file.\n"
"\n"
"\\subsection verbose --verbose\n"
"\\if MAN\n"
"\\verbinclude verbose.man\n"
"\\else\n"
"\\htmlinclude verbose.html\n"
"\\endif\n"
"\n"
"\\subsection host --host\n"
"server host and port to connect to.  Enclose ipv6 addresses in square brackets. For\n"
"link-local addresses, append the interface to the address after a % character.  Specify the\n"
"port after the interface using a colon.\n"
"\n"
"The following values are examples values:\n"
"\\if MAN\n"
"\\verbinclude host_syntax.man\n"
"\\else\n"
"\\htmlinclude host_syntax.html\n"
"\\endif\n"
"\n"
"\\subsection wait_for_server --wait-for-server\n"
"Keep trying to connect to the server if it is unavailable.\n"
"\n"
"\\subsection help --help\n"
"Display help text and ext.\n"
;

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

        // replace common arguments
        boost::replace_first(
                line, 
                "COMMON_ARGUMENTS_GO_HERE",
                common_arguments
                );
        // output line
        std::cout << line << std::endl;
    }

    return 0;
}


