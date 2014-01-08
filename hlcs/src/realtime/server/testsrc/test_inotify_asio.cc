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

#include "Inotify.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <iostream>

#include <unistd.h> // sleep, unlink

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE inotify_asio
#include <boost/test/unit_test.hpp>

using boost::bind;
using boost::shared_ptr;

using std::cerr;
using std::ofstream;
using std::string;

const string WORK_DIR_NAME( "work/" );

BOOST_AUTO_TEST_CASE( sync_dir_create )
{
    // Should be able to synchronously monitor for a file being created.

    const string filename( "sync_dir_create" ), file_path(WORK_DIR_NAME + filename);

    boost::asio::io_service io;

    Inotify inotify( io );

    Inotify::Watch::Ptr w_ptr(inotify.watch( WORK_DIR_NAME, IN_CREATE ));

    // create a file.
    ofstream ofs( file_path.c_str() );
    shared_ptr<void> f_g(static_cast<void*>(0), bind( &unlink, file_path.c_str() ));

    ofs << "test stuff." << std::endl;

    Inotify::Events events;
    boost::system::error_code error;

    inotify.read( events, error );

    BOOST_REQUIRE( ! error );
    BOOST_REQUIRE_EQUAL( events.size(), 1 );
    BOOST_CHECK_EQUAL( events[0].mask, IN_CREATE );
    BOOST_CHECK_EQUAL( events[0].name, filename );
    BOOST_CHECK_EQUAL( events[0].watch_ptr.lock(), w_ptr );
}

static void readFinished(
        const boost::system::error_code& error_in,
        boost::system::error_code& error_out
    )
{
    error_out = error_in;
}

BOOST_AUTO_TEST_CASE( async_dir_create )
{
    const string filename( "async_dir_create" ), file_path(WORK_DIR_NAME + filename);

    boost::asio::io_service io;

    Inotify inotify( io );

    Inotify::Watch::Ptr w_ptr(inotify.watch( WORK_DIR_NAME, IN_CREATE ));

    // create a file.
    ofstream ofs( file_path.c_str() );
    shared_ptr<void> f_g(static_cast<void*>(0), bind( &unlink, file_path.c_str() ));

    ofs << "test stuff.\n";
    ofs.close();

    Inotify::Events events;
    boost::system::error_code error;

    inotify.async_read( events, boost::bind( &readFinished, boost::asio::placeholders::error, boost::ref(error) ) );

    io.poll();

    BOOST_REQUIRE( ! error );
    BOOST_REQUIRE_EQUAL( events.size(), 1 );
    BOOST_CHECK_EQUAL( events[0].mask, IN_CREATE );
    BOOST_CHECK_EQUAL( events[0].name, filename );
    BOOST_CHECK_EQUAL( events[0].watch_ptr.lock(), w_ptr );
}

BOOST_AUTO_TEST_CASE( async_no_change )
{
    // If we're watching for an event and none occurs then should get no event.

    boost::asio::io_service io;

    Inotify inotify( io );

    Inotify::Watch::Ptr w_ptr(inotify.watch( WORK_DIR_NAME, IN_CREATE ));

    Inotify::Events events;
    boost::system::error_code error;

    inotify.async_read( events, boost::bind( &readFinished, _1, boost::ref(error) ) );

    io.poll();

    BOOST_CHECK( events.empty() );
}

BOOST_AUTO_TEST_CASE( two_files )
{
    // Should be able to watch two files and get the correct Watch::Ptr when two files are changed.

    const string filename1( "two_files_1" ), file_path1(WORK_DIR_NAME + filename1);
    const string filename2( "two_files_2" ), file_path2(WORK_DIR_NAME + filename2);

    boost::asio::io_service io;

    Inotify inotify( io );

    // create a couple temporary files

    ofstream ofs1( file_path1.c_str() );
    shared_ptr<void> f1_g(static_cast<void*>(0), bind( &unlink, file_path1.c_str() ));

    ofs1 << "test stuff.\n";

    ofstream ofs2( file_path2.c_str() );
    shared_ptr<void> f2_g(static_cast<void*>(0), bind( &unlink, file_path2.c_str() ));

    ofs2 << "test stuff.\n";

    // Watch those files!
    Inotify::Watch::Ptr w_ptr1(inotify.watch( file_path1, IN_MODIFY ));
    Inotify::Watch::Ptr w_ptr2(inotify.watch( file_path2, IN_MODIFY ));

    // modify the file.
    ofs1 << "more stuff\n";
    ofs1.close();

    // modify the file.
    ofs2 << "more stuff\n";
    ofs2.close();

    Inotify::Events events;
    boost::system::error_code error;

    inotify.read( events, error );

    BOOST_REQUIRE( ! error );
    BOOST_REQUIRE_EQUAL( events.size(), 2 );

    BOOST_CHECK_EQUAL( events[0].mask, IN_MODIFY );
    BOOST_CHECK_EQUAL( events[0].name, "" );
    BOOST_CHECK_EQUAL( events[0].watch_ptr.lock(), w_ptr1 );

    BOOST_CHECK_EQUAL( events[1].mask, IN_MODIFY );
    BOOST_CHECK_EQUAL( events[1].name, "" );
    BOOST_CHECK_EQUAL( events[1].watch_ptr.lock(), w_ptr2 );
}

BOOST_AUTO_TEST_CASE( separate_mods )
{
    // If make mods to a file between reads should get both of them.

    const string filename( "separate_mods" ), file_path(WORK_DIR_NAME + filename);

    boost::asio::io_service io;

    Inotify inotify( io );

    ofstream ofs1( file_path.c_str() );
    shared_ptr<void> f_g(static_cast<void*>(0), bind( &unlink, file_path.c_str() ));

    ofs1 << "test stuff." << std::endl;

    // Start watching...

    Inotify::Watch::Ptr w_ptr1(inotify.watch( file_path, IN_MODIFY ));

    // modify the file.
    ofs1 << "more stuff" << std::endl;

    // grab the event.

    Inotify::Events events;
    boost::system::error_code error;

    inotify.read( events, error );

    BOOST_REQUIRE( ! error );
    BOOST_REQUIRE_EQUAL( events.size(), 1 );

    BOOST_CHECK_EQUAL( events[0].mask, IN_MODIFY );
    BOOST_CHECK_EQUAL( events[0].name, "" );
    BOOST_CHECK_EQUAL( events[0].watch_ptr.lock(), w_ptr1 );

    // modify the file again.
    ofs1 << "more stuff2" << std::endl;

    // grab the next event.

    inotify.read( events, error );

    BOOST_REQUIRE( ! error );
    BOOST_REQUIRE_EQUAL( events.size(), 1 );

    BOOST_CHECK_EQUAL( events[0].mask, IN_MODIFY );
    BOOST_CHECK_EQUAL( events[0].name, "" );
    BOOST_CHECK_EQUAL( events[0].watch_ptr.lock(), w_ptr1 );
}

BOOST_AUTO_TEST_CASE( remove_watch )
{
    // If remove a watch then shouldn't get an event when the file is changed.

    const string filename( "remove_watch" ), file_path(WORK_DIR_NAME + filename);

    boost::asio::io_service io;

    Inotify inotify( io );

    ofstream ofs1( file_path.c_str() );
    shared_ptr<void> f_g(static_cast<void*>(0), bind( &unlink, file_path.c_str() ));

    Inotify::Watch::Ptr w_ptr1(inotify.watch( file_path, IN_MODIFY ));

    // modify the file.
    ofs1 << "more stuff" << std::endl;

    // grab the event.

    Inotify::Events events;
    boost::system::error_code error;

    inotify.read( events, error );

    BOOST_REQUIRE( ! error );
    BOOST_REQUIRE_EQUAL( events.size(), 1 );

    BOOST_CHECK_EQUAL( events[0].mask, IN_MODIFY );
    BOOST_CHECK_EQUAL( events[0].name, "" );
    BOOST_CHECK_EQUAL( events[0].watch_ptr.lock(), w_ptr1 );

    // remove the watch.
    w_ptr1.reset();

    // modify the file again.
    ofs1 << "more stuff2" << std::endl;

    inotify.read( events, error );

    BOOST_REQUIRE( ! error );
    BOOST_CHECK( events.empty() );
}

BOOST_AUTO_TEST_CASE( error_no_auth )
{
    // If try to watch a directory that don't have access to should get an error.

    boost::asio::io_service io;

    Inotify inotify( io );

    BOOST_CHECK_THROW( inotify.watch( "/var/log/httpd", IN_MODIFY ), std::runtime_error );
}

BOOST_AUTO_TEST_CASE( oneshot )
{
    // should be able to do a oneshot.

    const string filename( "oneshot" ), file_path(WORK_DIR_NAME + filename);

    boost::asio::io_service io;

    Inotify inotify( io );

    // create the file
    ofstream ofs1( file_path.c_str() );
    shared_ptr<void> f_g(static_cast<void*>(0), bind( &unlink, file_path.c_str() ));

    // watch the file with oneshot
    Inotify::Watch::Ptr w_ptr1(inotify.watch( file_path, IN_MODIFY | IN_ONESHOT ));

    // modify the file
    ofs1 << "more stuff" << std::endl;

    // get the event

    Inotify::Events events;
    boost::system::error_code error;

    inotify.async_read( events, boost::bind( &readFinished, _1, boost::ref(error) ) );

    cerr << "MODIFY ONESHOT POLL:\n";

    io.poll();

    cerr << "MODIFY ONESHOT POLL DONE\n";

    BOOST_REQUIRE( ! error );
    BOOST_CHECK_EQUAL( events.size(), 1 );

    // modify the file again
    ofs1 << "more stuff" << std::endl;

    // should not get another event.

    inotify.async_read( events, boost::bind( &readFinished, _1, boost::ref(error) ) );

    io.poll();

    BOOST_REQUIRE( ! error );
    BOOST_CHECK( events.empty() );
}
