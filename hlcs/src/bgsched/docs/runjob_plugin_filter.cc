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
#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <utility/include/version.h>

#include <boost/algorithm/string/replace.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iostream>
#include <string>

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

    // construct gcc version string
    std::ostringstream os;
    os << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;

    BGQDB::DBTJob job_table;

    // get line
    std::string line;
    while ( getline(file, line) ) {
        // replace gcc version number
        boost::replace_first(
                line, 
                "GCC_VERSION",
                os.str()
                );

        // replace scheduler data size
        boost::replace_first(
                line,
                "SCHEDULER_DATA_SIZE",
                boost::lexical_cast<std::string>( sizeof(job_table._schedulerdata) )
                );

        // replace block name size
        boost::replace_first(
                line,
                "BLOCK_NAME_SIZE",
                boost::lexical_cast<std::string>( sizeof(job_table._blockid) )
                );

        // replace exe size
        boost::replace_first(
                line, 
                "EXE_SIZE", 
                boost::lexical_cast<std::string>( sizeof(job_table._executable) )
                );

        // replace argument size
        boost::replace_first(
                line, 
                "LOAD_JOB_ARGS_SIZE", 
                boost::lexical_cast<std::string>(bgcios::jobctl::MaxArgumentSize + 1)
                );

        // replace env size
        boost::replace_first(
                line, 
                "LOAD_JOB_ENVS_SIZE", 
                boost::lexical_cast<std::string>(bgcios::jobctl::MaxVariableSize + 1)
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

        // output line
        std::cout << line << std::endl;
    }

    return 0;
}


