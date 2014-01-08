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
#define BOOST_TEST_MODULE totalview
#include <boost/test/unit_test.hpp>

#include "test/common.h"

#include "common/JobInfo.h"

#include "client/Debugger.h"
#include "client/MuxConnection.h"

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

using namespace runjob::client;

extern volatile int MPIR_being_debugged;
extern char MPIR_executable_path[256];
extern char MPIR_server_arguments[1024];
extern MPIR_PROCDESC* MPIR_proctable;
extern int MPIR_proctable_size;

BOOST_AUTO_TEST_CASE( ctor )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    BOOST_CHECK_EQUAL( debugger->attached(), false );
}

BOOST_AUTO_TEST_CASE( attach_then_detach )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    // attach
    MPIR_being_debugged = 1;
    BOOST_CHECK_EQUAL( debugger->attached(), true );

    // detach
    MPIR_being_debugged = 0;
    BOOST_CHECK_EQUAL( debugger->attached(), false );
}

BOOST_AUTO_TEST_CASE( server_path )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    const std::string tool( "/bin/foo" );
    MPIR_being_debugged = 1;
    strcpy( MPIR_executable_path, tool.c_str() );
    BOOST_CHECK_EQUAL(
            debugger->tool().getExecutable(),
            tool
            );
    BOOST_CHECK_EQUAL(
            debugger->tool().getArguments().size(),
            0
            );

    BOOST_CHECK_EQUAL( debugger->attached(), true );
}

BOOST_AUTO_TEST_CASE( arguments_without_embedded_null )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    const std::string tool( "/bin/foo" );
    const std::string args( "hello world how are you" );
    MPIR_being_debugged = 1;
    strcpy( MPIR_executable_path, tool.c_str() );
    strcpy( MPIR_server_arguments, args.c_str() );
    BOOST_CHECK_EQUAL(
            debugger->tool().getExecutable(),
            tool
            );
    BOOST_CHECK_EQUAL(
            debugger->tool().getArguments().size(),
            1u
            );
    BOOST_CHECK_EQUAL(
            debugger->tool().getArguments().at(0),
            args
            );
}

BOOST_AUTO_TEST_CASE( arguments_with_embedded_null )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    const std::string tool( "/bin/foo" );
    memset( MPIR_server_arguments, 0, sizeof(MPIR_server_arguments) );
    char* args = MPIR_server_arguments;
    int offset = sprintf( args, "hello" );
    args += offset + 1;
    offset += sprintf( args, "world" );

    MPIR_being_debugged = 1;
    strcpy( MPIR_executable_path, tool.c_str() );
    BOOST_CHECK_EQUAL(
            debugger->tool().getExecutable(),
            tool
            );
    BOOST_CHECK_EQUAL(
            debugger->tool().getArguments().size(),
            2u
            );
    BOOST_CHECK_EQUAL(
            debugger->tool().getArguments().at(0),
            "hello"
            );
    BOOST_CHECK_EQUAL(
            debugger->tool().getArguments().at(1),
            "world"
            );
}

BOOST_AUTO_TEST_CASE( proctable )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    const runjob::message::Proctable::Ptr proctable(
            new runjob::message::Proctable
            );

    const runjob::Uci io_one( "R00-IC-J00" );
    const runjob::Uci io_two( "R00-IC-J01" );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_one, "127.0.0.1" )
                ).second == true
            );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_two, "127.0.0.1" )
                ).second == true
            );

    // add four ranks to proctable, two for each I/O node
    unsigned pid = 0;
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 0, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 1, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 2, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 3, ++pid )
            );

    BOOST_CHECK_EQUAL(
            proctable->_proctable.size(), 4u
            );

    // proctable with fully qualified executable
    runjob::JobInfo info;
    info.setExe( "/bin/false" );
    debugger->fillProctable(
            proctable,
            info
            );

    BOOST_CHECK_EQUAL(
            MPIR_proctable_size, 4u
            );

    for ( int i = 0; i < MPIR_proctable_size; ++i ) {
        MPIR_PROCDESC* p = &MPIR_proctable[i];
        BOOST_CHECK(
                !strcmp( p->host_name, "127.0.0.1" )
                );
        BOOST_CHECK_EQUAL(
                p->executable_name, info.getExe()
                );
    }
}

BOOST_AUTO_TEST_CASE( proctable_relative_path )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    const runjob::message::Proctable::Ptr proctable(
            new runjob::message::Proctable
            );

    const runjob::Uci io_one( "R00-IC-J00" );
    const runjob::Uci io_two( "R00-IC-J01" );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_one, "127.0.0.1" )
                ).second == true
            );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_two, "127.0.0.1" )
                ).second == true
            );

    // add four ranks to proctable, two for each I/O node
    unsigned pid = 0;
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 0, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 1, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 2, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 3, ++pid )
            );

    BOOST_CHECK_EQUAL(
            proctable->_proctable.size(), 4u
            );

    // a job with a relative path for the executable should show a fully qualified
    // path in the proctable
    runjob::JobInfo info;
    info.setExe( "false" );
    info.setCwd( "/bin/" );
    debugger->fillProctable(
            proctable,
            info
            );

    BOOST_CHECK_EQUAL(
            MPIR_proctable_size, 4u
            );

    for ( int i = 0; i < MPIR_proctable_size; ++i ) {
        MPIR_PROCDESC* p = &MPIR_proctable[i];
        BOOST_CHECK(
                !strcmp( p->host_name, "127.0.0.1" )
                );
        BOOST_CHECK_EQUAL(
                p->executable_name, info.getCwd() + info.getExe() 
                );
    }
}

BOOST_AUTO_TEST_CASE( proctable_relative_path_no_slash )
{
    boost::asio::io_service io_service;
    Debugger::Ptr debugger;
    BOOST_CHECK_NO_THROW(
            debugger = Debugger::create( io_service )
            );

    const runjob::message::Proctable::Ptr proctable(
            new runjob::message::Proctable
            );

    const runjob::Uci io_one( "R00-IC-J00" );
    const runjob::Uci io_two( "R00-IC-J01" );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_one, "127.0.0.1" )
                ).second == true
            );
    BOOST_CHECK(
            proctable->_io.insert(
                std::make_pair( io_two, "127.0.0.1" )
                ).second == true
            );

    // add four ranks to proctable, two for each I/O node
    unsigned pid = 0;
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 0, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_one, 1, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 2, ++pid )
            );
    proctable->_proctable.push_back(
            runjob::tool::Rank( io_two, 3, ++pid )
            );

    BOOST_CHECK_EQUAL(
            proctable->_proctable.size(), 4u
            );

    // a job with a relative path for the executable should show a fully qualified
    // path in the proctable
    runjob::JobInfo info;
    info.setExe( "false" );
    info.setCwd( "/bin" );
    debugger->fillProctable(
            proctable,
            info
            );

    BOOST_CHECK_EQUAL(
            MPIR_proctable_size, 4u
            );

    for ( int i = 0; i < MPIR_proctable_size; ++i ) {
        MPIR_PROCDESC* p = &MPIR_proctable[i];
        BOOST_CHECK(
                !strcmp( p->host_name, "127.0.0.1" )
                );
        BOOST_CHECK_EQUAL(
                // resulting path should have the forward slash between
                p->executable_name, info.getCwd() + "/" + info.getExe() 
                );
    }
}
