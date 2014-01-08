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
#define BOOST_TEST_MODULE convert

#include <boost/test/unit_test.hpp>

#include "common/commands/convert.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

using namespace runjob::commands;

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

struct Fixture {
    Fixture() :
        _header(),
        _msg(),
        _buf(),
        _os(&_buf)
    {
        bzero(&_header, sizeof(_header));
    }

    Message::Ptr setHeader() {
        _msg->serialize(_os);
        _header._type = _msg->getType();
        _header._tag = _msg->getTag();

        return runjob::commands::convert( _header, _buf );
    }
    
    Header _header;
    Message::Ptr _msg;
    boost::asio::streambuf _buf;
    std::ostream _os;
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_SUITE( convert, Fixture )

BOOST_AUTO_TEST_CASE( job_acl_request )
{
    // create empty message
    _msg.reset( new request::JobAcl() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::JobAcl>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( job_acl_response )
{
    // create empty message
    _msg.reset( new response::JobAcl() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::JobAcl>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( job_status_request )
{
    // create empty message
    _msg.reset( new request::JobStatus() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::JobStatus>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( job_status_response )
{
    // create empty message
    _msg.reset( new response::JobStatus() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::JobStatus>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( kill_job_request )
{
    // create empty message
    _msg.reset( new request::KillJob() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::KillJob>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( kill_job_response )
{
    // create empty message
    _msg.reset( new response::KillJob() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::KillJob>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( refresh_config_request )
{
    // create empty messag
    _msg.reset( new request::RefreshConfig() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::RefreshConfig>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( refresh_config_response )
{
    // create empty message
    _msg.reset( new response::RefreshConfig() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::RefreshConfig>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( mux_status_request )
{
    // create empty message
    _msg.reset( new request::MuxStatus() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::MuxStatus>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( mux_status_response )
{
    // create empty message
    _msg.reset( new response::MuxStatus() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::MuxStatus>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( server_status_request )
{
    // create empty message
    _msg.reset( new request::ServerStatus() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::ServerStatus>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( server_status_response )
{
    // create empty message
    _msg.reset( new response::ServerStatus() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::ServerStatus>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( log_level_request )
{
    // create empty message
    _msg.reset( new request::LogLevel() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<request::LogLevel>(result) != NULL
            );
}

BOOST_AUTO_TEST_CASE( log_level_response )
{
    // create empty message
    _msg.reset( new response::LogLevel() );

    // set header
    Message::Ptr result = setHeader();

    // ensure it worked
    BOOST_REQUIRE(
            boost::dynamic_pointer_cast<response::LogLevel>(result) != NULL
            );
}

BOOST_AUTO_TEST_SUITE_END()
