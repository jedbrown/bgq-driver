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


#include "Configuration.hpp"
#include "Interval.hpp"

#include <boost/assign.hpp>

#include <sstream>
#include <string>


#define BOOST_TEST_MODULE Configuration
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace boost::assign;

using boost::gregorian::date;

using boost::posix_time::ptime;
using boost::posix_time::time_duration;

using log_merge::Configuration;
using log_merge::Interval;

using std::istringstream;
using std::ostringstream;
using std::string;


static const string HelpText(

"Usage: log_merge [OPTION]... [FILE or DIRECTORY]...\n"
"Merge Blue Gene log files.\n"
"\n"
"Allowed options:\n"
"  --live                Live updates\n"
"  -m [ --map ]          Display short names for log files\n"
"  --start arg           Interval start time\n"
"  --end arg             Interval end time\n"
"  -0 [ --null ]         Read null-terminated file names from stdin\n"
"  --properties arg      Blue Gene configuration file\n"
"  --verbose arg         Logging configuration\n"
"  -h [ --help ]         Print this help text\n"
"\n"
"Use a FILENAME of - to read file or directory names from stdin.\n"
"\n"

    );


BOOST_AUTO_TEST_CASE( test_defaults )
{
    // When created, starts out with defaults.

    Configuration config;

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_CHECK( config.getPaths().empty() );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Disabled );
    BOOST_CHECK_EQUAL( config.getInterval(), *Interval::DefaultPtr );
    BOOST_CHECK( ! config.isLive() );
}


BOOST_AUTO_TEST_CASE( test_set_filenames )
{
    Configuration config;

    config.setPaths( list_of( "testfile1.log" ) );

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_REQUIRE_EQUAL( config.getPaths().size(), unsigned(1) );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "testfile1.log" );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Disabled );
    BOOST_CHECK_EQUAL( config.getInterval(), *Interval::DefaultPtr );
}


BOOST_AUTO_TEST_CASE( test_parseTimestamp )
{
    // Can provide a full date and time.
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "2010-05-03 16:00:10.757", true ),
            ptime( date( 2010, 5, 3 ), time_duration( 16, 0, 10, 757000 ) )
        );

    // Can put the date, is midnight if at start.
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "2010-05-03", true ),
            ptime( date( 2010, 5, 3 ) )
        );
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "2010-05-03", false ),
            ptime( date( 2010, 5, 4 ) )
        );

    // Can leave off the year.
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "05-03", true ),
            ptime( date( boost::gregorian::day_clock::local_day().year(), 5, 3 ) )
        );
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "05-03", false ),
            ptime( date( boost::gregorian::day_clock::local_day().year(), 5, 4 ) )
        );

    // If put a time like HH::MM::SS.sss then is today.
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "16:00:10.757", true ),
            ptime( boost::gregorian::day_clock::local_day(), time_duration( 16, 0, 10, 757000 ) )
        );

    // If put a time like HH::MM:SS then is today.
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "16:00:10", true ),
            ptime( boost::gregorian::day_clock::local_day(), time_duration( 16, 0, 10, 0 ) )
        );

    // If put a time like HH::MM then is today.
    BOOST_CHECK_EQUAL(
            Configuration::parseTimestamp( "16:00", true ),
            ptime( boost::gregorian::day_clock::local_day(), time_duration( 16, 0, 0, 0 ) )
        );

}


BOOST_AUTO_TEST_CASE( test_setters )
{
    Interval interval(
            boost::posix_time::time_from_string( "2010-05-03 16:00:10.757" ),
            boost::posix_time::time_from_string( "2010-05-05 16:00:10.757" )
        );

    Configuration config;

    config.setPaths( list_of( "testfile1.log" )( "testfile2.log" ) );
    config.setMapFilenames( log_merge::MapFilenames::Enabled );
    config.setStartTime( boost::posix_time::time_from_string( "2010-05-03 16:00:10.757" ) );
    config.setEndTime( boost::posix_time::time_from_string( "2010-05-05 16:00:10.757" ) );
    config.setLive( true );

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_REQUIRE_EQUAL( config.getPaths().size(), unsigned(2) );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "testfile1.log" );
    BOOST_CHECK_EQUAL( config.getPaths()[1], "testfile2.log" );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Enabled );
    BOOST_CHECK_EQUAL( config.getInterval(), interval );
    BOOST_CHECK( config.isLive() );
}


BOOST_AUTO_TEST_CASE( test_noArguments )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" ) );

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_REQUIRE_EQUAL( config.getPaths().size(), unsigned(1) );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "/bgsys/logs/BGQ" );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Disabled );
    BOOST_CHECK_EQUAL( oss.str(), "" );
    BOOST_CHECK_EQUAL( config.getInterval(), *Interval::DefaultPtr );
    BOOST_CHECK( ! config.isLive() );
}


BOOST_AUTO_TEST_CASE( test_noOptionsOneFile )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" )( "file1.log" ) );

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_REQUIRE_EQUAL( config.getPaths().size(), unsigned(1) );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "file1.log" );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Disabled );
    BOOST_CHECK_EQUAL( config.getInterval(), *Interval::DefaultPtr );
    BOOST_CHECK( ! config.isLive() );

    BOOST_CHECK_EQUAL( oss.str(), "" );
}


BOOST_AUTO_TEST_CASE( test_mFlag )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" )( "-m" ) );

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_REQUIRE_EQUAL( config.getPaths().size(), 1U );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "/bgsys/logs/BGQ" );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Enabled );
    BOOST_CHECK_EQUAL( config.getInterval(), *Interval::DefaultPtr );
    BOOST_CHECK( ! config.isLive() );

    BOOST_CHECK_EQUAL( oss.str(), "" );
}


BOOST_AUTO_TEST_CASE( test_mFlagFiles )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" )( "-m" )( "file1.log" )( "file2.log" ) );

    BOOST_CHECK( ! config.isHelpRequested() );
    BOOST_REQUIRE_EQUAL( config.getPaths().size(), unsigned(2) );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "file1.log" );
    BOOST_CHECK_EQUAL( config.getPaths()[1], "file2.log" );
    BOOST_CHECK_EQUAL( config.getMapFilenames(), log_merge::MapFilenames::Enabled );
    BOOST_CHECK_EQUAL( config.getInterval(), *Interval::DefaultPtr );
    BOOST_CHECK( ! config.isLive() );

    BOOST_CHECK_EQUAL( oss.str(), "" );
}


BOOST_AUTO_TEST_CASE( test_start_end_flags )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" )( "--start" )( "2010-05-03 16:00:10.757" )( "--end" )( "2010-05-05 16:00:10.757" ) );

    Interval exp_interval(
            boost::posix_time::time_from_string( "2010-05-03 16:00:10.757" ),
            boost::posix_time::time_from_string( "2010-05-05 16:00:10.757" )
        );

    BOOST_CHECK_EQUAL( config.getInterval(), exp_interval );

    BOOST_CHECK_EQUAL( oss.str(), "" );
}


BOOST_AUTO_TEST_CASE( test_help )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" )( "-h" ) );

    BOOST_CHECK( config.isHelpRequested() );
    BOOST_CHECK_EQUAL( oss.str(), HelpText );
}


BOOST_AUTO_TEST_CASE( test_long_help )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    config.configure( list_of( "log_merge" )( "--help" ) );

    BOOST_CHECK( config.isHelpRequested() );
    BOOST_CHECK_EQUAL( oss.str(), HelpText );
}


BOOST_AUTO_TEST_CASE( test_invalidOption )
{
    ostringstream oss;

    Configuration config;

    config.setOutput( oss );

    BOOST_CHECK_THROW( config.configure( list_of( "./log_merge" )( "-z" ) ), std::exception );
}


BOOST_AUTO_TEST_CASE( test_file_names_from_stdin )
{
    Configuration config;

    istringstream iss( "test1.log\n" );

    config.setInput( iss );

    config.configure( list_of( "./log_merge" )( "-" ) );

    BOOST_REQUIRE_EQUAL( config.getPaths().size(), 1U );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "test1.log" );
}


BOOST_AUTO_TEST_CASE( test_file_names_from_stdin_no_names )
{
    Configuration config;

    istringstream iss( "" );

    config.setInput( iss );

    config.configure( list_of( "./log_merge" )( "-" ) );

    BOOST_CHECK( config.getPaths().empty() );
}


BOOST_AUTO_TEST_CASE( test_file_names_from_stdin_multi_names )
{
    Configuration config;

    istringstream iss( "a\nb\nc\nd\n" );

    config.setInput( iss );

    config.configure( list_of( "./log_merge" )( "-" ) );

    BOOST_REQUIRE_EQUAL( config.getPaths().size(), 4U );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "a" );
    BOOST_CHECK_EQUAL( config.getPaths()[1], "b" );
    BOOST_CHECK_EQUAL( config.getPaths()[2], "c" );
    BOOST_CHECK_EQUAL( config.getPaths()[3], "d" );
}


BOOST_AUTO_TEST_CASE( test_file_names_from_stdin_null_terminated )
{
    Configuration config;

    istringstream iss( string() + "a" + '\0' + "b" + '\0' );

    config.setInput( iss );

    config.configure( list_of( "./lm" )( "-0" ) );

    BOOST_REQUIRE_EQUAL( config.getPaths().size(), 2U );
    BOOST_CHECK_EQUAL( config.getPaths()[0], "a" );
    BOOST_CHECK_EQUAL( config.getPaths()[1], "b" );
}


BOOST_AUTO_TEST_CASE( test_live_option )
{
    Configuration config;

    config.configure( list_of( "log_merge" )( "--live" ) );

    BOOST_CHECK( config.isLive() );
}


BOOST_AUTO_TEST_CASE( test_live_with_start_end )
{
    ostringstream oss;

    Configuration config;
    BOOST_CHECK_THROW( config.configure( list_of( "lm" )( "--live" )( "--start" )( "05-05" ) ), std::runtime_error );
    BOOST_CHECK_THROW( config.configure( list_of( "lm" )( "--live" )( "--end" )( "05-05" ) ), std::runtime_error );
}
