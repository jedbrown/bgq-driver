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


#include "StreamPool.hpp"

#include <string>


#define BOOST_TEST_MODULE StreamPool
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using log_merge::StreamPool;
using log_merge::StreamPtr;
using log_merge::StreamWkPtr;

using std::string;


BOOST_AUTO_TEST_CASE( test_get )
{
    // Can get a stream from the stream pool

    StreamPool stream_pool( 1 );

    StreamPtr stream_ptr(stream_pool.getStream( "test_sample/mmcs_two_lines.log" ));

    string line;

    std::getline( *stream_ptr, line );

    BOOST_CHECK_EQUAL( line, "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57" );
}


BOOST_AUTO_TEST_CASE( test_get_same_pooled )
{
    // If get the same stream twice, get the same pointer.

    StreamPool stream_pool( 1 );

    StreamPtr stream_ptr1(stream_pool.getStream( "test_sample/mmcs_two_lines.log" ));
    StreamPtr stream_ptr2(stream_pool.getStream( "test_sample/mmcs_two_lines.log" ));

    BOOST_CHECK_EQUAL( stream_ptr1, stream_ptr2 );
}


BOOST_AUTO_TEST_CASE( test_overflow_invalidates_weak_ptr )
{
    // If overflow the pool, then the first one is removed from the pool.

    StreamPool stream_pool( 1 );

    StreamPtr stream1_ptr(stream_pool.getStream( "test_sample/mmcs_two_lines.log" ));

    StreamWkPtr stream1_wk_ptr( stream1_ptr );

    stream1_ptr.reset();

    BOOST_CHECK( stream1_wk_ptr.lock().get() != NULL );

    StreamPtr stream2_ptr(stream_pool.getStream( "test_sample/bgqsn2-Q05-I7-J02-bgagentd.log" ));

    BOOST_CHECK( stream1_wk_ptr.lock().get() == NULL );
}


BOOST_AUTO_TEST_CASE( test_overflow_removes_least_recently_used )
{
    // If overflow the pool, then the first one is removed from the pool.

    StreamPool stream_pool( 2 );

    StreamWkPtr stream1_wk_ptr(stream_pool.getStream( "test_sample/mmcs_two_lines.log" ));
    StreamWkPtr stream2_wk_ptr(stream_pool.getStream( "test_sample/log_file1.log" ));
    StreamWkPtr stream3_wk_ptr(stream_pool.getStream( "test_sample/log_file2.log" ));

    BOOST_CHECK( stream1_wk_ptr.lock().get() == NULL );

    stream_pool.getStream( "test_sample/log_file1.log" );
    stream_pool.getStream( "test_sample/three_lines.log" );

    BOOST_CHECK( stream2_wk_ptr.lock().get() != NULL );
    BOOST_CHECK( stream3_wk_ptr.lock().get() == NULL );
}


BOOST_AUTO_TEST_CASE( test_set_location )
{
    // Can set the offset when getting the stream.

    StreamPool stream_pool( 1 );

    StreamPtr stream1_1_ptr(stream_pool.getStream( "test_sample/three_lines.log" ));

    string line;

    std::getline( *stream1_1_ptr, line );

    BOOST_CHECK_EQUAL( line, "line1" );

    off_t offset(stream1_1_ptr->tellg());

    stream1_1_ptr.reset();

    StreamPtr stream2_ptr(stream_pool.getStream( "test_sample/log_file1.log" ));
    stream2_ptr.reset();

    StreamPtr stream1_2_ptr(stream_pool.getStream( "test_sample/three_lines.log", offset ));

    std::getline( *stream1_2_ptr, line );

    BOOST_CHECK_EQUAL( line, "line2" );
}
