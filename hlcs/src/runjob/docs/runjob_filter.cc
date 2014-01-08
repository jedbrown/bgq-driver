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

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <ramdisk/include/services/JobctlMessages.h>

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

    // database objects
    BGQDB::DBTJob job;
    BGQDB::DBTBlock block;

    // get line
    std::string line;
    while ( getline(file, line) ) {
        // replace argument size
        boost::replace_first(
                line, 
                "LOAD_JOB_ARGS_SIZE", 
                boost::lexical_cast<std::string>(bgcios::jobctl::MaxArgumentSize + 1)
                );

        // replace tool argument size
        boost::replace_first(
                line, 
                "TOOL_ARGS_SIZE", 
                boost::lexical_cast<std::string>(bgcios::jobctl::MaxArgumentSize + 1)
                );

        // replace exe size
        boost::replace_first(
                line, 
                "EXE_SIZE", 
                boost::lexical_cast<std::string>( sizeof(job._executable) )
                );

        // replace block size
        boost::replace_first(
                line, 
                "BLOCK_SIZE", 
                boost::lexical_cast<std::string>( sizeof(block._blockid) )
                );

        // replace env size
        boost::replace_first(
                line, 
                "LOAD_JOB_ENVS_SIZE", 
                boost::lexical_cast<std::string>(bgcios::jobctl::MaxVariableSize)
                );

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

        // replace cwd size
        boost::replace_first(
                line, 
                "WORKING_DIRECTORY_SIZE",
                boost::lexical_cast<std::string>( bgcios::jobctl::MaxPathSize )
                );

        // replace UNIX_PATH_MAX
        sockaddr_un size;
        boost::replace_first(
                line,
                "UNIX_PATH_MAX",
                boost::lexical_cast<std::string>( sizeof( size.sun_path ) - 1 )
                );

        // replace mapping size
        boost::replace_first(
                line, 
                "MAPPING_SIZE", 
                boost::lexical_cast<std::string>( sizeof(job._mapping) )
                );

        // output line
        std::cout << line << std::endl;
    }

    return 0;
}


