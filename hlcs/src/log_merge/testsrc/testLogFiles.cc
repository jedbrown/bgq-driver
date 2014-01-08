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


#include "Interval.hpp"
#include "LogFiles.hpp"

#include <stdexcept>


#define BOOST_TEST_MODULE LogFiles
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace log_merge;

using std::runtime_error;


BOOST_AUTO_TEST_CASE( test_starts_empty )
{
    LogFiles log_files;
    BOOST_CHECK( ! log_files.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_add_log_with_no_record_still_empty )
{
    LogFiles log_files;
    bool no_records;
    log_files.add( "test_sample/empty.log", &no_records );

    BOOST_CHECK( no_records );
    BOOST_CHECK( ! log_files.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_get_log_records_one_log_file )
{
    LogFiles log_files;
    bool no_records;

    log_files.add( "test_sample/mmcs_two_lines.log", &no_records );
    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK( ! no_records );

    const LogFile &log_file(log_files.getLogWithOldestRecord());

    BOOST_CHECK_EQUAL( log_file.getFile().getPath(), "test_sample/mmcs_two_lines.log" );
}


BOOST_AUTO_TEST_CASE( test_get_log_records_two_files )
{
    LogFiles log_files;
    bool no_records;

    log_files.add( "test_sample/log_file1.log", &no_records );
    BOOST_CHECK( ! no_records );

    log_files.add( "test_sample/log_file2.log", &no_records );
    BOOST_CHECK( ! no_records );

    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file2.log" );

    log_files.advance();
    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file1.log" );

    log_files.advance();
    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file2.log" );

    log_files.advance();
    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file1.log" );

    log_files.advance();
    BOOST_CHECK( ! log_files.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_interval )
{
    // Can pass an interval to the LogFiles and only get records in that interval.

    LogFiles log_files(
            Interval::create(
                    boost::posix_time::time_from_string( "2010-05-03 16:00:10.757" ),
                    boost::posix_time::time_from_string( "2010-05-05 16:00:10.757" )
                )
        );

    bool no_records;

    log_files.add( "test_sample/interval1.log", &no_records );

    BOOST_CHECK( ! no_records );

    BOOST_CHECK( log_files.hasRecord() );

    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getTimestamp(),
            boost::posix_time::time_from_string( "2010-05-04 16:00:10.757" )
        );

    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getText(),
            "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line2"
        );

    log_files.advance();

    BOOST_CHECK( ! log_files.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_log_file_doesnt_exist )
{
    LogFiles log_files;
    bool no_records;

    BOOST_CHECK_THROW( log_files.add( "test_sample/noexist.log", &no_records ), runtime_error );
}


BOOST_AUTO_TEST_CASE( test_add_log_file_twice )
{
    LogFiles log_files;

    bool no_records;

    log_files.add( "test_sample/log_file1.log", &no_records );
    log_files.add( "test_sample/log_file1.log", &no_records );

    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getText(),
            "2010-04-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57"
        );
    log_files.advance();
    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getText(),
            "2010-06-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server"
        );
    log_files.advance();
    BOOST_CHECK( ! log_files.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_LogFilesMapped )
{
    LogFiles log_files( Interval::DefaultPtr, FilenameMapper( MapFilenames::Enabled ) );

    bool no_records;

    log_files.add( "test_sample/bgqsn2-Q05-I7-J02-bgagentd.log", &no_records );

    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getLabel(), "bgagentd      " );
}


BOOST_AUTO_TEST_CASE( test_directory )
{
    // Add a directory to the log files adds the files in that directory

    LogFiles log_files;

    bool no_records;

    log_files.add( "test_sample/d1", &no_records );

    BOOST_CHECK( log_files.hasRecord() );

    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getText(),
            "2010-04-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57"
        );
}


BOOST_AUTO_TEST_CASE( test_update_one_file )
{
    // Can set the current position of a log file, update the log file, then get the log records.

    LogFiles log_files;

    log_files.setPath( "test_sample/interval1.log", 0 );

    BOOST_CHECK( ! log_files.hasRecord() );

    Paths paths_to_update;
    paths_to_update.push_back( "test_sample/interval1.log" );

    log_files.updatePaths(
            paths_to_update,
            Interval::create(
                    boost::posix_time::min_date_time,
                    boost::posix_time::time_from_string( "2010-04-05 16:00:10.757" )
                )
        );

    BOOST_REQUIRE( log_files.hasRecord() );

    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getText(),
            "2010-04-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line1"
        );

    log_files.advance();

    BOOST_CHECK( ! log_files.hasRecord() );

    CurrentPositions current_positions(log_files.getCurrentPositions( paths_to_update ));

    BOOST_CHECK_EQUAL( current_positions["test_sample/interval1.log"], 78 );

    log_files.updatePaths(
            paths_to_update,
            Interval::create(
                    boost::posix_time::min_date_time,
                    boost::posix_time::time_from_string( "2010-05-05 16:00:10.757" )
                )
        );

    BOOST_REQUIRE( log_files.hasRecord() );

    BOOST_CHECK_EQUAL(
            log_files.getLogWithOldestRecord().getCurrentRecord().getText(),
            "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line2"
        );
}


BOOST_AUTO_TEST_CASE( test_update_two_files )
{
    // Can update two log files and get merged records from both.

    LogFiles log_files;

    log_files.setPath( "test_sample/log_file1.log", 0 );
    log_files.setPath( "test_sample/log_file2.log", 0 );

    Paths paths_to_update;
    paths_to_update.push_back( "test_sample/log_file1.log" );
    paths_to_update.push_back( "test_sample/log_file2.log" );

    log_files.updatePaths(
            paths_to_update,
            Interval::create(
                    boost::posix_time::min_date_time,
                    boost::posix_time::time_from_string( "2010-04-05 16:00:10.757" )
                )
        );

    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file2.log" );

    log_files.advance();

    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file1.log" );

    log_files.advance();
    BOOST_CHECK( ! log_files.hasRecord() );

    CurrentPositions current_positions(log_files.getCurrentPositions( paths_to_update ));

    BOOST_CHECK_EQUAL( current_positions["test_sample/log_file1.log"], 153 );
    BOOST_CHECK_EQUAL( current_positions["test_sample/log_file2.log"], 153 );

    log_files.updatePaths(
            paths_to_update,
            Interval::create(
                    boost::posix_time::min_date_time,
                    boost::posix_time::time_from_string( "2010-05-05 16:00:10.758" )
                )
        );

    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file2.log" );

    log_files.advance();
    BOOST_CHECK( ! log_files.hasRecord() );

    CurrentPositions current_positions2(log_files.getCurrentPositions( paths_to_update ));

    BOOST_CHECK_EQUAL( current_positions2["test_sample/log_file1.log"], 153 );
    BOOST_CHECK_EQUAL( current_positions2["test_sample/log_file2.log"], 258 );
}


BOOST_AUTO_TEST_CASE( test_update_new_file )
{
    // Can add a new file when doing updatePaths

    LogFiles log_files;

    log_files.setPath( "test_sample/log_file2.log", 0 );

    Paths paths_to_update;
    paths_to_update.push_back( "test_sample/log_file1.log" );
    paths_to_update.push_back( "test_sample/log_file2.log" );

    log_files.updatePaths(
            paths_to_update,
            Interval::create(
                    boost::posix_time::min_date_time,
                    boost::posix_time::time_from_string( "2010-04-05 16:00:10.757" )
                )
        );

    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file2.log" );

    log_files.advance();

    BOOST_REQUIRE( log_files.hasRecord() );
    BOOST_CHECK_EQUAL( log_files.getLogWithOldestRecord().getFile().getPath(), "test_sample/log_file1.log" );

    log_files.advance();
    BOOST_CHECK( ! log_files.hasRecord() );
}
