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
#include "LogFile.hpp"
#include "StreamPool.hpp"


#define BOOST_TEST_MODULE LogFile
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


#include <stdexcept>
#include <string>


using namespace log_merge;

using boost::posix_time::time_from_string;

using std::runtime_error;
using std::string;


static StreamPool s_stream_pool;


BOOST_AUTO_TEST_CASE( test_logfile )
{
    LogFile logfile( "test_sample/mmcs_two_lines.log", s_stream_pool );
    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getTimestamp(), time_from_string( "2010-05-04 16:00:10.757" ) );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(), "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57" );

    logfile.advance();

    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getTimestamp(), time_from_string( "2010-05-04 16:00:10.758" ) );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(), "2010-05-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server" );

    logfile.advance();
    BOOST_CHECK( ! logfile.hasRecord() );
}

BOOST_AUTO_TEST_CASE( test_empty_logfile )
{
    LogFile logfile( "test_sample/empty.log", s_stream_pool );

    BOOST_CHECK( ! logfile.hasRecord() );
}

BOOST_AUTO_TEST_CASE( test_not_empty_no_log_record )
{
    LogFile logfile( "test_sample/no_log_record.log", s_stream_pool );

    BOOST_REQUIRE( ! logfile.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_multi_line_log_record )
{
    LogFile logfile( "test_sample/multi_line.log", s_stream_pool );

    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getTimestamp(), time_from_string( "2010-05-05 01:49:04.523" ) );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(),

"2010-05-05 01:49:04.523 (INFO ) [queenBee] mcservermain [0x400015f51d0] -\n"
"clientPort=1208\n"
"machineType=0\n"
"hostName=bgpfs29\n"
"computeRacks=16\n"
"ioRacks=1\n"
"rows=4\n"
"columns=5\n"
"debug=0\n"
"autoinit=1\n"
"bringupOptions=DoMcEmulationWithoutHardware\n"
"alteraPath=/bgsys/drivers/ppcfloor/bin/alteraImages\n"
"Subnet Subnet1\n"
"  PrimaryServer = bgpfs32\n"
"  PrimaryServerInterface = eth1\n"
"  PrimaryServerPort = 33458\n"
"  PrimaryBGInterface = eth0\n"
"  BackupServer = bgpfs33\n"
"  BackupServerInterface = eth1\n"
"  BackupServerPort = 33458\n"
"  BackupBGInterface = eth0\n"
"  HardwareToManage = R00, R01, R02, R03, R10, R11, R12, R13, Q14,\n"
"Subnet Subnet2\n"
"  PrimaryServer = bgpfs33\n"
"  PrimaryServerInterface = eth1\n"
"  PrimaryServerPort = 33459\n"
"  PrimaryBGInterface = eth0\n"
"  BackupServer = bgpfs32\n"
"  BackupServerInterface = eth1\n"
"  BackupServerPort = 33459\n"
"  BackupBGInterface = eth0\n"
"  HardwareToManage = R20, R21, R22, R23, R30, R31, R32, R33,\n"

        );
}


BOOST_AUTO_TEST_CASE( test_first_lines_not_log_record )
{
    LogFile logfile( "test_sample/starts_with_not_record.log", s_stream_pool );

    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getTimestamp(), time_from_string( "2010-05-04 16:00:10.757" ) );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(), "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57" );
}


BOOST_AUTO_TEST_CASE( test_interval1 )
{
    // Can pass an interval to LogFile. If contains an entry within the interval then that's the entry.

    LogFile logfile(
            "test_sample/interval1.log",
            s_stream_pool,
            Interval::create(
                    time_from_string( "2010-05-03 16:00:10.757" ),
                    time_from_string( "2010-05-05 16:00:10.757" )
                )
        );

    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getTimestamp(), time_from_string( "2010-05-04 16:00:10.757" ) );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(), "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line2" );

    logfile.advance();

    BOOST_CHECK( ! logfile.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_mappedName )
{
    LogFile logfile(
            "test_sample/bgqsn2-Q05-I7-J02-bgagentd.log",
            s_stream_pool,
            Interval::DefaultPtr,
            FilenameMapper( MapFilenames::Enabled )
        );

    BOOST_CHECK_EQUAL( logfile.getLabel(), "bgagentd      " );
}


BOOST_AUTO_TEST_CASE( test_notMappedName )
{
    LogFile logfile(
            "test_sample/bgqsn2-Q05-I7-J02-bgagentd.log",
            s_stream_pool,
            Interval::DefaultPtr,
            FilenameMapper( MapFilenames::Disabled )
        );

    BOOST_CHECK_EQUAL( logfile.getLabel(), "bgqsn2-Q05-I7-J02-bgagentd.log" );
}


BOOST_AUTO_TEST_CASE( test_fileAtEnd )
{
    LogFile logfile(
            "test_sample/log_file1.log",
            258,
            s_stream_pool
        );

    BOOST_CHECK( ! logfile.hasRecord() );
}


BOOST_AUTO_TEST_CASE( test_update )
{
    LogFile logfile(
            "test_sample/update.log",
            78,
            s_stream_pool,
            Interval::create(
                    boost::posix_time::min_date_time,
                    time_from_string( "2010-05-05 16:00:10.757" )
                )
        );

    BOOST_CHECK( ! logfile.hasRecord() );

    logfile.update(
            Interval::create(
                    boost::posix_time::min_date_time,
                    time_from_string( "2010-06-05 16:00:10.757" )
                )
        );

    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(), "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line2" );

    BOOST_CHECK_EQUAL( logfile.getCurrentPosition(), 78 );

    logfile.advance();

    BOOST_REQUIRE( logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentRecord().getText(), "2010-06-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: line3" );

    logfile.advance();

    BOOST_CHECK( ! logfile.hasRecord() );
    BOOST_CHECK_EQUAL( logfile.getCurrentPosition(), 232 );
}
