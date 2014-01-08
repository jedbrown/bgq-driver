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
#define BOOST_TEST_MODULE file_output

#include "client/options/Label.h"
#include "client/options/Parser.h"
#include "client/FileOutput.h"
#include "common/message/StdIo.h"

#include "test/client/Fixture.h"
#include "test/make_argv.h"

#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>

#include <cstdlib>

#include <unistd.h>

using namespace runjob::client::options;
using namespace runjob::client;

BOOST_FIXTURE_TEST_SUITE( file_output, Fixture )

BOOST_AUTO_TEST_CASE( short_prefix )
{
    _argv = make_argv("--label short", _argc);
    Parser::Ptr options(
            new Parser( _argc, _argv ) 
            );

    int fd[2];
    (void)pipe( fd );
    boost::asio::io_service io_service;
    const Output::Ptr foo(
            new FileOutput( options, io_service, fd[1], "label" )
            );
    std::string output( "hello world\n" );

    const runjob::message::StdIo::Ptr msg(
            new runjob::message::StdIo
            );
    msg->setData( output.c_str(), output.size() );
    msg->setRank( 1 );

    foo->write( msg );

    io_service.run();

    // we expect to see: 1: hello world\n
    char buf[16];
    memset(buf, 0, sizeof(buf));
    BOOST_CHECK_EQUAL(
            ::read( fd[0], buf, sizeof(buf) ),
            15
            );
    const std::string expected( "1: " + output );
    BOOST_CHECK_EQUAL(
            buf,
            expected
            );
}

BOOST_AUTO_TEST_CASE( long_prefix )
{
    _argv = make_argv("--label long", _argc);
    Parser::Ptr options(
            new Parser( _argc, _argv ) 
            );

    int fd[2];
    (void)pipe( fd );
    boost::asio::io_service io_service;
    const Output::Ptr foo(
            new FileOutput( options, io_service, fd[1], "label" )
            );
    std::string output( "hello world\n" );

    const runjob::message::StdIo::Ptr msg(
            new runjob::message::StdIo
            );
    msg->setData( output.c_str(), output.size() );
    msg->setRank( 5 );

    foo->write( msg );

    io_service.run();

    // we expect to see: label[5]: hello world\n
    char buf[23];
    memset(buf, 0, sizeof(buf));
    BOOST_CHECK_EQUAL(
            ::read( fd[0], buf, sizeof(buf) ),
            22
            );
    BOOST_CHECK_EQUAL(
            buf,
            "label[5]: " + output
            );
}

BOOST_AUTO_TEST_CASE( short_prefix_block_output )
{
    _argv = make_argv("--label short", _argc);
    Parser::Ptr options(
            new Parser( _argc, _argv ) 
            );

    int fd[2];
    (void)pipe( fd );
    boost::asio::io_service io_service;
    const Output::Ptr foo(
            new FileOutput( options, io_service, fd[1], "label" )
            );
    std::string output( "hello world\n" );
    for ( unsigned i = 0; i < output.size(); ++i ) {
        const runjob::message::StdIo::Ptr msg(
                new runjob::message::StdIo
                );
        msg->setData( output.c_str() + i, 1 );
        msg->setRank( 1 );

        foo->write( msg );
    }

    io_service.run();

    // we expect to see: 1: hello world\n
    char buf[16];
    memset(buf, 0, sizeof(buf));
    BOOST_CHECK_EQUAL(
            ::read( fd[0], buf, sizeof(buf) ),
            15
            );
    BOOST_CHECK_EQUAL(
            buf,
            "1: " + output
            );
}

BOOST_AUTO_TEST_CASE( long_prefix_block_output )
{
    _argv = make_argv("--label long", _argc);
    Parser::Ptr options(
            new Parser( _argc, _argv ) 
            );

    int fd[2];
    (void)pipe( fd );
    boost::asio::io_service io_service;
    const Output::Ptr foo(
            new FileOutput( options, io_service, fd[1], "label" )
            );
    std::string output( "hello world\nhello world\nhello world\n" );
    const runjob::message::StdIo::Ptr msg(
            new runjob::message::StdIo
            );
    msg->setData( output.c_str(), output.size() );
    msg->setRank( 22 );

    foo->write( msg );

    io_service.run();

    char buf[70];
    memset(buf, 0, sizeof(buf));
    BOOST_CHECK_EQUAL(
            ::read( fd[0], buf, sizeof(buf) ),
            69
            );
    BOOST_CHECK_EQUAL(
            buf,
            "label[22]: hello world\n"
            "label[22]: hello world\n"
            "label[22]: hello world\n"
            );
}

BOOST_AUTO_TEST_SUITE_END()

