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


#include "LogMerge.hpp"

#include <boost/assign.hpp>

#include <sstream>
#include <string>


#define BOOST_TEST_MODULE LogMerge
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace boost::assign;

using namespace log_merge;


using boost::posix_time::time_from_string;

using std::ostringstream;
using std::string;


BOOST_AUTO_TEST_CASE( test_one_file )
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/mmcs_two_lines.log" ) );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    BOOST_CHECK_EQUAL( result,
"mmcs_two_lines.log: 2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57\n"
"mmcs_two_lines.log: 2010-05-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server\n"
        );
}


BOOST_AUTO_TEST_CASE( test_two_files)
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/log_file1.log" )( "test_sample/log_file2.log" ) );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    BOOST_CHECK_EQUAL( result,

"log_file2.log: 2010-03-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57\n"
"log_file1.log: 2010-04-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57\n"
"log_file2.log: 2010-05-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server\n"
"log_file1.log: 2010-06-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server\n"

        );
}


BOOST_AUTO_TEST_CASE( test_single_file_no_exist )
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/noexist.log" ) );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    BOOST_CHECK_EQUAL( result, "WARNING, failed to open 'test_sample/noexist.log', error is 'No such file or directory'.\n" );
}


BOOST_AUTO_TEST_CASE( test_two_files_first_no_exist )
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/noexist.log" )( "test_sample/log_file1.log" ) );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    BOOST_CHECK_EQUAL( result,

"WARNING, failed to open 'test_sample/noexist.log', error is 'No such file or directory'.\n"
"log_file1.log: 2010-04-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57\n"
"log_file1.log: 2010-06-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server\n"

        );
}


BOOST_AUTO_TEST_CASE( test_one_file_no_records )
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/no_log_record.log" ) );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    BOOST_CHECK_EQUAL( result, "WARNING, 'test_sample/no_log_record.log' did not contain any log records.\n" );
}


BOOST_AUTO_TEST_CASE( test_one_file_interval )
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/interval1.log" ) );

    config.setStartTime( time_from_string( "2010-05-03 16:00:10.757" ) );
    config.setEndTime( time_from_string( "2010-05-05 16:00:10.757" ) );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    string exp_result(

"interval1.log: 2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line2\n"

        );

    BOOST_CHECK_EQUAL( result, exp_result );
}


BOOST_AUTO_TEST_CASE( test_mapped_names )
{
    LogMerge log_merge;

    ostringstream oss;

    log_merge.setOutput( oss );

    Configuration config;
    config.setPaths( list_of( "test_sample/bgqsn2-Q05-I7-J02-bgagentd.log" ) );
    config.setMapFilenames( MapFilenames::Enabled );

    log_merge.setConfiguration( config );

    log_merge.run();

    string result(oss.str());

    BOOST_CHECK_EQUAL( result,

"bgagentd      : 2010-05-12 00:17:58.556 (ERROR) [0x40002045d60] ibm.utility.cxxsockets.TCPSocket: cannot connect() Connection refused\n"
"bgagentd      : 2010-05-12 00:17:58.556 (DEBUG) [0x40002045d60] ibm.hlcs.master.BGMasterProtocol: Server not available.  connect error: Connection refused Retrying.\n"
"bgagentd      : 2010-05-12 00:17:58.562 (ERROR) [0x40002045d60] ibm.utility.cxxsockets.TCPSocket: cannot connect() Connection refused\n"
"bgagentd      : 2010-05-12 00:17:58.562 (DEBUG) [0x40002045d60] ibm.hlcs.master.BGMasterProtocol: Server not available.  connect error: Connection refused Retrying.\n"
"bgagentd      : 2010-05-12 00:17:58.613 (ERROR) [0x40002045d60] ibm.utility.cxxsockets.TCPSocket: cannot connect() Connection refused\n"

        );

}


BOOST_AUTO_TEST_CASE( test_validate_paths )
{
    // empty list works.
    LogMerge::validatePaths( Paths() );

    {
    // existing file doesn't throw
    Paths paths = list_of( "test_sample/mmcs_two_lines.log" );
    LogMerge::validatePaths( paths );
    }

    {
    // directory doesn't throw
    Paths paths = list_of( "test_sample/d1" );
    LogMerge::validatePaths( paths );
    }

    {
    // throws exception when
    Paths paths = list_of( "test_sample/no_such_file" );
    BOOST_CHECK_THROW( LogMerge::validatePaths( paths ), std::runtime_error );
    }
}
