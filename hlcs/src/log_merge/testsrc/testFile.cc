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


#include "File.hpp"
#include "StreamPool.hpp"

#include <stdexcept>
#include <string>


#define BOOST_TEST_MODULE File
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using log_merge::File;

using std::runtime_error;
using std::string;


static log_merge::StreamPool s_stream_pool;


BOOST_AUTO_TEST_CASE( test_readFile )
{
    File f( "test_sample/mmcs_two_lines.log", s_stream_pool );

    BOOST_CHECK_EQUAL( f.getPath(), "test_sample/mmcs_two_lines.log" );

    BOOST_REQUIRE( f.hasCurrentLine() );

    BOOST_CHECK_EQUAL( f.getCurrentLine(), "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57" );

    f.advance();

    BOOST_REQUIRE( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "2010-05-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_server" );

    f.advance();

    BOOST_CHECK( ! f.hasCurrentLine() );
}


BOOST_AUTO_TEST_CASE( test_lastLine )
{
    File f( "test_sample/three_lines.log", s_stream_pool );

    f.toLastLine();

    BOOST_REQUIRE( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "line3" );

    f.toPreviousLine();

    BOOST_REQUIRE( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "line2" );

    f.toPreviousLine();

    BOOST_REQUIRE( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "line1" );

    f.toPreviousLine();

    BOOST_CHECK( ! f.hasCurrentLine() );
}


BOOST_AUTO_TEST_CASE( test_lastLine_empty_file )
{
    File f( "test_sample/empty.log", s_stream_pool );

    f.toLastLine();

    BOOST_CHECK( ! f.hasCurrentLine() );
}


BOOST_AUTO_TEST_CASE( test_lastLine_one_line )
{
    File f( "test_sample/one_line.log", s_stream_pool );

    f.toLastLine();

    BOOST_REQUIRE( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "2010-05-04 16:00:10.758 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: Startup parameters: ./mmcs_serv" );
}


BOOST_AUTO_TEST_CASE( test_fileDoesntExist )
{
    BOOST_CHECK_THROW( File( "test_sample/filedoesntexist.log", s_stream_pool ), runtime_error );
}


BOOST_AUTO_TEST_CASE( test_filesSame )
{
    BOOST_CHECK( File( "test_sample/log_file1.log", s_stream_pool ).isSameFileAs( "test_sample/log_file1.log" ) );
}


BOOST_AUTO_TEST_CASE( test_filesDifferent )
{
    BOOST_CHECK( ! File( "test_sample/log_file1.log", s_stream_pool ).isSameFileAs( "test_sample/log_file2.log" ) );
}


BOOST_AUTO_TEST_CASE( test_filesSameSymlink )
{
    BOOST_CHECK( File( "test_sample/log_file1.log", s_stream_pool ).isSameFileAs( "test_sample/symlink_to_log_file1.log" ) );
}


BOOST_AUTO_TEST_CASE( test_OpenAtEndOfFile )
{
    File f( "test_sample/log_file1.log", s_stream_pool, 258 ); // The file is 258 bytes.

    BOOST_CHECK( ! f.hasCurrentLine() );

    BOOST_CHECK_EQUAL( f.getCurrentPosition(), 258 );
}


BOOST_AUTO_TEST_CASE( test_OpenAtLine )
{
    File f( "test_sample/three_lines.log", s_stream_pool, 6 ); // second line starts at 6.

    BOOST_CHECK( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "line2" );
    BOOST_CHECK_EQUAL( f.getCurrentPosition(), 6 );

    f.advance();
    BOOST_CHECK( f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentLine(), "line3" );
    BOOST_CHECK_EQUAL( f.getCurrentPosition(), 12 );

    f.advance();
    BOOST_CHECK( ! f.hasCurrentLine() );
    BOOST_CHECK_EQUAL( f.getCurrentPosition(), 18 );
}
