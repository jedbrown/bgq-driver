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


#include "DirectoryInfo.hpp"
#include "types.hpp"

#include <boost/assign.hpp>


#define BOOST_TEST_MODULE DirectoryInfo
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace boost::assign;
using namespace log_merge;


BOOST_AUTO_TEST_CASE( test_regular_file )
{
    // Can be a regular file, just opens the file.

    Paths paths = list_of( "test_sample/log_file1.log" );
    DirectoryInfo di( paths );

    DirectoryInfo exp_di;
    exp_di.set( "test_sample/log_file1.log", 258 );

    BOOST_CHECK_EQUAL( di, exp_di );
}


BOOST_AUTO_TEST_CASE( test_read_directory )
{
    // Can read a directory given directory path, ignores symlink in the directory

    Paths paths = list_of( "test_sample/d1" );

    DirectoryInfo di( paths );

    DirectoryInfo exp_di;
    exp_di.set( "test_sample/d1/log_file_d1_1.log", 258 );

    BOOST_CHECK_EQUAL( di, exp_di );
}


BOOST_AUTO_TEST_CASE( test_multi_dirs )
{
    // Can create a DirectoryInfo with multiple paths

    Paths paths = list_of( "test_sample/d1" )( "test_sample/d2" );

    DirectoryInfo di( paths );

    DirectoryInfo exp_di;
    exp_di.set( "test_sample/d1/log_file_d1_1.log", 258 );
    exp_di.set( "test_sample/d2/log_file_d2_1.log", 103 );

    BOOST_CHECK_EQUAL( di, exp_di );
}


BOOST_AUTO_TEST_CASE( test_dir_no_exist )
{
    // If the path doesn't exist then ignored.

    Paths paths = list_of( "notafile" );

    BOOST_CHECK_EQUAL( DirectoryInfo( paths ), DirectoryInfo() );
}


BOOST_AUTO_TEST_CASE( test_get_changes )
{
    // Can compare two directory infos and get the changed and new files.

    DirectoryInfo di1;
    di1.set( "f1", 1 ); // changes
    di1.set( "f2", 1 ); // stays same

    DirectoryInfo di2;
    di2.set( "f1", 2 ); // size changes
    di2.set( "f2", 1 ); // stays same
    di2.set( "f3", 1 ); // new

    Paths changed_paths(DirectoryInfo::calcChanges( di1, di2 ));

    Paths exp_changed_paths;
    exp_changed_paths.push_back( "f1" );
    exp_changed_paths.push_back( "f3" );

    BOOST_REQUIRE_EQUAL( changed_paths.size(), exp_changed_paths.size() );
    BOOST_CHECK( std::equal( changed_paths.begin(), changed_paths.end(), exp_changed_paths.begin() ) );
}


BOOST_AUTO_TEST_CASE( test_update_positions )
{
    // Can update the positions of files and add new files.

    DirectoryInfo di;
    di.set( "f1", 1 );
    di.set( "f2", 1 );

    CurrentPositions current_positions;
    current_positions["f1"] = 100;
    current_positions["f3"] = 200;

    di.update( current_positions );

    DirectoryInfo exp_di;
    exp_di.set( "f1", 100 );
    exp_di.set( "f2", 1 );
    exp_di.set( "f3", 200 );

    BOOST_CHECK_EQUAL( di, exp_di );
}
