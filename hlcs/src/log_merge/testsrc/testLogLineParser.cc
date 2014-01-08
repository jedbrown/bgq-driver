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


#include "LogLineParser.hpp"

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define BOOST_TEST_MODULE LogLineParser
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


#include <string>


using log_merge::LogLineParser;

using std::string;


BOOST_AUTO_TEST_CASE( test_parse_valid_fmt )
{
    LogLineParser log_line_parser;

    string line( "2010-05-04 16:00:10.757 (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57" );

    bool has_time;
    boost::posix_time::ptime time;
    string remaining;

    log_line_parser.parse( line, &has_time, &time, &remaining );

    BOOST_REQUIRE( has_time );
    BOOST_CHECK_EQUAL( time, boost::posix_time::time_from_string( "2010-05-04 16:00:10.757" ) );
    BOOST_CHECK_EQUAL( remaining, " (INFO ) [0x2b477594d890] ibm.mmcs.mmcs_server: MMCS[26651]: starting: BG/Q mmcs_server bgq (revision 23365) Apr 29 2010 16:17:57" );
}


BOOST_AUTO_TEST_CASE( test_parse_no_date )
{
    LogLineParser log_line_parser;

    string line( "this line might come after a line with a timestamp" );

    bool has_time;
    boost::posix_time::ptime time;
    string remaining;

    log_line_parser.parse( line, &has_time, &time, &remaining );

    BOOST_CHECK( ! has_time );
}
