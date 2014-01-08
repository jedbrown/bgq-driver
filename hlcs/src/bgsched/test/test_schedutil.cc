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

#include <bgsched/bgsched.h>
#include <bgsched/SchedUtil.h>

#include <utility/include/Log.h>

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <strings.h>

using namespace bgsched;
using namespace log4cxx;
using namespace std;

LOG_DECLARE_FILE( "testschedutil" );

// Obtain a backtrace and print it to stderr.
void
print_trace (void)
{
    void *array[20];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace (array, 20);
    strings = backtrace_symbols (array, size);

    cerr << "Obtained " << size << " stack frames." << endl;

    for (i = 0; i < size; i++)
        cerr << strings[i] << endl;

    cerr.flush();
    free (strings);
}

void
failed (void)
{
    LOG_ERROR_MSG("!!!FAILED!!!");
    exit(-1);
}

void
testcase100()
{
    string prefix = "test";
    string blockName = SchedUtil::createBlockName(prefix);
    LOG_INFO_MSG("Block name created: " << blockName);
}

void
testcase200()
{
    string prefix = "THISBLOCKIS16LNG";
    LOG_INFO_MSG("Block prefix is " << prefix);
    string blockName = SchedUtil::createBlockName(prefix);
    LOG_INFO_MSG("Block name created: " << blockName);
}

void
testcase300()
{
    string prefix = "TRUNCATEBLOCKTO16";
    LOG_INFO_MSG("Block prefix is " << prefix);
    string blockName = SchedUtil::createBlockName(prefix);
    LOG_INFO_MSG("Block name created: " << blockName);
}

void
testcase400()
{
    string prefix = "INVALID!@#$CHARS";
    LOG_INFO_MSG("Block prefix is " << prefix);
    string blockName = SchedUtil::createBlockName(prefix);
    LOG_INFO_MSG("Block name created: " << blockName);
}

void
testcase500()
{
    string prefix;
    LOG_INFO_MSG("Block prefix is " << prefix);
    string blockName = SchedUtil::createBlockName(prefix);
    LOG_INFO_MSG("Block name created: " << blockName);
}

int
main(int argc, const char** argv)
{
    // iterate through args first to get -properties
    string properties;
    for (int i = 1; i < argc; ++i) {
        if (!strcasecmp(argv[i], "-properties")) {
            if (++i == argc) {
                cerr << "Please give a file name after " << argv[i-1] << endl;
                exit(EXIT_FAILURE);
            } else {
                properties = argv[i];
            }
        } else if (!strcasecmp(argv[i], "-help") || !strcasecmp(argv[i], "-h")) {
            cerr << argv[0] << ":" << endl;
            cerr << "\t -properties <file>     : Set bg.properties file name" << endl;
            cerr << "\t -help | -h             : Print this help text" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    // init library
    init(properties);
    LOG_INFO_MSG( "test_schedutil running testcases" );

    try
    {
        LOG_INFO_MSG( "=== Get block name using prefix of 'test': should complete OK ===" );
        testcase100();
        LOG_INFO_MSG( "=== Get block name using 16 character prefix: should complete OK ===" );
        testcase200();
        LOG_INFO_MSG( "=== Get block name using 17 character prefix (truncated to 16): should complete OK ===" );
        testcase300();
        LOG_INFO_MSG( "=== Get block name using invalid prefix (will use default prefix): should complete OK ===" );
        testcase400();
        LOG_INFO_MSG( "=== Get block name using empty prefix (will use default prefix): should complete OK ===" );
        testcase500();
     }
    catch(...)
    {
        LOG_ERROR_MSG( "Exception caught" );
        print_trace();
        throw;
    }
}
